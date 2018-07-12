function visualiza_mapa_local_shfd(superficie,shfd)

% Esta funci�n recibe una superficie en formato .M (la utilizada como punto de partida en SPHARM) 
% o .SURF (la utilizada como salida de una reconstrucci�n de SPHARM) 
% y un fichero donde en cada l�nea hay un valor de dimensi�n fractal asociado al v�rtice
% correspondiente de la superficie. Por defecto utiliza el colormap 'rywb' 
% que devuelve un rango desde el rojo al azul.
% La funci�n visualiza la superficie utilizando como color en cada v�rtice
% su valor de dimensi�n fractal, permitiendo ver de manera visual los valores de
% dimensi�n fractal asociados a cada zona de la superficie

format long; % para no perder precisi�n al leer los flotantes desde fichero

disp('visualiza_mapa_local_shfd: comienza proceso');
addpath(genpath('figure_trimesh'));

% se pasa la superficie a formato .mat
disp(['Leyendo superficie: ' superficie]);
[path, name, extension] = fileparts(superficie);
if (strcmp(extension,'.surf') || strcmp(extension,'.SURF'))
    [v,f] = surf2mat(superficie); % para leer superficies .SURF devueltas por SPHARM
else
    if (strcmp(extension,'.m') || strcmp(extension,'.M'))
    [v,f] = LeerMFile(superficie); % para leer superficies .M generadas por UJA-3DSHFD
    else
        disp('Error: s�lo se admiten superficies en formato .m o .surf');
        return;
    end
end

surf.vertices = v; 
surf.faces = f;
num_vert = size(surf.vertices,1);

% se leen los valores de local shfd para cada v�rtice de la superficie
disp('Leyendo valores de shfd... ');
output = zeros(num_vert, 1);

fid = fopen(shfd, 'r') ;
if (fid < 0)
    disp(['visualiza_mapa_local_shfd: error, no puedo abrir el fichero con las shfd: ' shfd]);
    return;
end
for vert = 1:num_vert
    linea = fgetl(fid);
    if (linea < 0)
        disp('visualiza_mapa_local_shfd: error, n�mero insuficiente de valores de shfd en el fichero');
        return;
    end
    fd = sscanf(linea,'%f');
    output(vert,1) = fd(1);
end

linea = fgetl(fid);
if (linea ~= -1)
    disp('visualiza_mapa_local_shfd: error, sobran valores de shfd en el fichero');
    return;
end

fclose(fid);

disp('Terminado. Visualizando...');
% llama a c�digo de Chung
figure_trimesh(surf,output,'rywb'); 
end

function [vertices, faces] = LeerMFile(fname)
% Read triangular mesh from an m file
% Reads a triangular mesh containing vertices and faces from a
% .m file that contains these variables in a structure in Matlab format.
% The function assumes that these variables come in the order of
% 'vertices', then 'faces'.  They must also be
% identified by these variable names with all lowercase.  The variables
% 'vertices' and 'faces' must be present.

vertices = [];
faces = [];

disp(fname);

fid=fopen(fname);
while 1
    tline = fgetl(fid);
    % test for end of file
    if (~ischar(tline))
        break
    end
    %if line is empty
    if (strcmp(tline,''))
        continue
    end
    % if line is a comment, pass by
    if (strcmp(tline(1),'%'))
        continue
    end
    % found beginning of structure
    if (strcmp(tline(1:7),'surface'))
        % find if vertices is located in this line
        k = strfind(tline, '''vertices''');
        if (isempty(k))
            errordlg('File does not contain vertices first.','Wrong file format.','modal');
            break
        end
        % line contains vertices, so get first data
        startIndex = regexp(tline,'[');
        endIndex = regexp(tline, ';');
        a = str2num(tline((startIndex+1):(endIndex-1)));
        vertices(end+1,:) = a;
        % get rest of vertices
        while 1
            tline = fgetl(fid);
            if (strcmp(tline(1),']'))
                % go on to find faces
                break
            end
            % another line of data
            endIndex = regexp(tline, ';');
            if (~isempty(endIndex)) % pass over empty lines
                a = str2num(tline(1:(endIndex-1)));
                vertices(end+1,:) = a;
            end
        end
        % get faces
        k = strfind(tline, '''faces''');
        if (isempty(k))
            errordlg('File does not contain faces after vertices.','Wrong file format.','modal');
            break
        end
        % line contains vfaces, so get first data
        startIndex = regexp(tline,'[');
        endIndex = regexp(tline, ';');
        a = str2num(tline((startIndex+1):(endIndex-1)));
        faces(end+1,:) = a;
        % get rest of faces
        while 1
            tline = fgetl(fid);
            if (strcmp(tline(1),']'))
                % go on to find landmarks
                break
            end
            % another line of data
            endIndex = regexp(tline, ';');
            if (~isempty(endIndex)) % pass over empty lines
                a = str2num(tline(1:(endIndex-1)));
                faces(end+1,:) = a;
            end
        end
    end
end
fclose(fid);
end
