function obj = readObj(fname)
%
% obj = readObj(fname)
%
% This function parses wavefront object data
% It reads the mesh vertices, texture coordinates, normal coordinates
% and face definitions(grouped by number of vertices) in a .obj file 
% 
%
% INPUT: fname - wavefront object file full path
%
% OUTPUT: obj.v - mesh vertices
%       : obj.vt - texture coordinates
%       : obj.vn - normal coordinates
%       : obj.f - face definition assuming faces are made of of 3 vertices
%
% Bernard Abayowa, Tec^Edge
% 11/8/07

% set up field types
v = []; vt = []; vn = []; f.v = []; f.vt = []; f.vn = [];

fid = fopen(fname);

%variables of waitbar
s = dir(char(fname));
l = s.bytes / 23;
  
w = int16(l/100);
it = 0;
ib = 0;

%waitbar
wb = waitbar(ib, 'Loading mesh file...');

% parse .obj file 
while 1      
    it = it+1;
      if (it == w)
          it = 0;
          ib = ib+0.01;
          waitbar(ib, wb, 'Loading mesh file...');
      end


    tline = fgetl(fid);
    if ~ischar(tline),   break,   end  % exit at end of file 
     ln = sscanf(tline,'%s',1); % line type 
     %disp(ln)
    switch ln
        case 'v'   % mesh vertexs
            v = [v; sscanf(tline(2:end),'%f')'];
        case 'vt'  % texture coordinate
            vt = [vt; sscanf(tline(3:end),'%f')'];
        case 'vn'  % normal coordinate
            vn = [vn; sscanf(tline(3:end),'%f')'];
        case 'f'   % face definition
            fv = []; fvt = []; fvn = [];
            str = textscan(tline(2:end),'%s'); str = str{1};
       
           nf = length(findstr(str{1},'/')); % number of fields with this face vertices


           [tok str] = strtok(str,'//');     % vertex only
            for k = 1:length(tok) fv = [fv str2num(tok{k})]; end
           
            if (nf > 0) 
            [tok str] = strtok(str,'//');   % add texture coordinates
                for k = 1:length(tok) fvt = [fvt str2num(tok{k})]; end
            end
            if (nf > 1) 
            [tok str] = strtok(str,'//');   % add normal coordinates
                for k = 1:length(tok) fvn = [fvn str2num(tok{k})]; end
            end
             f.v = [f.v; fv]; f.vt = [f.vt; fvt]; f.vn = [f.vn; fvn];
    end
end
fclose(fid);
waitbar(1, wb, 'Loading mesh file...');
  
close(wb);

% set up matlab object 
obj.v = v; obj.vt = vt; obj.vn = vn; obj.f = f.v;
trisurf(obj.f , obj.v(:,1), obj.v(:,2), obj.v(:,3),'FaceColor',[0.26,0.33,1.0 ]);
shading interp
colormap gray(256);
lighting phong;
light('Position',[-1 0 0],'Style','local');
camproj('perspective');
axis square; 
axis off;
axis equal
axis tight;
cameratoolbar
%display_obj(obj, 0);
