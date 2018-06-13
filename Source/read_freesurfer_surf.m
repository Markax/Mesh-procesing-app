function [vertex_coords, faces] = read_freesurfer_surf(fname)
% reads the vertex coordinates and face lists from a freesurfer surface file

TRIANGLE_FILE_MAGIC_NUMBER =  16777214 ;
QUAD_FILE_MAGIC_NUMBER =  16777215 ;

fid = fopen(fname, 'rb', 'b') ;
if (fid < 0)
  str = sprintf('could not open surface file %s.', fname) ;
  error(str) ;
end

wb = waitbar(0, 'Loading mesh file...');
magic = fread3(fid) ;
if(magic == QUAD_FILE_MAGIC_NUMBER)
  waitbar(1/8, wb, 'Loading mesh file...');
  vnum = fread3(fid) ;
  fnum = fread3(fid) ;
  vertex_coords = fread(fid, vnum*3, 'int16') ./ 100 ; 
  waitbar(2/7, wb, 'Loading mesh file...');
  if (nargout > 1)
    for i=1:fnum
      for n=1:4
        faces(i,n) = fread3(fid) ;
      end
      waitbar(3/7, wb, 'Loading mesh file...');
    end
    waitbar(4/7, wb, 'Loading mesh file...');
  end
elseif (magic == TRIANGLE_FILE_MAGIC_NUMBER)
  waitbar(5/7, wb, 'Loading mesh file...');
  fgets(fid) ;
  fgets(fid) ;
  vnum = fread(fid, 1, 'int32') ;
  fnum = fread(fid, 1, 'int32') ;
  vertex_coords = fread(fid, vnum*3, 'float32') ; 
  faces = fread(fid, fnum*3, 'int32') ;
  faces = reshape(faces, 3, fnum)' ;
  waitbar(6/7, wb, 'Loading mesh file...');
end

vertex_coords = reshape(vertex_coords, 3, vnum)' ;
faces = faces + 1; % freesurfer indexes from 0 and MATLAB indexes from 1 

fclose(fid) ;

waitbar(1, wb, 'Loading mesh file...');
close(wb);

%------------------------------------------------------------------
function [retval] = fread3(fid)
% read a 3 byte integer out of a file
b1 = fread(fid, 1, 'uchar') ;
b2 = fread(fid, 1, 'uchar') ;
b3 = fread(fid, 1, 'uchar') ;

retval = bitshift(b1, 16) + bitshift(b2,8) + b3 ;
