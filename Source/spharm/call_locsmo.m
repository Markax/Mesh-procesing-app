function verts = call_locsmo(param_vs,obj_vs,faces,extents,reso,name);
% call locsmo.exe
%

vnum = size(param_vs,1); fnum = size(faces,1);
% save to infile
%disp(sprintf('save param_vs, obj_vs, faces, extents, reso to infile ...'));
fid = fopen([name '_infile'],'wb');
fwrite(fid, [reso vnum fnum], 'int');
fwrite(fid, extents, 'double');
fwrite(fid, param_vs, 'double');
fwrite(fid, obj_vs, 'double');
fwrite(fid, faces, 'int');
fclose(fid);

% remove remeshout
%oo = [name '_outfile'];
%if exist(oo)
%    %!rm oo
%    system(['rm' oo]);
%end

%!spa infile outfile 4
LocalSmoothing([name '_infile'],[name '_outfile']);

% read from outfile
%disp(sprintf('read verts from outfile ...'));
fid = fopen([name '_outfile'],'r');
verts = fread(fid, vnum*3, 'double'); 
verts = reshape(verts,size(param_vs));
fclose(fid);

return;

