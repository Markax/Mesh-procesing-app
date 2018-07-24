fid = fopen('../Config/config.txt');

sigma = fgetl(fid);
numiter = fgetl(fid);
ini = str2double(fgetl(fid)); 
fin = str2double(fgetl(fid));
ini = str2double(fgetl(fid)); 
fin = str2double(fgetl(fid));

fid2 = fopen('../Config/actualview.txt');

fpath = fgetl(fid2);
[filepath,name] = fileparts(fpath);
min_L = str2double(fgetl(fid2));
max_L = str2double(fgetl(fid2));
rec = max_L-min_L;

r = zeros(1, rec);
for i=min_L:max_L
    r(1,i) = i;
end

n = zeros(1, rec);

 disp(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_GLOBAL_SHFD_',int2str(ini),'_',int2str(fin),'.txt'));
f_area = fopen(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_GLOBAL_SHFD_',int2str(ini),'_',int2str(fin),'.txt'));

A = fgetl(f_area);
    
while ischar(A)
    A = fgetl(f_area);
    if (A~=-1)
        B = fscanf(f_area, '%f');
    end
end

n = B(1:rec+1);
n = n';
% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área de la reconstrucción con l máxima

N = log(n)';
R = log(r)';

figure('Name', 'Linear Regression - Global', 'NumberTitle', 'off');
scatter(R, N);
hold on;
 
% linear regression computation
Rr = R(ini : fin); % R limited to the selected range of boxes
Nr = N(ini : fin); % N limited to the selected range of boxes
 
x = [ones(length(Rr), 1) Rr]; % adds a column of ones to Rr
b = x \ Nr; % b(1) is the y-intercept and b(2) is the slope of the line 
y = x * b; % linear regression
corr = 1 - sum((Nr - y).^2) / sum((Nr - mean(Nr)).^2); % correlation
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);
xlabel('log(l)');
ylabel('log(normalized area)');


