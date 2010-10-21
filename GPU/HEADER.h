#ifndef _MIXED_SOLVE_H_

// own functions

void to_device (dev_spinor * device, spinor * host, dev_spinor * auxiliary, int size);

void to_host (spinor * host, dev_spinor * device, dev_spinor * auxiliary, int size);

__global__ void he_cg_init_nd_additional (float param_mubar, float param_epsbar);

__global__ void dev_mul_one_pm_imubar_gamma5 (dev_spinor * sin, dev_spinor * sout, REAL sign);

void init_mixedsolve_eo_nd(su3** gf);

void finalize_mixedsolve_eo_nd(void);

void matrix_multiplication32 (dev_spinor * , dev_spinor * , dev_spinor * , dev_spinor * , int, int, int, int, int, int, int, int);

void flopcount(unsigned long long int& total, int add);

extern "C" void benchmark_eo_nd (spinor * const Q_up, spinor * const Q_dn, int N);

int cg_eo_nd (dev_su3_2v * gf,
              dev_spinor * P_up, dev_spinor * P_dn,
              dev_spinor * Q_up, dev_spinor * Q_dn,
              int max_iter,
              int check_abs , int check_rel,
              double eps_abs, double eps_rel       );

extern "C" int mixedsolve_eo_nd (spinor * P_up, spinor * P_dn,
                                 spinor * Q_up, spinor * Q_dn,
                                 int max_iter, double eps_sq, int rel_prec);








// Florian's functions

__device__ inline dev_complex dev_cconj (dev_complex c);
__device__ inline void dev_ccopy(dev_complex* von, dev_complex* nach);
__device__ inline REAL dev_cabssquare (dev_complex c);
__device__ inline REAL dev_cabsolute (dev_complex c);
__device__ inline  dev_complex dev_crealmult(dev_complex c1, REAL real);
__device__ inline dev_complex dev_cmult (dev_complex c1, dev_complex c2);
__device__ inline dev_complex dev_cadd (dev_complex c1, dev_complex c2);
__device__ inline dev_complex dev_cdiv(dev_complex c1, dev_complex c2);
__device__ inline dev_complex dev_csub(dev_complex c1, dev_complex c2);
__device__ inline dev_complex dev_initcomplex(REAL re, REAL im);
__device__ inline void dev_copy_spinor(dev_spinor *i1, dev_spinor *i2);
__device__ inline void dev_zero_spinor(dev_spinor *sin);
__device__ inline void dev_skalarmult_add_assign_spinor(dev_spinor *in, REAL lambda,dev_spinor * in2, dev_spinor * out);
__device__ inline void dev_complexmult_add_assign_spinor(dev_spinor * in, dev_complex lambda,dev_spinor * in2, dev_spinor * out);
__device__ inline void dev_complexcgmult_add_assign_spinor(dev_spinor * in, dev_complex lambda,dev_spinor * in2, dev_spinor * out);
__device__ void inline dev_skalarmult_spinor(dev_spinor * in, dev_complex lambda, dev_spinor * out);
__device__ void inline dev_skalarmult_gamma5_spinor(dev_spinor * out, dev_complex lambda, dev_spinor * in);
__device__ void inline dev_realmult_spinor(dev_spinor * in, REAL lambda);
__device__ void inline dev_realmult_spinor_assign(dev_spinor* out, REAL lambda, dev_spinor* in);
__device__ void dev_assign_realmult_add_spinor(dev_spinor* out, REAL lambda, dev_spinor* in1,  dev_spinor* in2);
__device__ inline void dev_add_spinor_assign(dev_spinor * i1, dev_spinor * i2);
__device__ inline void dev_sub_spinor_assign(dev_spinor * i1, dev_spinor * i2);
__device__ void dev_su3MtV_spintex(dev_su3 M, int pos, dev_spinor * out);
__device__ void dev_su3MtV(dev_su3 M, const dev_spinor * s, dev_spinor * out);
__device__ void dev_su3MdaggertV(dev_su3 M, dev_spinor * s, dev_spinor * out);
__device__ void dev_Gamma0(dev_spinor * in);
__device__ void dev_Gamma3(dev_spinor * in);
__device__ void dev_Gamma2(dev_spinor * in);
__device__ void dev_Gamma1(dev_spinor * in);
__device__ void dev_Gamma5(dev_spinor * in);
__device__ void dev_Gamma5_assign(dev_spinor* out, dev_spinor* in);
__device__ void dev_GammatV(int mu, dev_spinor * in);
__device__ void dev_reconstructgf_2vtexref (const dev_su3_2v* field, int pos, dev_su3* gf);
__device__ void dev_reconstructgf_2vtexref_dagger (const dev_su3_2v* field, int pos, dev_su3* gf);
__device__ void dev_reconstructgf_8texref (const dev_su3_2v * field, int pos, dev_su3* gf);
__device__ void dev_reconstructgf_8texref_dagger (const dev_su3_2v* field,int pos, dev_su3* gf);
__global__ void dev_gamma5(dev_spinor * sin, dev_spinor * sout);
__global__ void dev_swapmu();
__global__ void dev_mul_one_pm_imu_inv(dev_spinor* sin, dev_spinor* sout, const REAL sign);
__global__ void dev_mul_one_pm_imu_sub_mul_gamma5(dev_spinor* sin1, dev_spinor* sin2, dev_spinor* sout, const REAL sign);
__device__ void dev_kappaP1_plus(dev_spinor * out, dev_spinor * in, REAL kappa);
__device__ void dev_kappaP1_minus(dev_spinor * out, dev_spinor * in, REAL kappa);
__device__ void dev_kappaP2_plus(dev_spinor * out, dev_spinor * in, REAL kappa);
__device__ void dev_kappaP2_minus(dev_spinor * out, dev_spinor * in, REAL kappa);
__device__ void dev_kappaP3_plus(dev_spinor * out, dev_spinor * in, REAL kappa);
__device__ void dev_kappaP3_minus(dev_spinor * out, dev_spinor * in, REAL kappa);
__device__ void dev_kappaP0_plus(dev_spinor * out, dev_spinor * in, dev_complex kappa);
__device__ void dev_kappaP0_minus(dev_spinor * out, dev_spinor * in, dev_complex kappa);
__global__ void dev_Hopping_Matrix(const dev_su3_2v * gf, const dev_spinor * sin, dev_spinor * sout, const int * gfindex_site, const int* gfindex_nextsite, const int * nn_evenodd, const int eo);
extern "C" void dev_Qtm_pm_psi(dev_spinor* spinin, dev_spinor* spinout, int gridsize, int blocksize, int gridsize2, int blocksize2);
__global__ void dev_tm_dirac_kappa(dev_su3_2v * gf, dev_spinor * sin, dev_spinor * sout, int * dev_nn);
extern "C" void dev_tm_dirac_dagger_kappa(dev_su3_2v * gf,dev_spinor* spinin, dev_spinor* spinout, int *grid, int * nn_grid, REAL* output,REAL* erg, int xsize, int ysize);
__device__ inline REAL dev_skalarprod_spinor(dev_spinor * s1, dev_spinor * s2);
__device__ inline REAL dev_squarenorm_spinor(dev_spinor * s1);
__device__ inline REAL dev_squarenorm_spinor_tex(int pos);
__global__ void dev_skalarprod_spinor_field2(dev_spinor* s1, dev_spinor* s2, REAL* erg);
__global__ void dev_squarenorm_spinor_field(dev_spinor* s1, REAL* erg);
__global__ void dev_skalarprod_spinor_field(dev_spinor* s1, dev_spinor* s2, REAL* erg);
__global__ void dev_zero_spinor_field(dev_spinor* s1);
__global__ void dev_copy_spinor_field(dev_spinor* s1, dev_spinor* s2);
__global__ void dev_skalarmult_add_assign_spinor_field(dev_spinor* s1, REAL lambda, dev_spinor* s2, dev_spinor* so);
__global__ void dev_skalarmult_spinor_field(dev_spinor* s1, REAL lambda, dev_spinor* so);
__global__ void dev_complexmult_spinor_field(dev_spinor* s1, dev_complex lambda, dev_spinor* so);
__global__ void he_cg_init (int* grid, REAL param_kappa, REAL param_mu, dev_complex k0, dev_complex k1, dev_complex k2, dev_complex k3);
extern "C" int find_devices();
extern "C" int bind_texture_spin(dev_spinor* s, int i);
extern "C" int unbind_texture_spin(int i);
extern "C" int bind_texture_gf(dev_su3_2v * gf);
extern "C" int unbind_texture_gf();
extern "C" int bind_texture_nn(int* nn);
extern "C" int unbind_texture_nn();
extern "C" void test_operator(dev_su3_2v * gf,dev_spinor* spinin, dev_spinor* spinout, dev_spinor* spin0, dev_spinor* spin1, dev_spinor* spin2, dev_spinor* spin3, dev_spinor* spin4, int *grid, int * nn_grid, REAL* output,REAL* erg, int xsize, int ysize);
extern "C" int dev_cg(dev_su3_2v * gf,dev_spinor* spinin, dev_spinor* spinout, dev_spinor* spin0, dev_spinor* spin1, dev_spinor* spin2, dev_spinor* spin3, dev_spinor* spin4, int *grid, int * nn_grid, REAL* output,REAL* erg, int xsize, int ysize, int rescalekappa);
extern "C" int dev_cg_eo(dev_su3_2v * gf,dev_spinor* spinin, dev_spinor* spinout, dev_spinor* spin0, dev_spinor* spin1, dev_spinor* spin2, dev_spinor* spin3, dev_spinor* spin4, int *grid, int * nn_grid, REAL* output,REAL* erg, int xsize, int ysize, int rescalekappa, REAL epsfinal);
void initnn();
void initnn_eo();
void shownn_eo();
void show_su3(su3 gf1);
void show_dev_su3(dev_su3 gf1);
void lptovec(int k);
void shownn();
void su3to2vf4(su3** gf, dev_su3_2v* h2d_gf);
void su3to8(su3** gf, dev_su3_8* h2d_gf);
void reconstructgf_2v (dev_su3* gf);
__global__ void dev_check_gauge_reconstruction_8(dev_su3_2v* gf, int pos, dev_su3 * outgf1, dev_su3* outgf2);
void check_gauge_reconstruction_8(su3 ** gf1, dev_su3_2v * gf2, int ind1, int mu);
void reconstructgf_8 (dev_su3_8 * h2d_gf, dev_su3* gf);
void showcompare_gf(int t, int x, int y, int z, int mu);
void convert2double_spin(dev_spinor* spin, spinor* h2d);
void convert2REAL4_spin(spinor* spin, dev_spinor* h2d);
void init_mixedsolve(su3** gf);
void init_mixedsolve_eo(su3** gf);
void finalize_mixedsolve();
extern "C" int mixed_solve (spinor * const P, spinor * const Q, const int max_iter, double eps, const int rel_prec,const int N);
void dummy (dev_spinor* a, dev_spinor* b);
void benchmark(spinor * const Q);
extern "C" int mixed_solve_eo (spinor * const P, spinor * const Q, const int max_iter, double eps, const int rel_prec, const int N);




#define _MIXED_SOLVE_H_

#endif





