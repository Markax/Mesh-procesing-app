function [vertex_coords, faces] = surf2mat(fname)

% Modificación realizada al script de Chung que cargaba una superficie de
% FreeSurfer para cargar una superficie SURF conteniendo una reconstrucción
% de SPHARM

% [vertex_coords, faces] = read_surf(fname)
% reads a the vertex coordinates and face lists from a surface file
% note that reading the faces from a quad file can take a very long
% time due to the goofy format that they are stored in. If the faces
% output variable is not specified, they will not be read so it 
% should execute pretty quickly.
%

disp('surf2mat: empezando a convertir');

fid = fopen(fname, 'r') ;
if (fid < 0)
    disp(['surf2mat: error, no puedo abrir el fichero: ' fname]);
    return;
end

% leer los vértices
linea = fgetl(fid);
if (size(linea,2) > 8)
    lineac = linea(1:8);
end
while ~strcmp(lineac,'Vertices')
    linea = fgetl(fid);
    if (size(linea,2) > 8)
        lineac = linea(1:8);
    end
end

format long; % para no perder precisión al leer los flotantes desde fichero

vnum = sscanf(linea,'%*s %f');
vertex_coords = zeros(3, vnum);
for i = 1:vnum
    linea = fgetl(fid);
    ver_coor = sscanf(linea,'%f');
    vertex_coords(1,i) = ver_coor(1,1);   
    vertex_coords(2,i) = ver_coor(2,1);   
    vertex_coords(3,i) = ver_coor(3,1);   
end

% leer los triángulos
linea = fgetl(fid);
if (size(linea,2) > 9)
    lineac = linea(1:9);
end
while ~strcmp(lineac,'Triangles')
    linea = fgetl(fid);
    if (size(linea,2) > 9)
        lineac = linea(1:9);
    end
end

fnum = sscanf(linea,'%*s %f');
faces = zeros(3, fnum);
for i = 1:fnum
    linea = fgetl(fid);
    tri_ind = sscanf(linea,'%d');
    faces(1,i) = tri_ind(1,1);   
    faces(2,i) = tri_ind(2,1);   
    faces(3,i) = tri_ind(3,1);   
end


%{
  fgets(fid) ;
  fgets(fid) ;
  vnum = fread(fid, 1, 'int32') ;
  fnum = fread(fid, 1, 'int32') ;
  vertex_coords = fread(fid, vnum*3, 'float32') ; 

  faces = fread(fid, fnum*3, 'int32') ;
  faces = reshape(faces, 3, fnum)' ;

  vertex_coords = reshape(vertex_coords, 3, vnum)' ;
%}

vertex_coords = vertex_coords';
faces = faces';

fclose(fid) ;
disp('surf2mat: terminado');
