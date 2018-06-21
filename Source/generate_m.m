function m_file = generate_m(fname)

[filepath,name,ext] = fileparts(fname);

if (isequal(ext, '.obj'))
    % Generacion de ficheros m y land 
    
    fichero_m = strcat(fname( 1 : length(fname)-3),'m');
    
    fichero_land = strcat(fname(1 : length(fname)-3), 'landmarkAscii');
else
    if (isequal(ext, '.ETIQ'))
        fichero_m = strcat(fname( 1 : length(fname)-3) + 'm');
    
        fichero_land = strcat(fname(1 : length(fname)-3), 'landmarkAscii');
    else
        disp('genera_M: Error: solo se admiten fichero .OBJ o .ETIQ');
        return;
    end    
end

if (isequal(ext, '.obj'))
    fid = fopen(fichero_m,'w');
    s = readObj(fname);
    
    % Header 
    fprintf(fid, '%% Superficie generada a partir de un fichero .obj \n');
    fprintf(fid, '%% superficie %s (%d vertices, %d triangulos)\n', name, length(s.v), length(s.f));
    
    % lanzamiento al archivo de los vértices 
    fprintf(fid,'surface = struct(''vertices'', [...\n');

    for i = 1:size(s.v)
        fprintf(fid, '%f %f %f;...\n', s.v(i,1), s.v(i,2), s.v(i,3));
    end
    fprintf(fid,'], ''faces'', [');

    % lanzamiento al archivo de los triángulos 
    for i = 1:size(s.f)
		fprintf(fid, "%d %d %d;...\n", s.f(i,1), s.f(i,2), s.f(i,3));
    end
    
    
    fprintf(fid, ']);\n');
    fprintf(fid, 'p = patch(surface);');
    
    % fin archivo
    fclose(fid);

end

if (isequal(ext, '.ETIQ'))
    
    fid = fopen(fichero_m,'w');
    
    % Header 
    fprintf(fid, '%% Superficie generada a partir de un fichero .asc de FreeSurfer\n');
    fprintf(fid, '%% superficie %s (%d vertices, %d triangulos)\n', name, length(s.v), length(s.f));
    
    % lanzamiento al archivo de los vértices 
    fprintf(fm,'surface = struct(''vertices'', [');

    for i = 1:s.length
			fprintf(fm, '%f %f %f;...\n', s.vertices(i).x, s.vertices(i).y, s.vertices(i).z);
    end
    fprintf(fid,'], ''faces'', [');

    % lanzamiento al archivo de los triángulos 
    for i = 1:size(s.f)
		fprintf(fid, "%d %d %d;...\n", s.f(i,1), s.f(i,2), s.f(i,3));
    end
    
    
    fprintf(fid, ']);\n');
    fprintf(fid, 'p = patch(surface);');
    
    % fin archivo
    fclose(fm);
    
end

% fichero con landmarks
fland=fopen(fichero_land,'w');

minx = s.v(1,1); maxx = s.v(1,1);
miny = s.v(1,2); maxy = s.v(1,2);
minz = s.v(1,3); maxz = s.v(1,3);

for i = 1:size(s.v)
		if (s.v(i,1) < minx) minx = s.v(i,1); end
		if (s.v(i,1) > maxx) maxx = s.v(i,1); end
		if (s.v(i,2) < miny) miny = s.v(i,2); end
		if (s.v(i,2) > maxy) maxy = s.v(i,2); end
		if (s.v(i,2) < minz) minz = s.v(i,3); end
		if (s.v(i,2) > maxz) maxz = s.v(i,3); end
end

fprintf(fland, '# UJA-SHFD\n\n\n');
fprintf(fland, 'define Markers 8\n');
fprintf(fland, 'Parameters {\n');
fprintf(fland, '    NumSets 1,\n');
fprintf(fland, '    ContentType \''LandmarkSet\''\n');
fprintf(fland, '}\n\n');
fprintf(fland, 'Markers { float[3] Coordinates } @1\n\n');
fprintf(fland, '# Data section follows\n');
fprintf(fland, '@1\n');

fprintf(fland,'%f %f %f\n', minx, miny, minz);
fprintf(fland,'%f %f %f\n', minx, miny, maxz);
fprintf(fland,'%f %f %f\n', maxx, miny, maxz);
fprintf(fland,'%f %f %f\n', maxx, miny, minz);

fprintf(fland,'%f %f %f\n', minx, maxy, minz);
fprintf(fland,'%f %f %f\n', minx, maxy, maxz);
fprintf(fland,'%f %f %f\n', maxx, maxy, maxz);
fprintf(fland,'%f %f %f\n', maxx, maxy, minz);

fclose(fland);