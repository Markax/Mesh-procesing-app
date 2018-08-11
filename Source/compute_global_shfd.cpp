/* Este código tiene que ser compilado desde el prompt de MATLAB con el siguiente comando: 
     - mex compute_global_shfd.cpp surface_processing.cpp -lmwlapack -lmwblas -largeArrayDims -v 
   Antes hay que configurar mex en matlab con el comando "mex -setup" para que busque un compilador C++ en el disco duro
*/
#include "mex.h"
#include "surface_processing.h"


extern surface_processing sup_global;

/*************************************************************************/

void mexFunction( int nlhs, mxArray *plhs[],int nrhs, mxArray *prhs[] )
{
	char *InName1;

	/* check for proper number of arguments */
  if(nrhs!=3) 
    mexErrMsgTxt("Se requieren 3 parámetros: fichero .surf, L mínima y L máxima para ajuste de recta de regresión");
    
  if ( mxIsChar(prhs[0]) != 1 )
    mexErrMsgTxt("Primer parámetro debe ser el nombre del fichero .surf");

  if ( mxIsNumeric(prhs[1]) != 1 )
    mexErrMsgTxt("Segundo parámetro debe ser el valor de L mínima, un valor numérico");

  if ( mxIsNumeric(prhs[2]) != 1 )
    mexErrMsgTxt("Tercer parámetro debe ser el valor de L máxima, un valor numérico");

	/* get InName and OutName from input parameters    */
  InName1 = mxArrayToString(prhs[0]);

	int* InName2 = (int*) mxGetData(prhs[1]);
	int* InName3 = (int*) mxGetData(prhs[2]);

	sup_global.calcula_Global_SH_FD((std::string) InName1, (int)*InName2, (int)*InName3);
}

/*************************************************************************/



