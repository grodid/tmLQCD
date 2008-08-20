/* $Id$ */
#ifdef HAVE_CONFIG_H
# include<config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef MPI
# include <mpi.h>
#endif
#include "global.h"
#include "complex.h"
#include "deflation/deflation_block.h"
#include "linalg/blas.h"
#include "little_D.h"


/* assume we have a little field w                       */
/* which has length 9*no_blocks*N_s                      */
/* with usual order in space                             */
/* no_blocks = 2 currently fixed                         */
/* and blocks devide z-direction by 2                    */
/*                                                       */
/* block[0], block[1], block[0], block[1], block[0]  ... */
/* local             , +t                , -t        ... */
/*                                                       */
/* block[0], block[1], block[0], block[1]                */
/* +z                , -z                                */
/* wasting some memory here...                           */

const int no_blocks = 2;
const int nblks_t = 1;
const int nblks_x = 1;
const int nblks_y = 1;
const int nblks_z = 2;
int nblks_dir[4] = {1,1,1,2};

/* some lapack related stuff */
static int ONE = 1;
static complex CONE, CZERO, CMONE;

void init_little_field_exchange(complex * w);
void wait_little_field_exchange(const int mu);

void little_D(complex * v, complex *w) {
  int i, j, k, sq = g_N_s*g_N_s;
  CONE.re = 1.;
  CONE.im = 0.;
  CMONE.re = -1.;
  CMONE.im = 0.;
  CZERO.re = 0.;
  CZERO.im = 0.;
  
#ifdef MPI
  init_little_field_exchange(w);
#endif
  /* all the mpilocal stuff first */
  for(i = 0; i < no_blocks; i++) {
    /* diagonal term */
    _FT(zgemv)("N", &g_N_s, &g_N_s, &CONE, g_blocks[i].little_dirac_operator,
	       &g_N_s, w + i*g_N_s, &ONE, &CZERO, v + i*g_N_s, &ONE, 1);

    /* offdiagonal terms */
    for(j = 0; j < 8; j++) {
      /* set k to neighbour in direction j */
      k = g_blocks[i].mpilocal_neighbour[j];
      /* if k is on the same mpi proc, but not myself */
      if(k > -1 && k != i) {
	_FT(zgemv)("N", &g_N_s, &g_N_s, &CONE, g_blocks[i].little_dirac_operator + (j+1)*sq,
		   &g_N_s, w + k*g_N_s, &ONE, &CONE, v + i*g_N_s, &ONE, 1);
      }
    }
  }

#ifdef MPI
  /* now all non-mpilocal stuff */
  /* start with z direction     */
  for(j = 7; j > -1; j--) {
    wait_little_field_exchange(j);
    for(i = 0; i < no_blocks; i++) {
      k = g_blocks[i].mpilocal_neighbour[j];
      if(k < 0) {
	_FT(zgemv)("N", &g_N_s, &g_N_s, &CONE, g_blocks[i].little_dirac_operator + (j+1)*sq,
		   &g_N_s, w + ((j+1)*no_blocks + i)*g_N_s, &ONE, &CONE, v + i*g_N_s, &ONE, 1);
      }
    }
  }
  
#endif
  return;
}


#ifdef MPI
MPI_Request lrequests[16];
MPI_Status lstatus[16];
int waitcount = 0;
#endif

void init_little_field_exchange(complex * w) {
#ifdef MPI
  int i = 0;
#  ifdef PARALLELT
  int no_dirs = 2;
#  elif defined PARALLELXT
  int no_dirs = 4;
#  elif (defined PARALLELXYT || defined PARALLELXYZT)
  int no_dirs = 6;
#  endif
  if(waitcount != 0) {
    if(g_proc_id == 0) {
      fprintf(stderr, "last little_field_exchange not finished! Aborting...\n");
    }
    exit(-1);
  }
  /* z-dir requires special treatment! */
  for(i = 0; i < no_dirs; i+=2) {
    /* send to the right, receive from the left */
    MPI_Isend((void*)w, no_blocks*g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[i], 
	      i, g_cart_grid, &lrequests[2*i]);
    MPI_Irecv((void*)(w + no_blocks*(i+2)*g_N_s), no_blocks*g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[i+1], 
	      i, g_cart_grid, &lrequests[2*i+1]);
    
    /* send to the left, receive from the right */
    MPI_Isend((void*)w, no_blocks*g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[i+1], 
	      i+1, g_cart_grid, &lrequests[2*i+2]);
    MPI_Irecv((void*)(w + no_blocks*(i+1)*g_N_s), no_blocks*g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[i], 
	      i+1, g_cart_grid, &lrequests[2*i+3]);
    waitcount += 4;
  }
#  ifdef PARALLELXYZT
  /* send to the right, receive from the left */
  i = 6;
  MPI_Isend((void*)(w + g_N_s), g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[i], 
	    i, g_cart_grid, &lrequests[2*i]);
  MPI_Irecv((void*)(w + (no_blocks*(i+1)+1)*g_N_s), g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[i+1], 
	    i, g_cart_grid, &lrequests[2*i+1]);
  
  /* send to the left, receive from the right */
  MPI_Isend((void*)w, g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[2*i+1], 
	    i+1, g_cart_grid, &lrequests[2*i+2]);
  MPI_Irecv((void*)(w + no_blocks*(i+1)*g_N_s), g_N_s, MPI_DOUBLE_COMPLEX, g_nb_list[i], 
	    i+1, g_cart_grid, &lrequests[2*i+3]);
  waitcount += 4;
#  endif
#endif
  return;
}

void wait_little_field_exchange(const int mu) {

#ifdef MPI
  MPI_Waitall(4, &lrequests[4*mu], &lstatus[4*mu]);
  waitcount -= 4;
#endif
  return;
}

void ldiff(complex * Q, complex * const R, complex * const S, const int N);
double lsquare_norm(complex * const Q, const int N);
complex lscalar_prod(complex * const R, complex * const S, const int N);
void lmul_r(complex * const R, const double c, complex * const S, const int N);
void lassign_diff_mul(complex * const R, complex * const S, const complex c, const int N);
void lassign_add_mul(complex * const R, complex * const S, const complex c, const int N);

static void init_lgcr(const int _M, const int _V);
static void free_lgcr();

static complex ** a = NULL; 
static complex * _a = NULL;
static double * b = NULL;
static complex * c = NULL;
static complex ** chi = NULL;
static complex * _chi = NULL;
static complex ** xi = NULL;
static complex * _xi = NULL;
static complex * alpha = NULL;
static complex * tmp = NULL;
static complex * rho = NULL;



int lgcr(complex * const P, complex * const Q, 
	 const int m, const int max_restarts,
	 const double eps_sq, const int rel_prec,
	 const int N, const int lda, c_matrix_mult f) {

  int k, l, restart, i;
  double norm_sq, err;
  complex ctmp;

  init_lgcr(m, lda);

  norm_sq = lsquare_norm(Q, N);
  
  for(restart = 0; restart < max_restarts; restart++) {
    f(tmp, P);
    ldiff(rho, Q, tmp, N);
    err = lsquare_norm(rho, N);
    if(g_proc_id == g_stdio_proc && g_debug_level > 0){
      printf("lGCR: %d\t%g true residue\n", restart*m, err); 
      fflush(stdout);
    }
    if(((err <= eps_sq) && (rel_prec == 0)) || ((err <= eps_sq*norm_sq) && (rel_prec == 1))) {
      return(restart*m);
    }
    for(k = 0; k < m; k++) {

      memcpy(xi[k], rho, N*sizeof(complex));
      /* here we could put in a preconditioner */
      f(tmp, xi[k]); 
      /* tmp will become chi[k] */
      for(l = 0; l < k; l++) {
	a[l][k] = lscalar_prod(chi[l], tmp, N);
	lassign_diff_mul(tmp, chi[l], a[l][k], N);
      }
      b[k] = sqrt(lsquare_norm(tmp, N));
      lmul_r(chi[k], 1./b[k], tmp, N);
      c[k] = lscalar_prod(chi[k], rho, N);
      lassign_diff_mul(rho, chi[k], c[k], N);
      err = lsquare_norm(rho, N);
      if(g_proc_id == g_stdio_proc && g_debug_level > 0){
	printf("GCR: %d\t%g iterated residue\n", restart*m+k, err); 
	fflush(stdout);
      }
      /* Precision reached? */
      if(((err <= eps_sq) && (rel_prec == 0)) || ((err <= eps_sq*norm_sq) && (rel_prec == 1))) {
	_mult_real(c[k], c[k], 1./b[k]);
	lassign_add_mul(P, xi[k], c[k], N);
	for(l = k-1; l >= 0; l--) {
	  for(i = l+1; i <= k; i++) {
	    _mult_assign_complex(ctmp, a[l][i], c[i]);
	    /* c[l] -= ctmp */
	    _diff_complex(c[l], ctmp);
	  }
	  _mult_real(c[l], c[l], 1./b[l]);
	  lassign_add_mul(P, xi[l], c[l], N);
	}
	return(restart*m+k);
      }
    }
    /* prepare for restart */
    k--;
    _mult_real(c[k], c[k], 1./b[k]);
    lassign_add_mul(P, xi[k], c[k], N);
    for(l = k-1; l >= 0; l--) {
      for(i = l+1; i <= k; i++) {
	_mult_assign_complex(ctmp, a[l][i], c[i]);
	/* c[l] -= ctmp */
	_diff_complex(c[l], ctmp);
      }
      _mult_real(c[l], c[l], 1./b[l]);
      lassign_add_mul(P, xi[l], c[l], N);
    }
  }
  return(-1);
}

static void init_lgcr(const int _M, const int _V){
  static int Vo = -1;
  static int M = -1;
  static int init = 0;
  int i;
  if((M != _M)||(init == 0)||(Vo != _V)){
    if(init == 1) free_lgcr();
    Vo = _V;
    M = _M;
    a = calloc(M+1, sizeof(complex *));
    chi = calloc(M, sizeof(complex *));
    xi = calloc(M, sizeof(complex *));
    tmp = calloc(Vo, sizeof(complex));
    rho = calloc(Vo, sizeof(complex));
    _a = calloc((M+1)*M, sizeof(complex));
    a[0] = _a;
    _chi = calloc(M*Vo, sizeof(complex));
    chi[0] = _chi;
    _xi = calloc(M*Vo, sizeof(complex));
    xi[0] = _xi;

    b = calloc(M, sizeof(double));
    c = calloc(M, sizeof(complex));
    alpha = calloc(M+1, sizeof(complex));
    for(i = 1; i < M; i++){
      chi[i] = chi[i-1] + Vo;
      xi[i] = xi[i-1] + Vo;
      a[i] = a[i-1] + M;
    }
    a[M] = a[M-1] + M;
    init = 1;
  }
}

static void free_lgcr() 
{
  free(a);
  free(chi);
  free(_a);
  free(_chi);
  free(alpha);
  free(c);
  free(_xi);
  free(xi);
  free(rho);
  free(tmp);
  return;
}


void ldiff(complex * const Q, complex * const R, complex * const S, const int N) 
{
  int i;
  for(i = 0; i < N; i++) {
    Q[i].re = R[i].re - S[i].re;
    Q[i].im = R[i].im - S[i].im;
  }
  return;
}

double lsquare_norm(complex * const Q, const int N) 
{
  int i;
  double nrm=0.;
  
  for(i = 0; i < N; i++) {
    nrm += _complex_square_norm(Q[i]);
  }
  return(nrm);
}

complex lscalar_prod(complex * const R, complex * const S, const int N) 
{
  complex res;
  int i;
  res.re = 0.;
  res.im = 0.;
  for(i = 0; i < N; i++) {
    _add_assign_complex_conj(res, R[i], S[i]);
  }
  return(res);
}

void lmul_r(complex * const R, const double c, complex * const S, const int N) 
{
  int i;
  for(i = 0; i < N; i++) {
    _mult_real(R[i], S[i], c);
  }
  return;
}


void lassign_diff_mul(complex * const R, complex * const S, const complex c, const int N)
{
  int i;
  for(i = 0; i < N; i++) {
    _add_assign_complex(R[i], c, S[i]);
  }
  return;
}

void lassign_add_mul(complex * const R, complex * const S, const complex c, const int N) 
{
  int i;
  complex d;
  d.re = -c.re;
  d.im = -c.im;
  for(i = 0; i < N; i++) {
    _add_assign_complex(R[i], d, S[i]);
  }
  return;
}