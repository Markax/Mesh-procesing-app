#ifndef SURFACE_PROCESSING_H
#define SURFACE_PROCESSING_H

#include <vector>
#include <string.h>
#include "dfsurface.h"

class surface_processing {

	protected:
		SILT::DFSurface s; /* superficie a procesar */
		std::vector<std::string> etiquetas; /* array con los nombres de las posibles etiquetas para cada v�rtice */
		std::vector<SILT::Point3D> normalesTriangulos; /* array con las normales a cada TRI�NGULO */
		float centro[3]; /* centro de gravedad de la superficie */
		float *array_normales; /* array con las normales en los vertices para visualizar array de vertices de OpenGL,
													    vertices y triangulos se sacan directamente del objeto superficie */

		/* estructura con los datos para el c�lculo de la LOCAL o Region SH-FD
		   para cada tri�ngulo, v�rtice o regi�n habr� tantos elementos de tipo lsc_ele como reconstrucciones 
			 a diferentes grados l */
		typedef struct {
			unsigned char l; /* grado de la reconstruccion de la superficie */
			double area;      /* area normalizada del triangulo para el grado l */
		} lsc_ele;

		/* estructura con la informaci�n relativa a un c�lculo de dimensi�n fractal en base al resultado de
		   una recta de regresi�n lineal */
		typedef struct {
			double a, /* pendiente de la recta, dimensi�n fractal */
				     b, /* corte en el eje Y */
						 error_a, /* error en a */
						 error_b, /* error en b */
						 r; /* correlaci�n */     
		} fd_ele;


		/* vector donde para cada tri�ngulo, v�rtice o regi�n de una superficie se almacena su surface counting (vector con grados y 
		   �reas normalizadas del tri�ngulo) */		
		std::vector<std::vector<lsc_ele> > lsc; 

	public:

		/* estructura con los datos para el c�lculo de la Global SH-FD */
		typedef struct {
			std::string fichero; /* nombre del fichero con la superficie en OBJ */
			unsigned char l; /* grado de la reconstruccion de la superficie */
			double area;      /* area de la superficie */
		} sc_ele;

		// Constructores por defecto y destructor
		surface_processing();
		~surface_processing();

		// metodos de procesamiento de superficie

		int leer_asc_etiq(char *fichero);
		/* lee el fichero .asc.etiq conteniendo la descripci�n de una superficie
			 de Freesurfer con v�rtices, colores, etiquetas y caras
			 y carga la superficie en sup
			 devuelve 0 si no se puede leer el fichero, 1 en caso contrario */

		int leer_dfs(char *fichero);
		/* lee el fichero .dfs fichero y carga la superficie en sup
       devuelve 0 si no se puede leer el fichero, 1 en caso contrario */

		void pinta_dfs ();
		/* visualiza la superfice .dfs */

		void genera_STL_o_M(std::string fichero);
		/* convierte la superficie a formato STL (si se parte de .DFS) o a 
			 formato Matlab .m (si se parte de .ETIQ) y la almacena en el fichero proporcionado:
				a partir del nombre proporcionado, se le elimina la extension .dfs y se generan
				dos ficheros uno a�adiendo la extension .stl o .m y otro a�adiendo 
				la extension .landmarkAscii (para procesamiento posterior con SPHARMA) */

		void calcula_Global_SH_FD(std::string fichero, int l_ini, int l_fin);
		/* calcula la Dimension Fractal Global a partir de las reconstrucciones basadas en harm�nicos esf�ricos
                   de la superficie utilizando el m�todo descrito en Yotter 2011 "Local cortical surface complexity maps 
                   from spherical harmonic reconstructions".
                   El fichero pasado como par�metro es el nombre de una de las reconstrucciones SH de la superficie para
                   el grado indicado en el propio nombre del fichero. El fichero debe estar en formato SURF (Amira). A partir del
                   nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
                   c�lculo de la DF Global basada en SH.
                   Devuelve el valor de dimensi�n fractal (a) y el resto de par�metros asociados a la recta de regresi�n
                   b, error_a, error_b y r, almacen�ndolos en un fichero con nombre fichero_GLOBAL_SHFD_ini_fin.txt */

		void calcula_Global_SH_FD_media_local_SHFD(std::string fichero);
		/* calcula la Dimension Fractal Global como la media de las local SHFD presentes en el fichero.
 		   Devuelve el valor de dimensi�n fractal (a) y el resto de par�metros asociados a la recta de regresi�n
 		   b, error_a, error_b y r, almacen�ndolos en un fichero con nombre fichero_GLOBAL_SHFD.txt */


		void calcula_Global_SH_FD_sin_unknown(std::string fichero, std::string fichero_etiq, int l_ini, int l_fin);
		/* calcula la Dimension Fractal Global a partir de las reconstrucciones basadas en harm�nicos esf�ricos
		de la superficie utilizando el m�todo descrito en Yotter 2011 "Local cortical surface complexity maps 
		from spherical harmonic reconstructions".
		El fichero pasado como par�metro es el nombre de una de las reconstrucciones SH de la superficie para
		el grado indicado en el propio nombre del fichero. El fichero debe estar en formato SURF (Amira). A partir del
		nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
		c�lculo de la DF Global basada en SH.
		PARA EL C�LCULO DE LA GLOBAL SHFD ***NO*** SE UTILIZAN AQUELLOS TRI�NGULOS CON ALG�N V�RTICE CLASIFICADO POR
		FREESURFER COMO UNKNOWN, ES DECIR, AQUELLOS V�RTICES QUE NO SE CLASIFICAN COMO NINGUNA REGI�N CEREBRAL, ES DECIR,
		AQUELLOS V�RTICES A�ADIDOS POR FREESURFER PARA CERRAR CADA HEMISFERIO
		Devuelve el valor de dimensi�n fractal (a) y el resto de par�metros asociados a la recta de regresi�n
		b, error_a, error_b y r, almacen�ndolos en un fichero con nombre fichero_GLOBAL_SHFD_ini_fin.txt */

		void calcula_Global_SH_FD_media_local_SHFD_SIN_UNKNOWN(std::string fichero, std::string fichero_etiq);
		/* Calcula la Dimension Fractal Global como la media de las local SHFD presentes en el fichero.
		 Este fichero debe contener en cada l�nea la shfd local del v�rtice correspondiente, en el siguiente formato:
			 a (shfd) b error_a error_b r
		 LOS VALORES CORRESPONDIENTES A LOS V�RTICES CLASIFICADOS COMO UNKNOWN POR FREESURFER NO SE TIENEN EN CUENTA
		 Devuelve el valor de shfd global (a) y el resto de par�metros asociados a la recta de regresi�n
		 b, error_a, error_b y r, como la media de los par�metros correspondientes para cada v�rtice y lo
		 almacena en un fichero con nombre fichero_GLOBAL_SHFD.txt */

		void calcula_fichero_Local_SH_FD_por_triangulos(std::string fichero);
		/* calcula la Dimension Fractal Local a partir de las reconstrucciones basadas en harm�nicos esf�ricos
		   de la superficie utilizando el m�todo descrito en Yotter 2011 "Local cortical surface complexity maps 
			 from spherical harmonic reconstructions". Calcula la DF local considerando el �rea de cada tri�ngulo en las diferentes
			 reconstrucciones.
			 El fichero pasado como par�metro es el nombre de una de las reconstrucciones SH de la superficie para
			 el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
			 nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
			 c�lculo de la DF Local */

		void calcula_Local_SH_FD_por_vertices(std::string fich_areas, int inicio_recta, int fin_recta);
		/* Calcula la local SH Fratal Dimension de un conjunto de v�rtices a partir de los datos pasados en fich_areas.
			 Este fichero debe contener para cada v�rtice las �reas suavizadas asociadas al v�rtice en cada reconstrucci�n.
			 inicio_recta y fin_recta son los valores de las reconstrucciones que se utilizar�n para cada v�rtice para el ajuste 
			 de la recta de regresi�n. 
			 Los resultados se devuelven en un fichero con el mismo nombre que fich_areas pero con la extensi�n .local_shfd
			 En cada l�nea del fichero se devuelven los resultados del c�lculo de la dimensi�n fractal para el v�rtice correspondiente,
			 siendo estos los datos asociados al calculo de la recta de regresi�n aX + b = 0, errores y correlaci�n, en el siguiente orden:
						a, b, error_a, error_b, correlacion */


		void calcula_fichero_Local_SH_FD_por_vertices(std::string fichero);
		/* calcula el fichero con las �reas necesarias para el c�lculo de la Dimension Fractal Local a partir 
		   de las reconstrucciones basadas en harm�nicos esf�ricos
		   de la superficie utilizando el m�todo descrito en Yotter 2010 "Estimating Local Surface Complexity Maps Using 
			 Spherical Harmonic Reconstructions". Calcula las �reas asociadas a cada v�rtice considerando la suma de las �reas de los tri�ngulos
			 incidentes en un v�rtice para cada reconstrucci�n.
			 El fichero pasado como par�metro es el nombre de una de las reconstrucciones SH de la superficie para
			 el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
			 nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
			 c�lculo de la DF Local */

		void calcula_fichero_Region_SH_FD_por_region(std::string fichero_surf, std::string fichero_etiq);
		/* calcula las �reas de las regiones cerebrales a partir de las reconstrucciones basadas en harm�nicos esf�ricos
			 de la superficie. Calcula las �reas de las regiones considerando la suma de las �reas de los tri�ngulos pertenecientes 
			 a la region.
			 fichero_surf es el nombre de una de las reconstrucciones SH de la superficie para
			 el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
			 nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
			 c�lculo de la DF
			 fichero_etiq es el fichero .ETIQ correspondiente a la superficie original de la cual se han generado las reconstrucciones,
			 este fichero se necesita porque contiene los v�rtices etiquetados con las diferentes regiones,
			 el resultado con las �reas de cada regi�n en cada reconstrucci�n se devuelve en un fichero .txt */

		SILT::DFSurface *get_sup(void);
		/* devuelve un puntero a la superficie dfs */

		std::vector<SILT::Point3D> *get_normalesTriangulos(void);
		/* devuelve un puntero al vector con las normales a los triangulos */

		void inicia_array_vertices_normales();
		/* reserva la memoria e inicializa el array de normales en vertices */

		void get_array_normales(float *&nor);
		/* devuelve el punteros al array de normales a vertices */

		std::vector<std::string> *get_etiquetas(void);
		/* devuelve el puntero al vector de etiquetas */

		void get_centro(float *c);
		/* devuelve el centro de gravedad de la superficie en el array c */
};

#endif


