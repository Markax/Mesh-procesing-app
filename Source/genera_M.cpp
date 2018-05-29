void dfs_processing::genera_STL_o_M(std::string fichero) {
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
		std::cout << "genera_STL: Error: solo se admiten fichero .DFS o .ETIQ";
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
