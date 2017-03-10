#include <flecc_in_c/utils/param.h>

int main(int argc, char** argv) {
  eccp_parameters_t curve_params;
  param_load( &curve_params, SECP256R1 );
  return 0;
}
