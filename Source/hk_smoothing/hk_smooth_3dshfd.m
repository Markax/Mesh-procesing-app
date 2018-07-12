function hk_smooth_3dshfd(fich_areas,sigma,num_iter)
% Esta funci�n recibe como par�metros un fichero con las �reas
% correspondientes a los v�rtices de varias SH reconstrucciones y realiza
% sobre ellas un heat kernel smoothing de par�metros sigma y num_iter
% iteraciones.
% El fichero fich_areas se genera desde UJA-3DSHFD, desde la funci�n
% calcula_Local_SH_FD_por_vertices()
% El fichero fich_areas debe tener en su primera linea el n�mero de
% reconstrucciones y en la segunda l�nea el n�mero de v�rtices en cada reconstrucci�n 
% A continuaci�n, aparecer�n las �reas a suavizar correspondientes a cada
% v�rtice
% La funci�n genera un fichero con el mismo nombre introducido pero acabado
% en "LOCAL_RESULTS_VERTICES_HKS.txt" conteniendo en cada l�nea las �reas SUAVIZADAS con HKS 
% para el v�rtice i para cada reconstrucci�n de grado l

sigma = double(sigma);
num_iter = double(num_iter);

wb = waitbar(0, 'Smoothing mesh...  (5/5)', 'Name', 'Generating Spherical Harmonics');

format long; % para no perder precisi�n al leer los flotantes desde fichero

disp('hk_smooth_3dshfd: comienza proceso');
fid = fopen(fich_areas, 'r') ;
if (fid < 0)
    disp(['hk_smooth_3dshfd: error, no puedo abrir el fichero con las areas: ' fich_areas]);
    return;
end
    
% lee el n�mero de reconstrucciones
linea = fgetl(fid);
num_rec = sscanf(linea,'%d');

% lee el n�mero de v�rtices en cada reconstrucci�n
linea = fgetl(fid);
num_vert = sscanf(linea,'%d');

input = zeros(num_vert, 1);

% areas_smooth almacena en cada fila las �reas suavizadas para ese v�rtice en cada reconstrucci�n
areas_smooth = zeros(num_vert, num_rec); 

% para cada reconstrucci�n, se cargan las �reas de los v�rtices y se
% aplica y almacena el heat kernel smoothing
for rec = 1:num_rec
    
    waitbar(rec/(num_rec), wb, 'Smoothing mesh...  (5/5)');
    % se lee el nombre del fichero SURF para la reconstruccion rec
    fich_surf = sscanf(fgetl(fid),'%s');
    %disp('**************************');
    %disp(['Procesando reconstrucci�n: ' fich_surf]);
    %disp('**************************');
    
    % el .surf se convierte a formato .mat para que sea legible por hk_smmoth.m
    %disp('Convirtiendo de surf a mat');
    [v,f] = surf2mat(fich_surf);
    surf.vertices = v; 
    surf.faces = f;

    % se leen las areas para la reconstrucion rec
    %disp('Leyendo areas');
    for vert = 1:num_vert
        area = sscanf(fgetl(fid),'%f');
        input(vert,1) = area;
    end
    
    % se aplica el heat kernel smoothing
    %disp('Aplicando heat kernel smoothing');
    output = hk_smooth(input,surf,sigma,num_iter);

    % el resultado se pasa a areas_smooth
    areas_smooth(:,rec) = output;
end

fclose(fid);

% se genera el fichero de resultados
%disp('hk_smooth_3dshfd: guardando fichero con resultados');
fichero = fich_areas(1:size(fich_areas,2)-22);
fichero = [fichero 'LOCAL_RESULTS_VERTICES_HKS_'  int2str(sigma) '_' int2str(num_iter) '.txt'];
fid = fopen(fichero, 'w') ;
if (fid < 0)
    disp('hk_smooth_3dshfd: error, no puedo abrir el fichero para guardar los resultados');
    return;
end

%fprintf(fid,'Resultado del Local Surface Counting - SUMANDO �REAS DE TRI�NGULOS INCIDENTES - SUAVIZADAS con HKS\n');
%fprintf(fid,'En cada l�nea se muestra la suma SUAVIZADA con HKS de las �reas de los tri�ngulos incidentes en el v�rtice para cada reconstrucci�n de grado l\n');

% se graba el n�mero de reconstrucciones
fprintf(fid,'%d\n', num_rec);

% se graba el n�mero de v�rtices
fprintf(fid,'%d\n', num_vert);

for i = 1:num_vert
    %fprintf(fid,'%d: ',i-1);
	for j = 1:num_rec
        fprintf(fid,'%f ',areas_smooth(i,j));
    end
    fprintf(fid,'\n');
end
fclose(fid);

waitbar(1, wb, 'Smoothing mesh...  (5/5)');

disp('hk_smooth_3dshfd: terminado');
close(wb);

%{ 
% Prueba con n�meros aleatorios
input = random('unif',2,6,[size(v,1),1]);
figure_trimesh(surf,input,'rwb');

disp('hk_smooth_3dshfd: haciendo heat kernel smoothing');
output = hk_smooth(input,surf,sigma,num_iter);
figure_trimesh(surf,output,'rwb'); 
%}

