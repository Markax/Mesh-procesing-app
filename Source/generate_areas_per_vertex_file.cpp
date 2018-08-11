/* Este código tiene que ser compilado desde el prompt de MATLAB con el siguiente comando: 
     - mex generate_areas_per_vertex_file.cpp surface_processing.cpp -lmwlapack -lmwblas -largeArrayDims -v 
   Antes hay que configurar mex en matlab con el comando "mex -setup" para que busque un compilador C++ en el disco duro
*/
#include "mex.h"
#include "surface_processing.h"


extern surface_processing sup_global;

/*************************************************************************/

void mexFunction( int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[] )
{
	char *InName1;

	/* check for proper number of arguments */
  if(nrhs!=1) 
    mexErrMsgTxt("Se requiere una cadena con el nombre de una reconstruccion en formato .surf");
    
  /* input must be a string */
  if ( mxIsChar(prhs[0]) != 1 )
    mexErrMsgTxt("Se requiere una cadena con el nombre de una reconstruccion en formato .surf");

  /* get InName and OutName from input parameters    */
  InName1 = mxArrayToString(prhs[0]);

	sup_global.calcula_fichero_Local_SH_FD_por_vertices((std::string) InName1);
}

/*************************************************************************/



