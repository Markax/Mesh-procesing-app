#include "mex.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "dfsurface.h"
#include "surface_processing.h"


/*************************************************************************/
// constructores y destructores

surface_processing::surface_processing() {
	array_normales = NULL;
}

surface_processing::~surface_processing() {
	delete array_normales;
}

/*************************************************************************/

int convierte_SURF_a_DFS(std::string fichero_SURF, SILT::DFSurface &sup) {
/* guarda en sup la superficie SURF almacenada en fichero_SURF. Solo tiene en cuenta
   los vertices y las caras, cualquier otro dato del SURF se ignora. 
	 Borra cualquier contenido previo en sup */

        SILT::Point3D vertex;
	size_t num_vert = 0 /*113725*/, num_tri = 0 /*227446*/;
        char lineHeader[512];
        int a,b,c;
        SILT::Triangle triangulo;

	FILE * file = fopen(fichero_SURF.c_str(), "r");
	if( file == NULL ){
		std::string err = std::string("Convierte_SURF_a_DFS: Error: No se puede abrir el fichero") + fichero_SURF;
		mexErrMsgTxt(err.c_str());
		return 0;
	}

	/* borra los triángulos y vértices que ya hubiera en sup */
	sup.triangles.resize(0);
	sup.vertices.resize(0);

	while( 1 ) {
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
 
    // else : parse lineHeader
		if ( strcmp( lineHeader, "Vertices" ) == 0 ) { 
			/* se leen los vértices */
			res = fscanf(file, "%d\n", &num_vert);
                        
                        sup.vertices.resize(num_vert);
                        
			for (size_t i = 0; i < num_vert; i++) {
                                
				fscanf(file, "\t %f  %f  %f\n", &vertex.x, &vertex.y, &vertex.z);
                                sup.vertices[i] = vertex;
				//sup.vertices.push_back(vertex);
                                
			}
		} else if ( strcmp( lineHeader, "Triangles" ) == 0 ){ 
			/* se leen los triángulos */
			fscanf(file, "%d\n", &num_tri);
                        
                        sup.triangles.resize(num_tri);
                        
			for (size_t i = 0; i < num_tri; i++) {
				fscanf(file, "  %d %d %d\n", &a, &b, &c);
				/* SURF referencia los vertices a partir de 1 y DFS a partir de 0 */
				triangulo.a = a-1; 	triangulo.b = b-1; triangulo.c = c-1;
                                sup.triangles[i] = triangulo;
				//sup.triangles.push_back(triangulo);
                                
			}
		} else { /* cualquier otra linea se ignora */
			fgets(lineHeader, 256, file);
		}
	}		

	fclose(file);
	return 1;
}

/*************************************************************************/

int convierte_OBJ_a_DFS(std::string fichero_OBJ, SILT::DFSurface &sup) {
/* guarda en sup la superficie OBJ almacenada en fichero_OBJ. Solo tiene en cuenta
   los vertices y las caras, cualquier otro dato del OBJ se ignora. 
	 Borra cualquier contenido previo en sup */

	FILE * file = fopen(fichero_OBJ.c_str(), "r");
	if( file == NULL ){
		mexPrintf("No se puede abrir el fichero %s\n",fichero_OBJ);
		return 0;
	}

	/* borra los triángulos y vértices que ya hubiera en sup */
	sup.triangles.resize(0);
	sup.vertices.resize(0);

	while( 1 ) {
		char lineHeader[256];
		
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
 
    // else : parse lineHeader
		if ( strcmp( lineHeader, "v" ) == 0 ) { /* se lee un vertice */
			SILT::Point3D vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			sup.vertices.push_back(vertex);
		} else if ( strcmp( lineHeader, "f" ) == 0 ){ /* se lee una cara */
			int a,b,c;
			SILT::Triangle triangulo;
			fscanf(file, "%d %d %d\n", &a, &b, &c);
			/* OBJ referencia los vertices a partir de 1 y DFS a partir de 0 */
			triangulo.a = a-1; 	triangulo.b = b-1; triangulo.c = c-1;
			sup.triangles.push_back(triangulo);
		} else { /* cualquier otra linea se ignora */
			fgets(lineHeader, 128, file);
			//fscanf(file,"\n");
		}
	}		

	fclose(file);
	return 1;
}

/*************************************************************************/

float calcula_area_triangulo(SILT::Point3D &v1, SILT::Point3D &v2, SILT::Point3D &v3) {
	double xn, yn, zn;
	SILT::Point3D v12, v13;

	v12.x = v2.x - v1.x; v12.y = v2.y - v1.y; v12.z = v2.z - v1.z;
	v13.x = v3.x - v1.x; v13.y = v3.y - v1.y; v13.z = v3.z - v1.z;

	xn = (v12.y * v13.z) - (v12.z * v13.y);
	yn = (v12.z * v13.x) - (v12.x * v13.z);
	zn = (v12.x * v13.y) - (v12.y * v13.x);

	return (sqrt((xn * xn) + (yn * yn) + (zn * zn))/2.0);
}

/*************************************************************************/

double calcula_area_superficie(SILT::DFSurface &recons) {
	double area = 0.0;

	for(size_t i = 0; i < recons.triangles.size(); i++) {
		area += calcula_area_triangulo(recons.vertices[recons.triangles[i].a], 
			                             recons.vertices[recons.triangles[i].b], 
			                             recons.vertices[recons.triangles[i].c]);
	}
	return area;
}

/*************************************************************************/

double calcula_area_superficie_sin_unknown(SILT::DFSurface &recons, surface_processing *dfs) {
	double area = 0.0;
	long int unknown = 0;
	short int et_a, et_b, et_c;
	SILT::DFSurface *s = dfs->get_sup();

	for(size_t i = 0; i < recons.triangles.size(); i++) {
		et_a = s->vertexLabels[s->triangles[i].a]; /* índice de etiqueta del vértice a del triángulo */
		et_b = s->vertexLabels[s->triangles[i].b]; /* índice de etiqueta del vértice b del triángulo */
		et_c = s->vertexLabels[s->triangles[i].c]; /* índice de etiqueta del vértice c del triángulo */

		if (((*(dfs->get_etiquetas()))[et_a] != "unknown") && 
		    ((*(dfs->get_etiquetas()))[et_b] != "unknown") &&
		    ((*(dfs->get_etiquetas()))[et_c] != "unknown")) {

			area += calcula_area_triangulo(recons.vertices[recons.triangles[i].a], 
					               recons.vertices[recons.triangles[i].b], 
					               recons.vertices[recons.triangles[i].c]);
		} else {
			unknown++;
		}
	}
	mexPrintf("Encontrados %d triangulos con unknown\n", unknown);
	return area;
}

/*************************************************************************/

int busca_ficheros_SURF(std::string fichero, std::vector<surface_processing::sc_ele> &sce) {
/* busca los ficheros .SURF correspondientes a las reconstrucciones de la misma superficie
   que fichero para diferentes grados l (sólo busca para grados de 1 a 999),
	 devuelve en el vector sce, para cada fichero encontrado, el nombre del fichero, y el grado de la reconstrucción,
	 la función devuelve el número de ficheros encontrados */

  FILE *fp;
  unsigned char k;
	surface_processing::sc_ele ele_sup;
	int pos_ini, pos_fin;
	char i_char[5];

	//PATRON con resize: template_Egem_CT_6_1_OL_2O_CS_35_0_reg_orig.surf
	//PATRON sin resize: template_Egem_CT_6_1_OL_2O_35_0_des_orig.surf

	/* comprobación patrones */
	if (fichero.find("OL_2O_CS_") != std::string::npos) {
		pos_ini = fichero.find("OL_2O_CS_") + 9;
		pos_fin = fichero.find("_", pos_ini);
	} else if (fichero.find("OL_2O_") != std::string::npos) {
		pos_ini = fichero.find("OL_2O_") + 6;
		pos_fin = fichero.find("_", pos_ini);
	} else {
		mexErrMsgTxt("busca_ficheros_SURF: Error, el fichero seleccionado no se ajusta a los posibles patrones\n");
		return 0;
	}

	std::string inicio_nombre_fichero = fichero.substr(0, pos_ini),
		          fin_nombre_fichero = fichero.substr(pos_fin, std::string::npos);

	k = 0;
	for (int i = 1; i < 1000; i++) { /* ATENCION: solo busca reconstrucciones de hasta el grado l = 999 */
		//std::string nombre = inicio_nombre_fichero + std::to_string(i) + fin_nombre_fichero;
		sprintf(i_char,"%d",i);
		std::string nombre = inicio_nombre_fichero + i_char + fin_nombre_fichero;
		if ((fp = fopen(nombre.c_str(), "r")) != NULL) {
			ele_sup.fichero = nombre;
			ele_sup.l = i;
			ele_sup.area = 0;
			sce.push_back(ele_sup);

			k++;
			fclose(fp);
		}
	}
	return k;
}

/*************************************************************************/

int busca_ficheros_OBJ(std::string fichero, std::vector<surface_processing::sc_ele> &sce) {
/* busca los ficheros .OBJ correspondientes a las reconstrucciones de la misma superficie
   que fichero para diferentes grados l (sólo busca para grados de 1 a 99),
	 devuelve en el vector sce, para cada fichero encontrado, el nombre del fichero, y el grado de la reconstrucción,
	 la función devuelve el número de ficheros encontrados */

  FILE *fp;
  unsigned char k;
	surface_processing::sc_ele ele_sup;
	int pos_ini, pos_fin;
	char i_char[5];

	//PATRON con resize: template_Egem_CT_6_1_OL_2O_CS_35_0_reg_orig.obj
	//PATRON sin resize: template_Egem_CT_6_1_OL_2O_35_0_des_orig.obj

	/* comprobación patrones */
	if (fichero.find("OL_2O_CS_") != std::string::npos) {
		pos_ini = fichero.find("OL_2O_CS_") + 9;
		pos_fin = fichero.find("_", pos_ini);
	} else if (fichero.find("OL_2O_") != std::string::npos) {
		pos_ini = fichero.find("OL_2O_") + 6;
		pos_fin = fichero.find("_", pos_ini);
	} else {
		mexErrMsgTxt("busca_ficheros_OBJ: Error, el fichero seleccionado no se ajusta a los posibles patrones\n");
		return 0;
	}

	std::string inicio_nombre_fichero = fichero.substr(0, pos_ini),
		          fin_nombre_fichero = fichero.substr(pos_fin, std::string::npos);

	k = 0;
	for (unsigned char i = 1; i < 100; i++) { /* ATENCION: solo busca reconstrucciones de hasta el grado l = 99 */
		//std::string nombre = inicio_nombre_fichero + std::to_string(i) + fin_nombre_fichero;
		sprintf(i_char,"%d",i);
		std::string nombre = inicio_nombre_fichero + i_char + fin_nombre_fichero;

		if ((fp = fopen(nombre.c_str(), "r")) != NULL) {
			ele_sup.fichero = nombre;
			ele_sup.l = i;
			ele_sup.area = 0;
			sce.push_back(ele_sup);

			k++;
			fclose(fp);
		}
	}
	return k;
}

/*************************************************************************/

void surface_processing::calcula_fichero_Region_SH_FD_por_region(std::string fichero_surf, std::string fichero_etiq) {
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

	std::vector<SILT::DFSurface> recons; /* superficies que almacenan las reconstrucciones pasadas a formato DFS,
																			    hay una por cada grado */ 
	std::vector<std::vector<double> > recons_area_tri; 
	                               /* vector que almacena, para cada reconstrucción, un vector con el área de cada uno 
																    de sus triángulos, 
																    de esta forma el área de cada triángulo sólo se calcula una vez */
	std::vector<sc_ele> surf_count; /* vector que almacena los datos para acceder a las diferentes reconstrucciones:
																	     - fichero SURF de la superficie i
														           - grado de la superficie i
																       - area de la superficie i (este dato no se utiliza aquí, sólo en global sh) */

	/* se carga la superficie original con las etiquetas para cada vértice */
	leer_asc_etiq((char *) fichero_etiq.c_str());

	/* carga los nombres de los ficheros SURF con todas las reconstrucciones SH y el grado de cada una,
	   se reutiliza la estructura sc_ele de la global sh, aunque sc_ele.area no se utiliza */
	if (!busca_ficheros_SURF(fichero_surf, surf_count)/*busca_ficheros_OBJ(fichero, surf_count)*/) {
		mexErrMsgTxt("Error: calcula_Local_SH_FD: No encuentro ficheros SURF para calcular la Local SH-FD\n");
		return;
	}

	/* Para cada reconstruccion SH en formato SURF, hay que pasarla a formato DFS y calcular el área de cada uno 
	   de sus triángulos */
	recons.resize(surf_count.size()); // aloja espacio en el vector para todas las reconstrucciones
	recons_area_tri.resize(surf_count.size()); // aloja espacio en el vector para todas las reconstrucciones
	int i = 0;
	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
		/* convertir a DFS la malla SURF */
		if (!convierte_SURF_a_DFS(it->fichero, recons[i])/*convierte_OBJ_a_DFS(it->fichero, recons[i])*/) {
			std::string err = std::string("Error: calcula_Local_SH_FD: No se puede convertir fichero ") + it->fichero + std::string("\n");
			mexErrMsgTxt(err.c_str());
			return;
		};

		/* calcular el área de cada triángulo de la reconstrucción i */
		size_t num_t = recons[i].triangles.size(); // verdaderamente el número de triángulos es el mismo para todas las
		                                           // reconstrucciones
		for(size_t tri = 0; tri < num_t; tri++) {
			/* se calcula el área */
			SILT::Point3D v1 = recons[i].vertices[recons[i].triangles[tri].a], 
										v2 = recons[i].vertices[recons[i].triangles[tri].b],
										v3 = recons[i].vertices[recons[i].triangles[tri].c];

			double area_tri = calcula_area_triangulo(v1, v2, v3);
			recons_area_tri[i].push_back(area_tri); // para el triángulo tri de la reconstrucción i se almacena su área
		}

		i++;
	}

	/*************************************************************************/
	/* Implementación de la region SHFD utilizando las áreas de las regiones */
	/* Para cada triángulo de la superficie original:
	     - se suma su área, para cada reconstrucción l, en las regiones
			   a las que pertenecen sus tres vértices */
	/*************************************************************************/
	size_t num_vert = s.vertices.size();
	size_t num_tri = s.triangles.size();

	short int num_et = get_etiquetas()->size(); // número de etiquetas
	size_t nr = recons.size(); // número de reconstrucciones
	lsc.resize(num_et); // aloja espacio en el vector para los surface_counting para cada región
	for (size_t i = 0; i < num_et; i++) {
		// aloja espacio e inicializa el área y la l de la región en cada reconstrucción
		lsc[i].resize(nr);
		for (size_t j = 0; j < nr; j++) {
			lsc[i][j].l = surf_count[j].l;
			lsc[i][j].area = 0.0;
		}
	}

	for (size_t tri = 0 ; tri < num_tri; tri++) {
		short int et_a, et_b, et_c;
		et_a = s.vertexLabels[s.triangles[tri].a]; /* índice de etiqueta del vértice a del triángulo */
		et_b = s.vertexLabels[s.triangles[tri].b]; /* índice de etiqueta del vértice b del triángulo */
		et_c = s.vertexLabels[s.triangles[tri].c]; /* índice de etiqueta del vértice c del triángulo */

		if ((et_a < 0) || (et_a >= num_et) || (et_b < 0) || (et_b >= num_et) || (et_c < 0) || (et_c >= num_et)) {
			std::cout << "Aquí está el fallo";
			exit(0);
		}

		for (size_t rec = 0; rec < nr; rec++) {
			/* el área del triángulo tri en la reconstrucción rec se suma a la
			   que ya hay para la región de su vértice al nivel de reconstrucción rec */
			lsc[et_a][rec].area += recons_area_tri[rec][tri];  
		}

		if (et_b != et_a) {
			/* el triángulo pertenece a varias regiones, hay que sumar su área en todas las regiones a las
			   que pertenece */
			for (size_t rec = 0; rec < nr; rec++) {
				lsc[et_b][rec].area += recons_area_tri[rec][tri];  
			}
		}
		if ((et_c != et_b) && ((et_c != et_a))) {
			/* el triángulo pertenece a varias regiones, hay que sumar su área en todas las regiones a las
			   que pertenece */
			for (size_t rec = 0; rec < nr; rec++) {
				lsc[et_c][rec].area += recons_area_tri[rec][tri];  
			}
		}
	}

	/* la suma de las áreas de los triángulos de la región tienen que ser normalizadas respecto 
		  a la suma de las áreas de los triángulos de la región correspondiente en la superficie original */
	/* siguiendo lo realizado en Yotter 2011, se considera como superficie original la SH reconstrucción
			de mayor grado l */
	for (size_t et = 0; et < num_et; et++) {
		surface_processing::lsc_ele max = lsc[et].back();
		for(size_t rec = 0; rec < nr; rec++) {
			/* se normaliza el área dividiendo por el área del triangulo equivalente en superficie original (grado l máximo) */
			//lsc[et][rec].area = lsc[et][rec].area / max.area;
		}
	}


	/* se graba a disco el resultado del surface-counting */
	//std::ofstream fs(fichero_surf + "_REGION_RESULTS.txt");
	std::ofstream fs((fichero_surf + "_REGION_RESULTS.txt").c_str());

	fs << "Resultado del Region Surface Counting - SUMANDO ÁREAS DE TRIÁNGULOS DE CADA REGION" << "\n";	
	fs << "En cada línea se muestra la suma normalizada de las áreas de los triángulos de la región para cada reconstrucción de grado l" << "\n";	
	for(size_t i = 0; i < lsc.size(); i++) {
		fs << i << "(" << etiquetas[i] << "): ";
		for(size_t j = 0; j < lsc[i].size(); j++) {
			fs << lsc[i][j].area << " ";
		}
		fs << "\n";
	}
	fs.close();
	mexPrintf("Terminado\n");
}

/*************************************************************************/

void surface_processing::calcula_fichero_Local_SH_FD_por_triangulos(std::string fichero) {
/* calcula la Dimension Fractal Local a partir de las reconstrucciones basadas en harmónicos esféricos
		de la superficie utilizando el método descrito en Yotter 2011 "Local cortical surface complexity maps 
		from spherical harmonic reconstructions". Calcula la DF local considerando el área de cada triángulo en las diferentes
		reconstrucciones.
		El fichero pasado como parámetro es el nombre de una de las reconstrucciones SH de la superficie para
		el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
		nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
		cálculo de la DF Local */

	std::vector<SILT::DFSurface> recons; /* superficies que almacenan las reconstrucciones pasadas a formato DFS,
																			    hay una por cada grado */ 
	std::vector<std::vector<size_t> > tri_en_vert; 
	                           /* vector que almacena, para cada vértice, un vector con los triángulos que lo comparten.
														    esta información es común a todas las reconstrucciones, ya que todas tienen los mismos
																triángulos y en el mismo orden */

	std::vector<sc_ele> surf_count; /* vector que almacena los datos para acceder a las diferentes reconstrucciones:
																	     - fichero SURF de la superficie i
														           - grado de la superficie i
																       - area de la superficie i (este dato no se utiliza aquí, sólo en global sh) */

	/* carga los nombres de los ficheros SURF con todas las reconstrucciones SH y el grado de cada una,
	   se reutiliza la estructura sc_ele de la global sh, aunque sc_ele.area no se utiliza */
	if (!busca_ficheros_SURF(fichero, surf_count)/*busca_ficheros_OBJ(fichero, surf_count)*/) {
		mexErrMsgTxt("Error: calcula_Local_SH_FD: No encuentro ficheros SURF para calcular la Local SH-FD\n");
		return;
	}

	/* Para cada reconstruccion SH en formato SURF, hay que pasarla a formato DFS */
	recons.resize(surf_count.size()); // aloja espacio en el vector para todas las reconstrucciones
	int i = 0;
	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
		/* convertir a DFS la malla SURF */
		if (!convierte_SURF_a_DFS(it->fichero, recons[i])/*convierte_OBJ_a_DFS(it->fichero, recons[i])*/) {
			std::string err = std::string("Error: calcula_Local_SH_FD: No se puede convertir fichero ") + it->fichero + std::string("\n");
			mexErrMsgTxt(err.c_str());
			return;
		};

		i++;
	}

	/* se almacenan los triángulos incidentes en cada vértice */
	/* se utiliza la reconstrucción 0, ya que todas tienen los mismos triángulos y en el mismo orden */
	size_t num_vert = recons[0].vertices.size();
	size_t num_tri = recons[0].triangles.size();
	tri_en_vert.resize(num_vert);
	for (size_t i = 0; i < num_tri; i++) {
		tri_en_vert[recons[0].triangles[i].a].push_back(i);
		tri_en_vert[recons[0].triangles[i].b].push_back(i);
		tri_en_vert[recons[0].triangles[i].c].push_back(i);
	}

	/************************************/
	/* Implementación según YOTTER 2011 */
	/* para cada triángulo de la superficie se calcula su area normalizada en cada una de las reconstrucciones */
	/************************************/
	size_t nt = recons[0].triangles.size(); // número de triángulos es el mismo en cualquier reconstrucción
	lsc.resize(nt); // aloja espacio en el vector con los surface_counting para cada triángulo
	size_t nr = recons.size(); // número de reconstrucciones
	for (size_t i = 0 ; i < nt; i++) {
		for (size_t rec = 0; rec < nr; rec++) {
			lsc_ele elem;
			/* se calcula el área del triangulo i en la superficie de grado l */
			SILT::Point3D v1 = recons[rec].vertices[recons[rec].triangles[i].a], 
										v2 = recons[rec].vertices[recons[rec].triangles[i].b],
										v3 = recons[rec].vertices[recons[rec].triangles[i].c];

			elem.l = surf_count[rec].l; // l verdaderamente debe ser igual a rec+1
			elem.area = calcula_area_triangulo(v1, v2, v3);
			lsc[i].push_back(elem); // para el triángulo i se introduce un elemento del surface counting (grado l y area)
		}

		/* las áreas de cada triángulo tienen que ser normalizadas respecto al área del triángulo correspondiente en la 
		   superficie original */
		/* siguiendo lo realizado en Yotter 2011, se considera como superficie original la SH reconstrucción
			 de mayor grado l */
		surface_processing::lsc_ele max = lsc[i].back();
		for(size_t rec = 0; rec < nr; rec++) {
			/* se normaliza el área dividiendo por el área del triangulo equivalente en superficie original (grado l máximo) */
			//lsc[i][rec].area = lsc[i][rec].area / max.area;
		}
	}

	/* se graba a disco el resultado del surface-counting */
	//std::ofstream fs(fichero + "_LOCAL_RESULTS_TRIANGLES.txt");
	std::ofstream fs((fichero + "_LOCAL_RESULTS_TRIANGLES.txt").c_str());

	fs << "Resultado del Local Surface Counting" << "\n";	
	fs << "En cada línea se muestran las áreas normalizadas de un triángulo para cada reconstrucción de grado l" << "\n";	
	for(size_t i = 0; i < lsc.size(); i++) {
		fs << i << ": ";
		for(size_t j = 0; j < lsc[i].size(); j++) {
			fs << lsc[i][j].area << " ";
		}
		fs << "\n";
	}
	fs.close();
}

/*************************************************************************/

void surface_processing::calcula_Local_SH_FD_por_vertices(std::string fich_areas, int inicio_recta, int fin_recta) {
/* Calcula la local SH Fratal Dimension de un conjunto de vértices a partir de los datos pasados en fich_areas.
	 Este fichero debe contener para cada vértice las áreas suavizadas asociadas al vértice en cada reconstrucción.
	 inicio_recta y fin_recta son los valores de l de las reconstrucciones que se utilizarán para cada vértice para el ajuste 
	 de la recta de regresión. Se supone que todas las l son consecutivas.
   Los resultados se devuelven en un fichero con el mismo nombre que fich_areas pero con la extensión .local_shfd
	 En cada línea del fichero se devuelven los resultados del cálculo de la dimensión fractal para el vértice correspondiente,
	 siendo estos los datos asociados al calculo de la recta de regresión aX + b = 0, errores y correlación, en el siguiente orden:
	      a, b, error_a, error_b, correlacion */

  float n,
        suma_x,suma_y,suma_xy,suma_x2,x,y,media_x,media_y,
        delta,suma_delta,
        suma_xymedia,suma_xmedia2,suma_ymedia2,
				a, b, error_a, error_b, r;
	std::string linea;

	//std::ifstream f_areas(fich_areas);
	std::ifstream f_areas(fich_areas.c_str());
	if (!f_areas.good()) {
		std::string err = std::string("calcula_Local_SH_FD_por_vertices: error, el fichero de áreas ") + fich_areas + std::string(" no existe");
    mexErrMsgTxt(err.c_str());
		return;
	}

	/* se lee el número de reconstrucciones, irán de 0 a tv_fin-1 */
	f_areas >> linea;
	int num_rec = atoi(linea.c_str());

  if ((inicio_recta < 1) || (fin_recta > num_rec) ||
      (inicio_recta > num_rec) || (fin_recta < 1) ||
      (fin_recta < inicio_recta)) {
    mexErrMsgTxt("calcula_Local_SH_FD_por_vertices: error, rangos calculo de recta erroneos");
		return;
	}

	/* se leen el número de vértices */
	f_areas >> linea;
	size_t num_vert = atoi(linea.c_str());

  mexPrintf("calcula_Local_SH_FD_por_vertices: calculando local shfd para %d vertices...\n", num_vert);
	std::vector<double> datos; // vector con las áreas correspondientes a un vértice
	datos.resize(num_rec);
	std::vector<fd_ele> lshfd; // vector que almacena la local shfd para cada vértice, para posteriormente poder hacer una
	                           // normalización de los resultados entre 2 y 3
	fd_ele res_fd;
	lshfd.resize(num_vert);
	n=(float)fin_recta-inicio_recta+1;
	double maxfd = -99999; // para normalizar
	double minfd = 99999; // para normalizar
	for (size_t vert = 0; vert < num_vert ; vert++) {
	  /* printf("  vertice %d\n", vert); */
		/* se leen los datos de las áreas para el vértice vert */
		for (size_t j = 0; j < num_rec; j++) {
			f_areas >> datos[j];
		}

		/* calculo de la recta aX + b = 0 para cada vértice i */
		suma_x=suma_y=suma_xy=suma_x2=suma_delta=media_x=media_y=0;
		for(size_t i=inicio_recta;i<=fin_recta;i++) {
			//x=log(1.0/(float)i); // versión box-counting
            x=log((float)i);
			//x=log(log((float)i));
			y=log(datos[i-1]); // l se mide desde 1, pero en el vector se almacena desde 0

			suma_x=suma_x+x;
			suma_x2=suma_x2+(x*x);
			suma_y=suma_y+y;
			suma_xy=suma_xy+(x*y);
		}
		media_x=suma_x/n;
		media_y=suma_y/n;
  
		a=(n*suma_xy - suma_x*suma_y)/(n*suma_x2 - suma_x * suma_x);
		b=(suma_y - (a)*suma_x)/n;

		/* calculo de los errores en a y en b */
		suma_xymedia=suma_xmedia2=suma_ymedia2=0;
		for(size_t i=inicio_recta;i<=fin_recta;i++) {
			// x=log(1.0/(float)i); // versión box-counting
			x=log((float)i);
			//x=log(log((float)i));
			y=log(datos[i-1]);
    
			suma_delta=suma_delta+ (y - (a)*x - (b)) * (y - (a)*x - (b));
			suma_xymedia=suma_xymedia+ ((x-media_x)*(y-media_y));
			suma_xmedia2=suma_xmedia2+ ((x-media_x)*(x-media_x));
			suma_ymedia2=suma_ymedia2+ ((y-media_y)*(y-media_y));
		}
		delta=sqrt(suma_delta/(n-2));
		error_a=(sqrt(n)*delta) / (sqrt(n*suma_x2 - suma_x*suma_x));
		error_b=(error_a)*sqrt(suma_x2/n);
		r=suma_xymedia/(sqrt(suma_xmedia2)*sqrt(suma_ymedia2));

		res_fd.a = a;
		res_fd.b = b;
		res_fd.error_a = error_a;
		res_fd.error_b = error_b;
		res_fd.r = r;
		lshfd[vert] = res_fd;
		if (a > maxfd) maxfd = a;
		if (a < minfd) minfd = a;
	}
	f_areas.close();

	/* CUIDADO!!!! LO SIGUIENTE NO DEBERÍA SER NECESARIO HACERLO, HAY QUE INTENTAR ENCONTRAR UNA
	   LÓGICA PARA LA OBTENCIÓN ADECUADA DE VALORES DE SHFD ENTRE 2 Y 3 *************************/
	/* se normalizan los resultados entre 2.0 y 3.0, lo que deberían ser los resultados lógicos de
	   valores de dimensión fractal */
	/* *******************************************************************************************/
	//double ancho = maxfd - minfd;
	//for (size_t vert = 0; vert < num_vert; vert++) {
	//	lshfd[vert].a = ((lshfd[vert].a - minfd) / ancho) + 2.0;
	//}
	
	/* se graban a fichero los resultados para cada vértice */
	char ini[5],fin[5];
	//itoa(inicio_recta,ini,10);
	//itoa(fin_recta,fin,10);
	sprintf(ini,"%d",inicio_recta);
	sprintf(fin,"%d",fin_recta);

	//std::string fich_fd = fich_areas.substr(0,fich_areas.length()-3) + "local_shfd_" + 
	//	                                        ini + "_" + fin + ".txt";

        std::string fich_fd = fich_areas.substr(0,fich_areas.length()-3) + "local_shfd_" +
                                                      ini + "_" + fin + ".txt";
	//std::ofstream f_fd(fich_fd);
	std::ofstream f_fd(fich_fd.c_str());
	for (size_t vert = 0; vert < num_vert; vert++) {
		f_fd << lshfd[vert].a << " " << lshfd[vert].b << " " << lshfd[vert].error_a << " " << lshfd[vert].error_b << " " << lshfd[vert].r << "\n";
	}
	f_fd.close();

	mexPrintf("calcula_Local_SH_FD_por_vertices: Terminado.\n", num_vert);
}

/*************************************************************************/

void surface_processing::calcula_fichero_Local_SH_FD_por_vertices(std::string fichero) {
		/* calcula el fichero con las áreas necesarias para el cálculo de la Dimension Fractal Local a partir 
		   de las reconstrucciones basadas en harmónicos esféricos de la superficie utilizando el método descrito en 
			 Yotter 2010 "Estimating Local Surface Complexity Maps Using 
			 Spherical Harmonic Reconstructions". Calcula las áreas asociadas a cada vértice considerando la MEDIA de las áreas de los triángulos
			 incidentes en un vértice para cada reconstrucción.
			 El fichero pasado como parámetro es el nombre de una de las reconstrucciones SH de la superficie para
			 el grado indicado en el propio nombre del fichero. El fichero debe estar en formato .SURF (Amira). A partir del
			 nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
			 cálculo de la DF Local */

	std::vector<SILT::DFSurface> recons; /* superficies que almacenan las reconstrucciones pasadas a formato DFS,
																			    hay una por cada grado */ 
	std::vector<std::vector<double> > recons_area_tri; 
	                               /* vector que almacena, para cada reconstrucción, un vector con el área de cada uno 
																    de sus triángulos, 
																    de esta forma el área de cada triángulo sólo se calcula una vez, aunque será utilizada
																		por los tres vértices del triángulo */
	std::vector<std::vector<size_t> > tri_en_vert; 
	                           /* vector que almacena, para cada vértice, un vector con los triángulos que lo comparten.
														    esta información es común a todas las reconstrucciones, ya que todas tienen los mismos
																triángulos y en el mismo orden */

	std::vector<sc_ele> surf_count; /* vector que almacena los datos para acceder a las diferentes reconstrucciones:
																	     - fichero SURF de la superficie i
														           - grado de la superficie i
																       - area de la superficie i (este dato no se utiliza aquí, sólo en global sh) */

	/* carga los nombres de los ficheros SURF con todas las reconstrucciones SH y el grado de cada una,
	   se reutiliza la estructura sc_ele de la global sh, aunque sc_ele.area no se utiliza */
	if (!busca_ficheros_SURF(fichero, surf_count)/*busca_ficheros_OBJ(fichero, surf_count)*/) {
		mexErrMsgTxt("Error: calcula_Local_SH_FD: No encuentro ficheros SURF para calcular la Local SH-FD\n");
		return;
	}

	/* Para cada reconstruccion SH en formato SURF, hay que pasarla a formato DFS y calcular el área de cada uno 
	   de sus triángulos */
	recons.resize(surf_count.size()); // aloja espacio en el vector para todas las reconstrucciones
	recons_area_tri.resize(surf_count.size()); // aloja espacio en el vector para todas las reconstrucciones
	int i = 0;
	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
		/* convertir a DFS la malla SURF */
		if (!convierte_SURF_a_DFS(it->fichero, recons[i])/*convierte_OBJ_a_DFS(it->fichero, recons[i])*/) {
			std::string err = std::string("Error: calcula_Local_SH_FD: No se puede convertir fichero ") + it->fichero + std::string("\n");
			mexErrMsgTxt(err.c_str());
			return;
		};

		/* calcular el área de cada triángulo de la reconstrucción i */
		size_t num_t = recons[i].triangles.size(); // verdaderamente el número de triángulos es el mismo para todas las
		                                           // reconstrucciones
		for(size_t tri = 0; tri < num_t; tri++) {
			/* se calcula el área */
			SILT::Point3D v1 = recons[i].vertices[recons[i].triangles[tri].a], 
										v2 = recons[i].vertices[recons[i].triangles[tri].b],
										v3 = recons[i].vertices[recons[i].triangles[tri].c];

			double area_tri = calcula_area_triangulo(v1, v2, v3);
			recons_area_tri[i].push_back(area_tri); // para el triángulo tri de la reconstrucción i se almacena su área
		}

		i++;
	}

	/* se almacenan los triángulos incidentes en cada vértice */
	/* se utiliza la reconstrucción 0, ya que todas tienen los mismos triángulos y en el mismo orden */
	size_t num_vert = recons[0].vertices.size();
	size_t num_tri = recons[0].triangles.size();
	tri_en_vert.resize(num_vert);
	for (size_t i = 0; i < num_tri; i++) {
		tri_en_vert[recons[0].triangles[i].a].push_back(i);
		tri_en_vert[recons[0].triangles[i].b].push_back(i);
		tri_en_vert[recons[0].triangles[i].c].push_back(i);
	}


	/************************************/
	/* Implementación según YOTTER 2010 */
	/* para cada vértice y en cada reconstrucción, se calcula la MEDIA del área de los triángulos incidentes en él */
	/************************************/
	lsc.resize(num_vert); // aloja espacio en el vector con los surface_counting para cada vértice
	size_t nr = recons.size(); // número de reconstrucciones
	for (size_t vert = 0 ; vert < num_vert; vert++) {
		size_t num_tri_en_vert = tri_en_vert[vert].size();
		for (size_t rec = 0; rec < nr; rec++) {
			lsc_ele elem;
			/* se accede a las áreas ya calculadas de los triángulos incidentes en el vértice vert para la superficie de grado l */
			double suma_area = 0;
			for (size_t tri = 0; tri < num_tri_en_vert; tri++) {
				size_t un_tri = tri_en_vert[vert][tri]; // indice al triángulo incidente en vert
				suma_area += recons_area_tri[rec][un_tri];
			}

			elem.l = surf_count[rec].l; // l verdaderamente debe ser igual a rec+1
			//elem.area = suma_area; // antigua implementación, para cada vértice considera la suma de las áreas 
			elem.area = suma_area / (float) num_tri_en_vert; // nueva implementación, para cada vértice considera la media de las áreas
			lsc[vert].push_back(elem); // para el vértice verts se introduce un elemento del surface counting (grado l y area)
		}

		/* la MEDIA de las áreas de los triángulos incidentes en el vértice vert tienen que ser normalizadas respecto 
		   a la MEDIA de las áreas de los triángulos correspondientes en la superficie original */
		/* siguiendo lo realizado en Yotter 2011, se considera como superficie original la SH reconstrucción
			 de mayor grado l */
		surface_processing::lsc_ele max = lsc[vert].back();
		for(size_t rec = 0; rec < nr; rec++) {
			/* se normaliza el área dividiendo por el área del triangulo equivalente en superficie original (grado l máximo) */
			//lsc[vert][rec].area = lsc[vert][rec].area / max.area;
		}
	}

	/* se graban a disco las áreas correspondientes a cada vértice para cada reconstrucción,
	   a estas áreas hay que aplicarles un heat kernel smoothing previo a utilizarlas para el
		 cálculo de la local shfd */
	std::ofstream fs_areas((fichero + "_AREAS_SIN_SUAVIZAR.txt").c_str());

	/* la cabecera del fichero es el número de reconstrucciones seguido del numero de vertices en cada reconstruccion */
	fs_areas << nr << "\n";	
	fs_areas << num_vert << "\n";	
	/* después van el nombre del fichero SURF de la reconstrucción y las áreas de cada vértice para esa reconstrucción, 
	   primero las num_vert áreas correspondientes a la primera reconstrucción, luego las num_vert áreas correspondientes
		 a la segunda reconstrucción, y así hasta nr reconstrucciones */
	for(size_t rec = 0; rec < nr; rec++) {
		std::string surf = surf_count[rec].fichero;
		fs_areas << surf << "\n";	
		for(size_t vert = 0; vert < num_vert; vert++) {
			fs_areas << lsc[vert][rec].area << "\n";
		}
	}
	fs_areas.close();
	mexPrintf("Guardando areas terminado\n");


	/* se graba a disco el resultado del surface-counting */
	//std::ofstream fs(fichero + "_LOCAL_RESULTS_VERTICES.txt");

	//fs << "Resultado del Local Surface Counting - CON MEDIA DE ÁREAS DE TRIÁNGULOS INCIDENTES" << "\n";	
	//fs << "En cada línea se muestra la MEDIA normalizada de las áreas de los triángulos incidentes en el vértice para cada reconstrucción de grado l" << "\n";	
	//std::cout << "Grabando resultados a disco\n";
	//for(size_t i = 0; i < lsc.size(); i++) {
	//	fs << i << ": ";
	//	for(size_t j = 0; j < lsc[i].size(); j++) {
	//		fs << lsc[i][j].area << " ";
	//	}
	//	fs << "\n";
	//}
	//fs.close();
}

/*************************************************************************/

void surface_processing::calcula_Global_SH_FD_media_local_SHFD(std::string fichero) {
/* Calcula la Dimension Fractal Global como la media de las local SHFD presentes en el fichero.
    Este fichero debe contener en cada línea la shfd local del vértice correspondiente, en el siguiente formato:
      a (shfd) b error_a error_b r
    Devuelve el valor de shfd global (a) y el resto de parámetros asociados a la recta de regresión
    b, error_a, error_b y r, como la media de los parámetros correspondientes para cada vértice y lo
    almacena en un fichero con nombre fichero_GLOBAL_SHFD.txt */

  float n, a, b, error_a, error_b, r,
        suma_a, suma_b, suma_error_a, suma_error_b, suma_r;

	/* se leen los valores de local shfd de todos los vértices */
	FILE *fin = fopen(fichero.c_str(),"r");
	n = 0;
	suma_a = suma_b = suma_error_a = suma_error_b = suma_r = 0.0;
	while (1) {
		if (fscanf(fin, "%f %f %f %f %f\n", &a, &b, &error_a, &error_b, &r) == EOF)
			break;
		suma_a += a;
		suma_b += b;
		suma_error_a += error_a;
		suma_error_b += error_b;
		suma_r += r;
		n++;
	}
	fclose(fin);

	suma_a /= n;
	suma_b /= n;
	suma_error_a /= n;
	suma_error_b /= n;
	suma_r /= n;

	/* se graba a disco el resultado de la global shfd */
	FILE *fs = fopen((fichero + "_GLOBAL_SHFD.txt").c_str(),"w");

	/* se graba en formato a b error_a error_b r */
	fprintf(fs,"%f\t%f\t%f\t%f\t%f\n", suma_a, suma_b, suma_error_a, suma_error_b, suma_r);
	fclose(fs);
}

/*************************************************************************/

void surface_processing::calcula_Global_SH_FD_sin_unknown(std::string fichero, std::string fichero_etiq, int l_ini, int l_fin) {
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

  float n,
        suma_x,suma_y,suma_xy,suma_x2,x,y,media_x,media_y,
        delta,suma_delta,
        suma_xymedia,suma_xmedia2,suma_ymedia2,
				a, b, error_a, error_b, r;

	SILT::DFSurface recons; /* superficie que almacena una reconstruccion pasada a formato DFS */ 

	std::vector<sc_ele> surf_count; /* vector que almacena los datos para el calculo de la Global SH-FD:
					     - fichero .SURF de la superficie i
				           - grado de la superficie i
				       - area de la superficie i */

	/* se carga la superficie original con las etiquetas para cada vértice */
	leer_asc_etiq((char *) fichero_etiq.c_str());

	/* carga los nombres de los ficheros SURF con todas las reconstrucciones SH y el grado de cada una */
	if (!busca_ficheros_SURF(fichero, surf_count)/*busca_ficheros_OBJ(fichero, surf_count)*/) {
		mexPrintf("No encuentro ficheros SURF para calcular la Global SH-FD\n");
		return;
	}

	/* Para cada reconstruccion SH en formato SURF, hay que pasarla a formato DFS y calcular el 
 	   área de la superficie */
	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
		mexPrintf("Leyendo superficie: %s\n", it->fichero);
		/* convertir a DFS la malla SURF */
		if (!convierte_SURF_a_DFS(it->fichero, recons)/*convierte_OBJ_a_DFS(it->fichero, recons)*/) {
			mexPrintf("\nNo se puede convertir fichero %s a DFS\n", it->fichero);
			exit(0);
		};

		/* se calcula el área de la superficie de grado l, SIN CONSIDERAR LOS TRIÁNGULOS CON ALGÚN VÉRTICE UNKNOWN */
		mexPrintf("Calculando area grado %d\n", (int) it->l);
		it->area = calcula_area_superficie_sin_unknown(recons, this);
	}


	/* las áreas se normalizan respecto al área de la superficie original */
	/* en Yotter 2011 se considera como superficie original la SH reconstrucción
 	   de mayor grado l. 
	   Yo normalizo respecto de la superficie original devuelta por FreeSurfer, sin considerar los triángulos
 	   que tienen alguno de sus vértices clasificado como unknown */
	double area_original = calcula_area_superficie_sin_unknown(this->s, this);
	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
		/* se normaliza el área dividiendo por el área de la superficie original (grado l máximo) */
		it->area = it->area / area_original;
	}

	/* calculo de la recta aX + b = 0 para cada vértice i */
	suma_x=suma_y=suma_xy=suma_x2=suma_delta=media_x=media_y=0;
	n=(float)l_fin-l_ini+1;
	for(size_t i=l_ini;i<=l_fin;i++) {
		//x=log(1.0/(float)i); // versión box-counting
		x=log((float)i);
		//x=log(log((float)i)); // con el doble log se consiguen valores razonables de df3d para la mayoría de los vértices
		y=log(surf_count[i-1].area); // l se mide desde 1, pero en el vector se almacena desde 0
		suma_x=suma_x+x;
		suma_x2=suma_x2+(x*x);
		suma_y=suma_y+y;
		suma_xy=suma_xy+(x*y);
	}
	media_x=suma_x/n;
	media_y=suma_y/n;
  	a=(n*suma_xy - suma_x*suma_y)/(n*suma_x2 - suma_x * suma_x);
	b=(suma_y - (a)*suma_x)/n;
	/* calculo de los errores en a y en b */
	suma_xymedia=suma_xmedia2=suma_ymedia2=0;
	for(size_t i=l_ini;i<=l_fin;i++) {
		// x=log(1.0/(float)i); // versión box-counting
		//x=log(log((float)i));
		x=log((float)i);
		y=log(surf_count[i-1].area);
    		suma_delta=suma_delta+ (y - (a)*x - (b)) * (y - (a)*x - (b));
		suma_xymedia=suma_xymedia+ ((x-media_x)*(y-media_y));
		suma_xmedia2=suma_xmedia2+ ((x-media_x)*(x-media_x));
		suma_ymedia2=suma_ymedia2+ ((y-media_y)*(y-media_y));
	}
	delta=sqrt(suma_delta/(n-2));
	error_a=(sqrt(n)*delta) / (sqrt(n*suma_x2 - suma_x*suma_x));
	error_b=(error_a)*sqrt(suma_x2/n);
	r=suma_xymedia/(sqrt(suma_xmedia2)*sqrt(suma_ymedia2));
	/* se graba a disco el resultado de la global shfd */
	char ini[5],fin[5];
	sprintf(ini,"%d",l_ini);
	sprintf(fin,"%d",l_fin);
    	FILE *fs = fopen((fichero + "_GLOBAL_SHFD_SIN_UNKNOWN_" + ini + "_" + fin + "_NORMALIZANDO.txt").c_str(),"w");
	fprintf(fs, "#Global Spherical Harmonic Fractal Dimension (triangles with any vertex classified as unknown by FreeSurfer are ignored)\n");
	fprintf(fs, "#File: %s\n",fichero.c_str());
	fprintf(fs, "#Range l = %d to l = %d \n",l_ini,l_fin);
	fprintf(fs, "#Fractal dimension (a) - b - error_a - error_b - r  \n"); 
	/* se graba en formato a b error_a error_b r */
	fprintf(fs,"%f\t%f\t%f\t%f\t%f\n", a, b, error_a, error_b, r);
	fclose(fs);

	/* se graba a disco el resultado del surface-counting */
	//	FILE *fs = fopen((fichero + "_RESULTS.txt").c_str(),"w");
	//	fprintf(fs,"Resultado del Surface Counting\n");
	//	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
	//		fprintf(fs,"%d\t%10.10f\n", it->l, it->area);
	//	}
	//	fclose(fs);
}

/*************************************************************************/

void surface_processing::calcula_Global_SH_FD_media_local_SHFD_SIN_UNKNOWN(std::string fichero, std::string fichero_etiq) {
/* Calcula la Dimension Fractal Global como la media de las local SHFD presentes en el fichero.
 Este fichero debe contener en cada línea la shfd local del vértice correspondiente, en el siguiente formato:
   a (shfd) b error_a error_b r
 LOS VALORES CORRESPONDIENTES A LOS VÉRTICES CLASIFICADOS COMO UNKNOWN POR FREESURFER NO SE TIENEN EN CUENTA
 Devuelve el valor de shfd global (a) y el resto de parámetros asociados a la recta de regresión
 b, error_a, error_b y r, como la media de los parámetros correspondientes para cada vértice y lo
 almacena en un fichero con nombre fichero_GLOBAL_SHFD_SIN_UNKNOWN.txt */

  float ver, n, unknown, a, b, error_a, error_b, r,
		       suma_a, suma_b, suma_error_a, suma_error_b, suma_r;
	short int et;

	/* se carga la superficie original con las etiquetas para cada vértice */
	leer_asc_etiq((char *) fichero_etiq.c_str());

	/* se leen los valores de local shfd de todos los vértices */
	FILE *fin = fopen(fichero.c_str(),"r");
	ver = n = unknown = 0;
	suma_a = suma_b = suma_error_a = suma_error_b = suma_r = 0.0;
	while (1) {
		if (fscanf(fin, "%f %f %f %f %f\n", &a, &b, &error_a, &error_b, &r) == EOF)
			break;

		et = s.vertexLabels[ver]; /* índice de etiqueta del vértice */

		if ((*(get_etiquetas()))[et] != "unknown")  {
			suma_a += a;
			suma_b += b;
			suma_error_a += error_a;
			suma_error_b += error_b;
			suma_r += r;
			n++;
		} else {
			unknown++;
		}

		ver++;
	}
	fclose(fin);
	mexPrintf("He encontrado %d vertices unknown\n", unknown);

	suma_a /= n;
	suma_b /= n;
	suma_error_a /= n;
	suma_error_b /= n;
	suma_r /= n;

	/* se graba a disco el resultado de la global shfd */
	FILE *fs = fopen((fichero + "_GLOBAL_SHFD_SIN_UNKNOWN.txt").c_str(),"w");

	/* se graba en formato a b error_a error_b r */
	fprintf(fs,"%f\t%f\t%f\t%f\t%f\n", suma_a, suma_b, suma_error_a, suma_error_b, suma_r);
	fclose(fs);
}

/*************************************************************************/

void surface_processing::calcula_Global_SH_FD(std::string fichero, int l_ini, int l_fin) {
/* calcula la Dimension Fractal Global a partir de las reconstrucciones basadas en harmónicos esféricos
   de la superficie utilizando el método descrito en Yotter 2011 "Local cortical surface complexity maps 
   from spherical harmonic reconstructions".
   El fichero pasado como parámetro es el nombre de una de las reconstrucciones SH de la superficie para
   el grado indicado en el propio nombre del fichero. El fichero debe estar en formato SURF (Amira). A partir del
   nombre del fichero se buscan en el mismo directorio el resto de reconstrucciones SH necesarias para el
   cálculo de la DF Global basada en SH.
   Devuelve el valor de dimensión fractal (a) y el resto de parámetros asociados a la recta de regresión
   b, error_a, error_b y r, almacenándolos en un fichero con nombre fichero_GLOBAL_SHFD_ini_fin.txt */

  float n,
        suma_x,suma_y,suma_xy,suma_x2,x,y,media_x,media_y,
        delta,suma_delta,
        suma_xymedia,suma_xmedia2,suma_ymedia2,
        a, b, error_a, error_b, r;

	SILT::DFSurface recons; /* superficie que almacena una reconstruccion pasada a formato DFS */ 

	std::vector<sc_ele> surf_count; /* vector que almacena los datos para el calculo de la Global SH-FD:
					   - fichero .SURF de la superficie i
					   - grado de la superficie i
					   - area de la superficie i */

	/* carga los nombres de los ficheros SURF con todas las reconstrucciones SH y el grado de cada una */
	if (!busca_ficheros_SURF(fichero, surf_count)/*busca_ficheros_OBJ(fichero, surf_count)*/) {
		mexPrintf("No encuentro ficheros SURF para calcular la Global SH-FD\n");
		return;
	}

	/* Para cada reconstruccion SH en formato SURF, hay que pasarla a formato DFS y calcular el 
           área de la superficie */
	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
		mexPrintf("Leyendo superficie: %s\n", it->fichero.c_str());
		/* convertir a DFS la malla SURF */
		if (!convierte_SURF_a_DFS(it->fichero, recons)/*convierte_OBJ_a_DFS(it->fichero, recons)*/) {
			mexPrintf("\nNo se puede convertir fichero %s a DFS\n", it->fichero.c_str());
			exit(0);
		};

		/* se calcula el área de la superficie de grado l */
		mexPrintf("Calculando area grado %d\n", (int) it->l);
		it->area = calcula_area_superficie(recons);
	}

	/* las áreas tienen que ser normalizadas respecto al área de la superficie original */
	/* siguiendo lo realizado en Yotter 2011, se considera como superficie original la SH reconstrucción
 * 	   de mayor grado l */
//	surface_processing::sc_ele max = surf_count.back();
//	//	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
//	//		/* se normaliza el área dividiendo por el área de la superficie original (grado l máximo) */
//	//		it->area = it->area / max.area;
//	//	}
//

		/* calculo de la recta aX + b = 0 para cada vértice i */
		suma_x=suma_y=suma_xy=suma_x2=suma_delta=media_x=media_y=0;
		n=(float)l_fin-l_ini+1;
		for(size_t i=l_ini;i<=l_fin;i++) {
		  //x=log(1.0/(float)i); // versión box-counting
  		  //x=log(log((float)i)); // con el doble log se consiguen valores razonables de df3d para la mayoría de los vértices
 		  x=log((float)i); 
		  y=log(surf_count[i-1].area); // l se mide desde 1, pero en el vector se almacena desde 0
		  suma_x=suma_x+x;
                  suma_x2=suma_x2+(x*x);
		  suma_y=suma_y+y;
			suma_xy=suma_xy+(x*y);
		}
		media_x=suma_x/n;
		media_y=suma_y/n;

	  	a=(n*suma_xy - suma_x*suma_y)/(n*suma_x2 - suma_x * suma_x);
  		b=(suma_y - (a)*suma_x)/n;

		/* calculo de los errores en a y en b */
		suma_xymedia=suma_xmedia2=suma_ymedia2=0;
		for(size_t i=l_ini;i<=l_fin;i++) {
			// x=log(1.0/(float)i); // versión box-counting
			//x=log(log((float)i));
			x=log((float)i);
			y=log(surf_count[i-1].area);
		  											    
	    		suma_delta=suma_delta+ (y - (a)*x - (b)) * (y - (a)*x - (b));
			suma_xymedia=suma_xymedia+ ((x-media_x)*(y-media_y));
			suma_xmedia2=suma_xmedia2+ ((x-media_x)*(x-media_x));
			suma_ymedia2=suma_ymedia2+ ((y-media_y)*(y-media_y));
		}
		delta=sqrt(suma_delta/(n-2));
		error_a=(sqrt(n)*delta) / (sqrt(n*suma_x2 - suma_x*suma_x));
		error_b=(error_a)*sqrt(suma_x2/n);
		r=suma_xymedia/(sqrt(suma_xmedia2)*sqrt(suma_ymedia2));

		/* se graba a disco el resultado de la global shfd */
		char ini[5],fin[5];
	  	sprintf(ini,"%d",l_ini);
		sprintf(fin,"%d",l_fin);

	    	FILE *fs = fopen((fichero + "_GLOBAL_SHFD_" + ini + "_" + fin + ".txt").c_str(),"w");
                fprintf(fs, "#Global Spherical Harmonic Fractal Dimension\n");
                fprintf(fs, "#File: %s\n",fichero.c_str());
                fprintf(fs, "#Range l = %d to l = %d \n",l_ini,l_fin);
                fprintf(fs, "#Fractal dimension (a) - b - error_a - error_b - r  \n"); 
   		/* se graba en formato a b error_a error_b r */
		fprintf(fs,"%f\t%f\t%f\t%f\t%f\n", a, b, error_a, error_b, r);

		/* se graban las áreas de cada superficie */
    fprintf(fs, "#Area of each reconstruction\n");
		for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
			fprintf(fs,"%f ", it->area);
		}
		fprintf(fs,"\n");
		fclose(fs);
						
		/* se graba a disco el resultado del surface-counting */
		//	FILE *fs = fopen((fichero + "_RESULTS.txt").c_str(),"w");
		//	fprintf(fs,"Resultado del Surface Counting\n");
		//	for(std::vector<sc_ele>::iterator it = surf_count.begin(); it != surf_count.end(); it++) {
		//		fprintf(fs,"%d\t%10.10f\n", it->l, it->area);
		//	}
		//	fclose(fs);
	}


/*************************************************************************/

void surface_processing::genera_STL_o_M(std::string fichero) {
/* convierte la superficie a formato STL (si se parte de .DFS) o a 
   formato Matlab .m (si se parte de .ETIQ) y la almacena en el fichero proporcionado:
		a partir del nombre proporcionado, se le elimina la extension .dfs y se generan
		dos ficheros uno añadiendo la extension .stl o .m y otro añadiendo 
		la extension .landmarkAscii (para procesamiento posterior con SPHARMA) */

  FILE *fstl,
		   *fm,
		   *fland;
	std::string fichero_stl, fichero_m;
	std::string fichero_land;

	float maxx,maxy,maxz,minx,miny,minz;
		
  /* creación de fichero para .stl o .m*/
	/* ver si el fichero es .DFS o .ETIQ */
	std::string fin1 = fichero.substr(fichero.length()-4,4);
	std::string fin2 = fichero.substr(fichero.length()-5,5);
	std::transform(fin1.begin(), fin1.end(), fin1.begin(), ::toupper);
	std::transform(fin2.begin(), fin2.end(), fin2.begin(), ::toupper);

	if (fin1 == ".DFS") {
		fichero_stl = fichero.substr(0,fichero.size()-4) + ".stl";
		fichero_land = fichero.substr(0,fichero.size()-4) + ".landmarkAscii";
	} else if (fin2 == ".ETIQ") {
		fichero_m = fichero.substr(0,fichero.size()-5) + ".m";
		fichero_land = fichero.substr(0,fichero.size()-5) + ".landmarkAscii";
	} else {
		mexErrMsgTxt("genera_STL: Error: solo se admiten fichero .DFS o .ETIQ");
		return;
	}

	if (fin1 == ".DFS") {
		fstl=fopen(fichero_stl.c_str(),"w");

		/* cabecera */
		fprintf(fstl,"solid %s (%d vertices, %d triangulos)\n", fichero_stl.c_str(), s.vertices.size(), s.triangles.size());

		/* lanzamiento al archivo de los triángulos */
		for (size_t i = 0; i < s.triangles.size(); i++) {
			fprintf(fstl,"  facet normal %f %f %f\n",normalesTriangulos[i].x,normalesTriangulos[i].y,normalesTriangulos[i].z);
			fprintf(fstl,"    outer loop\n");

			fprintf(fstl,"      vertex %f %f %f\n",s.vertices[s.triangles[i].a].x,
																							s.vertices[s.triangles[i].a].y,
																							s.vertices[s.triangles[i].a].z);
			fprintf(fstl,"      vertex %f %f %f\n",s.vertices[s.triangles[i].b].x,
																							s.vertices[s.triangles[i].b].y,
																							s.vertices[s.triangles[i].b].z);
			fprintf(fstl,"      vertex %f %f %f\n",s.vertices[s.triangles[i].c].x,
																							s.vertices[s.triangles[i].c].y,
																							s.vertices[s.triangles[i].c].z);

			fprintf(fstl,"    endloop\n");
			fprintf(fstl,"  endfacet\n");
		}

		/* fin archivo */
		fprintf(fstl,"endsolid\n");
		fclose(fstl);
	}

	if (fin2 == ".ETIQ") {
		fm=fopen(fichero_m.c_str(),"w");

		/* cabecera */
    fprintf(fm, "%% Superficie generada a partir de un fichero .asc de FreeSurfer\n");
		fprintf(fm, "%% superficie %s (%d vertices, %d triangulos)\n", fichero_m.c_str(), s.vertices.size(), s.triangles.size());

		/* lanzamiento al archivo de los vértices */
    fprintf(fm,"surface = struct('vertices', [");
		for (size_t i = 0; i < s.vertices.size(); i++) {
			fprintf(fm, "%f %f %f;...\n", s.vertices[i].x, s.vertices[i].y, s.vertices[i].z);
		}
		fprintf(fm,"], 'faces', [");

		/* lanzamiento al archivo de los triángulos */
		for (size_t i = 0; i < s.triangles.size(); i++) {
			// hay que sumar 1 porque los ficheros .m son procesados por Matlab y los arrays de matlab empiezan en 1
			fprintf(fm, "%d %d %d;...\n", s.triangles[i].a + 1, s.triangles[i].b + 1, s.triangles[i].c + 1);
		}
    fprintf(fm, "]);\n");
    fprintf(fm, "p = patch(surface);");

		/* fin archivo */
		fclose(fm);
	}

	/* fichero con landmarks */
  fland=fopen(fichero_land.c_str(),"w");

	minx = s.vertices[0].x; maxx = s.vertices[0].x;
	miny = s.vertices[0].y; maxy = s.vertices[0].y;
	minz = s.vertices[0].z; maxz = s.vertices[0].z;

	for(size_t i = 1; i < s.vertices.size(); i++) {
		if (s.vertices[i].x < minx) minx = s.vertices[i].x;
		if (s.vertices[i].x > maxx) maxx = s.vertices[i].x;
		if (s.vertices[i].y < miny) miny = s.vertices[i].y;
		if (s.vertices[i].y > maxy) maxy = s.vertices[i].y;
		if (s.vertices[i].z < minz) minz = s.vertices[i].z;
		if (s.vertices[i].z > maxz) maxz = s.vertices[i].z;
	}

	fprintf(fland, "# UJA-3DFD\n\n\n");
	fprintf(fland, "define Markers 8\n");
	fprintf(fland, "Parameters {\n");
	fprintf(fland, "    NumSets 1,\n");
	fprintf(fland, "    ContentType \"LandmarkSet\"\n");
	fprintf(fland, "}\n\n");
	fprintf(fland, "Markers { float[3] Coordinates } @1\n\n");
	fprintf(fland, "# Data section follows\n");
	fprintf(fland, "@1\n");

	fprintf(fland,"%f %f %f\n", minx, miny, minz);
	fprintf(fland,"%f %f %f\n", minx, miny, maxz);
	fprintf(fland,"%f %f %f\n", maxx, miny, maxz);
	fprintf(fland,"%f %f %f\n", maxx, miny, minz);

	fprintf(fland,"%f %f %f\n", minx, maxy, minz);
	fprintf(fland,"%f %f %f\n", minx, maxy, maxz);
	fprintf(fland,"%f %f %f\n", maxx, maxy, maxz);
	fprintf(fland,"%f %f %f\n", maxx, maxy, minz);

	fclose(fland);
}

/*************************************************************************/

void calcula_centro_gravedad(SILT::DFSurface &s, float &x, float &y, float &z) {
/* calcula el centro de gravedad de la superficie y lo devuelve en (x, y, z) */

	float xx = 0.0,
		    yy = 0.0,
				zz = 0.0;

	for (size_t i = 0; i < s.vertices.size(); i++) {
		xx = xx + s.vertices[i].x;
		yy = yy + s.vertices[i].y;
		zz = zz + s.vertices[i].z;
	}

	x = xx/s.vertices.size();
	y = yy/s.vertices.size();
	z = zz/s.vertices.size();
}

/*************************************************************************/

void calcula_normal(float x1, float y1, float z1,
		     					  float x2, float y2, float z2,
				    			  float &xn, float &yn, float &zn) {

  xn = (y1 * z2) - (z1 * y2);
  yn = (z1 * x2) - (x1 * z2);
  zn = (x1 * y2) - (y1 * x2);

	double l = sqrt((xn * xn) + (yn * yn) + (zn * zn));
	xn = xn / l;
	yn = yn / l;
	zn = zn / l;
}

/*************************************************************************/

void calcula_normales(surface_processing *dfs) {
/* calcula la normal a cada triangulo de la superficie y la almacena en dfs->normalesTriangulos */

	float x1,y1,z1,x2,y2,z2;
	SILT::DFSurface *ss = dfs->get_sup();
	std::vector<SILT::Point3D> *nor = dfs->get_normalesTriangulos();
	SILT::Point3D normal;

	nor->resize(ss->triangles.size());

	for (size_t i = 0; i < (dfs->get_sup())->triangles.size(); i++) {
		x1 = ss->vertices[ss->triangles[i].b].x - ss->vertices[ss->triangles[i].a].x;
		y1 = ss->vertices[ss->triangles[i].b].y - ss->vertices[ss->triangles[i].a].y;
		z1 = ss->vertices[ss->triangles[i].b].z - ss->vertices[ss->triangles[i].a].z;

		x2 = ss->vertices[ss->triangles[i].c].x - ss->vertices[ss->triangles[i].a].x;
		y2 = ss->vertices[ss->triangles[i].c].y - ss->vertices[ss->triangles[i].a].y;
		z2 = ss->vertices[ss->triangles[i].c].z - ss->vertices[ss->triangles[i].a].z;

		calcula_normal(x1, y1, z1, x2, y2, z2, normal.x, normal.y, normal.z);

		(*nor)[i].x = normal.x;
		(*nor)[i].y = normal.y;
		(*nor)[i].z = normal.z;
	}
}

/*************************************************************************/

void surface_processing::inicia_array_vertices_normales() {
/* reserva la memoria e inicializa el array de normales en vertices */

	size_t nv = s.vertices.size();

	array_normales = new float[nv * 3];

	for (size_t i = 0; i < (nv * 3); i++) {
		array_normales[i] = 0;
	}
}

/*************************************************************************/

void calcula_array_vertices_OpenGL(surface_processing *dfs) {
/* calcula el array de vertices y normales para visualizar la superficie 
   utilizando un array de vertices de OpenGL,
	 REQUIERE que previamente se haya calculado las normales a cada triangulo
	 de la superficie */

	SILT::DFSurface *ss = dfs->get_sup();
	std::vector<SILT::Point3D> *norTri = dfs->get_normalesTriangulos();

	float *nor;

	size_t nt = ss->triangles.size();
	size_t nv = ss->vertices.size();

	unsigned char *nor_por_vertice; /* numero de normales incidentes en cada vertice */

	dfs->inicia_array_vertices_normales();
	dfs->get_array_normales(nor);

	/* se calcula la normal a cada vertice como la media 
	   de las normales de las caras incidentes en el vertice */
	nor_por_vertice = new unsigned char[nv];
	for (size_t i = 0; i < nv; i++) 
		nor_por_vertice[i] = 0;

	for (size_t i = 0; i < nt; i++) {
		/* indices a los vertices del triangulo i */
		int ind[3] = {ss->triangles[i].a, 
			            ss->triangles[i].b, 
									ss->triangles[i].c};

		/* se acumula la normal al triangulo i en las normales a los tres vertices del triangulo i */
		nor[ind[0] * 3]       += (*norTri)[i].x;
		nor[(ind[0] * 3) + 1] += (*norTri)[i].y;
		nor[(ind[0] * 3) + 2] += (*norTri)[i].z;

		nor[ind[1] * 3]       += (*norTri)[i].x;
		nor[(ind[1] * 3) + 1] += (*norTri)[i].y;
		nor[(ind[1] * 3) + 2] += (*norTri)[i].z;

		nor[ind[2] * 3]       += (*norTri)[i].x;
		nor[(ind[2] * 3) + 1] += (*norTri)[i].y;
		nor[(ind[2] * 3) + 2] += (*norTri)[i].z;

		/* se actualiza el numero de normales acumuladas en cada vertice del triangulo i */
		nor_por_vertice[ind[0]] += 1;
		nor_por_vertice[ind[1]] += 1;
		nor_por_vertice[ind[2]] += 1;
	}

	/* se calcula la normal en cada vertice como la media de las normales de los triangulos
	   incidentes en el vertice, como se tiene acumulado la suma de las normales solo queda
		 dividir por el numero de triangulos incidentes en el vertice */
	for (size_t i = 0; i < nv; i++) {
		nor[i*3] =     nor[i*3]     / nor_por_vertice[i];
		nor[i*3 + 1] = nor[i*3 + 1] / nor_por_vertice[i];
		nor[i*3 + 2] = nor[i*3 + 2] / nor_por_vertice[i];
	}

	delete nor_por_vertice;
}

/*************************************************************************/

void cambia_orientacion_triangulos(SILT::DFSurface &s) {
	/* se cambia la orientacion de los triangulos de la superficie para que la normal
	   salga hacia afuera de la superficie */

	int aux;

	for (size_t i = 0; i < s.triangles.size(); i++) {
		aux = s.triangles[i].a;
		s.triangles[i].a = s.triangles[i].b;
		s.triangles[i].b = aux;
	}
}

/*************************************************************************/

int lee_fichero_ASC_ETIQ(char *fichero, surface_processing *dfs) {
/* lee el fichero .asc.etiq y almacena en dfs->sup los vértices, triángulos, colores e índices a las etiquetas
   de la superficie. Se crea en dfs el array con los nombres de las etiquetas. 
	 Cualquier contenido previo en sup queda borrado */

	SILT::DFSurface *sup = dfs->get_sup();

	FILE * file = fopen(fichero, "r");
	if( file == NULL ){
		mexPrintf("lee_fichero_ASC_ETIQ: No se puede abrir el fichero %s\n", fichero);
		return 0;
	}

	/* borra los triángulos, vértices y colores que ya hubiera en sup */
	sup->triangles.resize(0);
	sup->vertices.resize(0);
	sup->vertexColors.resize(0);
	sup->vertexLabels.resize(0);

	/* borra el array de etiquetas que ya hubiera en dfs */
	dfs->get_etiquetas()->resize(0);

	while( 1 ) {
		char lineHeader[128];
		
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
 
    // else : parse lineHeader
		if ( strcmp( lineHeader, "v" ) == 0 ) { 
			/* se lee un vertice */
			SILT::Point3D vertex,color;
			int etiq;

			fscanf(file, "%f %f %f %f %f %f %d\n", &vertex.x, &vertex.y, &vertex.z, &color.x, &color.y, &color.z, &etiq);
			// se guarda el vértice
			sup->vertices.push_back(vertex);

			// se pasa a rango 0..1 y se guarda el color
			color.x /= 255.0;color.y /= 255.0;color.z /= 255.0;
			sup->vertexColors.push_back(color);

			// se guarda la etiqueta (su índice al array de etiquetas)
			sup->vertexLabels.push_back(etiq);
		} else if ( strcmp( lineHeader, "f" ) == 0 ){ 
			/* se lee una cara */
			SILT::Triangle triangulo;
			fscanf(file, "%d %d %d\n", &triangulo.a, &triangulo.b, &triangulo.c);
			sup->triangles.push_back(triangulo);
		} else if (strcmp( lineHeader, "e" ) == 0) { 
			/* se lee una etiqueta al array de etiquetas */
			char label[256];
			fscanf(file, "%s\n", &label);
			dfs->get_etiquetas()->push_back(label);
		} else { /* cualquier otra linea se ignora */
			fgets(lineHeader, 128, file);
		}
	}		

	fclose(file);
	return 1;
}

/*************************************************************************/

int surface_processing::leer_asc_etiq(char *fichero) {
/* lee el fichero .asc.etiq conteniendo la descripción de una superficie
   de Freesurfer con vértices, colores, etiquetas y caras
	 y carga la superficie en sup
   devuelve 0 si no se puede leer el fichero, 1 en caso contrario */

	/* se lee la superficie desde el fichero y se crea el objeto superficie */
	if (!lee_fichero_ASC_ETIQ(fichero,this)) return 0;

	/* se cambia la orientacion de los triangulos de la superficie */
	//cambia_orientacion_triangulos(s);

	/* se calcula el centro de gravedad de la superficie */
	calcula_centro_gravedad(s,centro[0], centro[1], centro[2]);

	/* calcula la normal a cada triangulo para utilizarla en la visualizacion 
	   de la superficie */
	calcula_normales(this);

	/* calcula array de vertices y normales para visualizacion OpenGL */
	calcula_array_vertices_OpenGL(this);

	return 1;
}

/*************************************************************************/

int surface_processing::leer_dfs(char *fichero) {
/* lee el fichero .dfs y carga la superficie en sup
   devuelve 0 si no se puede leer el fichero, 1 en caso contrario */

	std::string ifname(fichero);

	/* se lee la superficie desde el fichero y se crea el objeto superficie */
	if (s.readDFS(ifname)==false) return 0;

	/* se cambia la orientacion de los triangulos de la superficie */
	cambia_orientacion_triangulos(s);

	/* se calcula el centro de gravedad de la superficie */
	calcula_centro_gravedad(s,centro[0], centro[1], centro[2]);

	/* calcula la normal a cada triangulo para utilizarla en la visualizacion 
	   de la superficie */
	calcula_normales(this);

	/* calcula array de vertices y normales para visualizacion OpenGL */
	calcula_array_vertices_OpenGL(this);

	return 1;
}

/*************************************************************************/

void surface_processing::pinta_dfs (void) {
/* visualiza una superfice .dfs */

	//const size_t nt = s.triangles.size();

	///* propiedades de material */
 // GLfloat material_ambiente[]= {0.5, 0.5, 0.5};
 // GLfloat material_difuso[]=   {0.5, 0.5, 0.5};
 // GLfloat material_especular[]={0.5, 0.5, 0.5};
	//GLfloat material_emision[]=  {0.0, 0.0, 0.0};
	//GLfloat Ns = 128.0;
	//
	//glMaterialfv(GL_FRONT,GL_AMBIENT,material_ambiente);
 // glMaterialfv(GL_FRONT,GL_DIFFUSE,material_difuso);
	//glMaterialfv(GL_FRONT,GL_SPECULAR,material_especular);
	//glMaterialf(GL_FRONT,GL_SHININESS, Ns);
	//glMaterialfv(GL_FRONT,GL_EMISSION,material_emision);

	///* creacion de array de vertices para visualizar la superficie */
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3,GL_FLOAT,0,s.vertices.data());
	//
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glNormalPointer(GL_FLOAT,0,array_normales);

	//glEnableClientState(GL_COLOR_ARRAY);
	//glColorPointer(3,GL_FLOAT,0,s.vertexColors.data());

	//if ((s.vertices.size() < 1) || (s.vertexColors.size() < 1) || (array_normales == NULL)) {
	//	printf("Pinta_dfs: Error, para pintar se necesitan vértices, normales y colores\n");
	//	exit(0);
	//}

	//glDrawElements(GL_TRIANGLES,nt * 3,GL_UNSIGNED_INT,s.triangles.data());
	//
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);




	/* visualizacion triangulo a triangulo */
	//GLfloat color_triangulo[]={1.0,0.0,0.0};
	//for (size_t i = 0; i< nt; i++) {
	//	/* normal al triangulo */
	//	//glNormal3f(normalesTriangulos[i].x, normalesTriangulos[i].y , normalesTriangulos[i].z);

	//	/* color del triangulo */
	//	if (false/*s.vertexColors.size() == s.vertices.size()*/) { /* la superficie tiene colores asignados */
	//		color_triangulo[0] = s.vertexColors[s.triangles[i].a].x;
	//		color_triangulo[1] = s.vertexColors[s.triangles[i].a].y;
	//		color_triangulo[2] = s.vertexColors[s.triangles[i].a].z;
	//	} else {
	//		color_triangulo[0] = 0.25;
	//		color_triangulo[1] = 0.0;
	//		color_triangulo[2] = 0.0;
	//	}

	//	//glMaterialfv(GL_FRONT,GL_EMISSION,color_triangulo);

	//	glBegin(GL_TRIANGLES);
	//		glColor3d(s.vertexColors[s.triangles[i].a].x, s.vertexColors[s.triangles[i].a].y, s.vertexColors[s.triangles[i].a].z);
	//		glNormal3f(array_normales[s.triangles[i].a * 3], array_normales[s.triangles[i].a * 3 + 1], array_normales[s.triangles[i].a * 3 + 2]);
	//		glVertex3d(s.vertices[s.triangles[i].a].x, s.vertices[s.triangles[i].a].y, s.vertices[s.triangles[i].a].z);

	//		glColor3d(s.vertexColors[s.triangles[i].b].x, s.vertexColors[s.triangles[i].b].y, s.vertexColors[s.triangles[i].b].z);
	//		glNormal3f(array_normales[s.triangles[i].b * 3], array_normales[s.triangles[i].b * 3 + 1], array_normales[s.triangles[i].b * 3 + 2]);
	//		glVertex3d(s.vertices[s.triangles[i].b].x, s.vertices[s.triangles[i].b].y, s.vertices[s.triangles[i].b].z);

	//		glColor3d(s.vertexColors[s.triangles[i].c].x, s.vertexColors[s.triangles[i].c].y, s.vertexColors[s.triangles[i].c].z);
	//		glNormal3f(array_normales[s.triangles[i].c * 3], array_normales[s.triangles[i].c * 3 + 1], array_normales[s.triangles[i].c * 3 + 2]);
	//		glVertex3d(s.vertices[s.triangles[i].c].x, s.vertices[s.triangles[i].c].y, s.vertices[s.triangles[i].c].z);
	//	glEnd();
	//}

}

/*************************************************************************/

void surface_processing::get_centro(float *c) {
/* devuelve el centro de gravedad de la superficie en el array c */

	c[0] = centro[0];
	c[1] = centro[1];
	c[2] = centro[2];
}

SILT::DFSurface *surface_processing::get_sup(void){
/* devuelve un puntero a la superficie dfs */

	return &s;

}

std::vector<SILT::Point3D> *surface_processing::get_normalesTriangulos(void) {
/* devuelve un puntero al vector con las normales a los triangulos */

	return &normalesTriangulos;
}

void surface_processing::get_array_normales(float *&nor) {
/* devuelve el puntero al array de normales a vertices */

	nor = array_normales;
}

std::vector<std::string> *surface_processing::get_etiquetas(void) {
/* devuelve el puntero al vector de etiquetas */

	return &etiquetas;
}
