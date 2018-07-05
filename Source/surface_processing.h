#ifndef SURFACE_PROCESSING_H
#define SURFACE_PROCESSING_H

#include <vector>
#include <string.h>
#include "dfsurface.h"

class surface_processing {

	protected:
		SILT::DFSurface s; /* superficie a procesar */
		std::vector<std::string> etiquetas; /* array con los nombres de las posibles etiquetas para cada vértice */
		std::vector<SILT::Point3D> normalesTriangulos; /* array con las normales a cada TRIÁNGULO */
		float centro[3]; /* centro de gravedad de la superficie */
		float *array_normales; /* array con las normales en los vertices para visualizar array de vertices de OpenGL,
													    vertices y triangulos se sacan directamente del objeto superficie */

		/* estructura con los datos para el cálculo de la LOCAL o Region SH-FD
		   para cada triángulo, vértice o región habrá tantos elementos de tipo lsc_ele como reconstrucciones 
			 a diferentes grados l */
		typedef struct {
			unsigned char l; /* grado de la reconstruccion de la superficie */
			double area;      /* area normalizada del triangulo para el grado l */
		} lsc_ele;

		/* estructura con la información relativa a un cálculo de dimensión fractal en base al resultado de
		   una recta de regresión lineal */
		typedef struct {
			double a, /* pendiente de la recta, dimensión fractal */
				     b, /* corte en el eje Y */
						 error_a, /* error en a */
						 error_b, /* error en b */
						 r; /* correlación */     
		} fd_ele;


		/* vector donde para cada triángulo, vértice o región de una superficie se almacena su surface counting (vector con grados y 
		   áreas normalizadas del triángulo) */		
		std::vector<std::vector<lsc_ele> > lsc; 

	public:

		/* estructura con los datos para el cálculo de la Global SH-FD */
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
		/* lee el fichero .asc.etiq conteniendo la descripción de una superficie
			 de Freesurfer con vértices, colores, etiquetas y caras
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
				dos ficheros uno añadiendo la extension .stl o .m y otro añadiendo 
				la extension .landmarkAscii (para procesamiento posterior con SPHARMA) */

		void calcula_Global_SH_FD(std::string fichero, int l_ini, int l_fin);
		/* calcula la Dimension Fractal Global a partir de las reconstrucciones basadas en harmónicos esféricos
                   de la superficie utilizando el método descrito en Yotter 2011 "Local cortical surface complexity maps 
                   from spherical harmonic reconstructions".
                   El fichero pasado como parámetro es el nombre de una de las reconstrucciones SH de la superficie para
                   el grado indicado en el propio nombre del fichero. El fichero debe estar en formato SURF (Amira). A partir del
                   nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
                   cálculo de la DF Global basada en SH.
                   Devuelve el valor de dimensión fractal (a) y el resto de parámetros asociados a la recta de regresión
                   b, error_a, error_b y r, almacenándolos en un fichero con nombre fichero_GLOBAL_SHFD_ini_fin.txt */

		void calcula_Global_SH_FD_media_local_SHFD(std::string fichero);
		/* calcula la Dimension Fractal Global como la media de las local SHFD presentes en el fichero.
 		   Devuelve el valor de dimensión fractal (a) y el resto de parámetros asociados a la recta de regresión
 		   b, error_a, error_b y r, almacenándolos en un fichero con nombre fichero_GLOBAL_SHFD.txt */


		void calcula_Global_SH_FD_sin_unknown(std::string fichero, std::string fichero_etiq, int l_ini, int l_fin);
		/* calcula la Dimension Fractal Global a partir de las reconstrucciones basadas en harmónicos esféricos
		de la superficie utilizando el método descrito en Yotter 2011 "Local cortical surface complexity maps 
		from spherical harmonic reconstructions".
		El fichero pasado como parámetro es el nombre de una de las reconstrucciones SH de la superficie para
		el grado indicado en el propio nombre del fichero. El fichero debe estar en formato SURF (Amira). A partir del
		nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
		cálculo de la DF Global basada en SH.
		PARA EL CÁLCULO DE LA GLOBAL SHFD ***NO*** SE UTILIZAN AQUELLOS TRIÁNGULOS CON ALGÚN VÉRTICE CLASIFICADO POR
		FREESURFER COMO UNKNOWN, ES DECIR, AQUELLOS VÉRTICES QUE NO SE CLASIFICAN COMO NINGUNA REGIÓN CEREBRAL, ES DECIR,
		AQUELLOS VÉRTICES AÑADIDOS POR FREESURFER PARA CERRAR CADA HEMISFERIO
		Devuelve el valor de dimensión fractal (a) y el resto de parámetros asociados a la recta de regresión
		b, error_a, error_b y r, almacenándolos en un fichero con nombre fichero_GLOBAL_SHFD_ini_fin.txt */

		void calcula_Global_SH_FD_media_local_SHFD_SIN_UNKNOWN(std::string fichero, std::string fichero_etiq);
		/* Calcula la Dimension Fractal Global como la media de las local SHFD presentes en el fichero.
		 Este fichero debe contener en cada línea la shfd local del vértice correspondiente, en el siguiente formato:
			 a (shfd) b error_a error_b r
		 LOS VALORES CORRESPONDIENTES A LOS VÉRTICES CLASIFICADOS COMO UNKNOWN POR FREESURFER NO SE TIENEN EN CUENTA
		 Devuelve el valor de shfd global (a) y el resto de parámetros asociados a la recta de regresión
		 b, error_a, error_b y r, como la media de los parámetros correspondientes para cada vértice y lo
		 almacena en un fichero con nombre fichero_GLOBAL_SHFD.txt */

		void calcula_fichero_Local_SH_FD_por_triangulos(std::string fichero);
		/* calcula la Dimension Fractal Local a partir de las reconstrucciones basadas en harmónicos esféricos
		   de la superficie utilizando el método descrito en Yotter 2011 "Local cortical surface complexity maps 
			 from spherical harmonic reconstructions". Calcula la DF local considerando el área de cada triángulo en las diferentes
			 reconstrucciones.
			 El fichero pasado como parámetro es el nombre de una de las reconstrucciones SH de la superficie para
			 el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
			 nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
			 cálculo de la DF Local */

		void calcula_Local_SH_FD_por_vertices(std::string fich_areas, int inicio_recta, int fin_recta);
		/* Calcula la local SH Fratal Dimension de un conjunto de vértices a partir de los datos pasados en fich_areas.
			 Este fichero debe contener para cada vértice las áreas suavizadas asociadas al vértice en cada reconstrucción.
			 inicio_recta y fin_recta son los valores de las reconstrucciones que se utilizarán para cada vértice para el ajuste 
			 de la recta de regresión. 
			 Los resultados se devuelven en un fichero con el mismo nombre que fich_areas pero con la extensión .local_shfd
			 En cada línea del fichero se devuelven los resultados del cálculo de la dimensión fractal para el vértice correspondiente,
			 siendo estos los datos asociados al calculo de la recta de regresión aX + b = 0, errores y correlación, en el siguiente orden:
						a, b, error_a, error_b, correlacion */


		void calcula_fichero_Local_SH_FD_por_vertices(std::string fichero);
		/* calcula el fichero con las áreas necesarias para el cálculo de la Dimension Fractal Local a partir 
		   de las reconstrucciones basadas en harmónicos esféricos
		   de la superficie utilizando el método descrito en Yotter 2010 "Estimating Local Surface Complexity Maps Using 
			 Spherical Harmonic Reconstructions". Calcula las áreas asociadas a cada vértice considerando la suma de las áreas de los triángulos
			 incidentes en un vértice para cada reconstrucción.
			 El fichero pasado como parámetro es el nombre de una de las reconstrucciones SH de la superficie para
			 el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
			 nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
			 cálculo de la DF Local */

		void calcula_fichero_Region_SH_FD_por_region(std::string fichero_surf, std::string fichero_etiq);
		/* calcula las áreas de las regiones cerebrales a partir de las reconstrucciones basadas en harmónicos esféricos
			 de la superficie. Calcula las áreas de las regiones considerando la suma de las áreas de los triángulos pertenecientes 
			 a la region.
			 fichero_surf es el nombre de una de las reconstrucciones SH de la superficie para
			 el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
			 nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
			 cálculo de la DF
			 fichero_etiq es el fichero .ETIQ correspondiente a la superficie original de la cual se han generado las reconstrucciones,
			 este fichero se necesita porque contiene los vértices etiquetados con las diferentes regiones,
			 el resultado con las áreas de cada región en cada reconstrucción se devuelve en un fichero .txt */

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


