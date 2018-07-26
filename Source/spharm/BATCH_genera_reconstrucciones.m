function BATCH_genera_reconstrucciones(modelo_suavizado,inicio,fin)
    % partiendo de un fichero conteniendo un template con el resize y el
    % smoothing hecho (template_*_OL_2O_CS_smo.mat o template_*_OL_2O_smo.mat) genera las
    % reconstrucciones desde el grado "inicio" al grado "fin"
    
    wb = waitbar(0, 'Generating reconstructions...  (4/6)', 'Name', 'Generating Spherical Harmonics');
    
    inicio = double(inicio); % para evitar problemas de tiepos de datos al llamar desde funcion mex
    fin = double(fin); % para evitar problemas de tiepos de datos al llamar desde funcion mex

    for i = inicio:fin
        waitbar(i/fin, wb, 'Generating reconstructions...  (4/6)');
        res = BATCH_MLMakeTemplate(modelo_suavizado, 0, i);
        res = BATCH_MLMakeModels(res, modelo_suavizado, 0, i, 0);
        BATCH_MLMakeSurfacesFromSPHARMModels(res, 0, 'Amira');
    end
    
    close(wb);
end
