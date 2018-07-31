function varargout = FractalDimensionLocal(varargin)
% FRACTALDIMENSIONLOCAL MATLAB code for FractalDimensionLocal.fig
%      FRACTALDIMENSIONLOCAL, by itself, creates a new FRACTALDIMENSIONLOCAL or raises the existing
%      singleton*.
%
%      H = FRACTALDIMENSIONLOCAL returns the handle to a new FRACTALDIMENSIONLOCAL or the handle to
%      the existing singleton*.
%
%      FRACTALDIMENSIONLOCAL('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in FRACTALDIMENSIONLOCAL.M with the given input arguments.
%
%      FRACTALDIMENSIONLOCAL('Property','Value',...) creates a new FRACTALDIMENSIONLOCAL or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before FractalDimensionLocal_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to FractalDimensionLocal_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help FractalDimensionLocal

% Last Modified by GUIDE v2.5 31-Jul-2018 21:43:15

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @FractalDimensionLocal_OpeningFcn, ...
                   'gui_OutputFcn',  @FractalDimensionLocal_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before FractalDimensionLocal is made visible.
function FractalDimensionLocal_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to FractalDimensionLocal (see VARARGIN)

% Choose default command line output for FractalDimensionLocal
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes FractalDimensionLocal wait for user response (see UIRESUME)
% uiwait(handles.LocalGUI);
axes(handles.axes1);
cla(handles.axes1);
fid = fopen('../Config/config.txt');

sigma = fgetl(fid);
numiter = fgetl(fid);
global ini;
ini = str2double(fgetl(fid)); 
global fin;
fin = str2double(fgetl(fid));

fid2 = fopen('../Config/actualview.txt');

fpath = fgetl(fid2);
[filepath,name] = fileparts(fpath);
global min_L;
min_L = str2double(fgetl(fid2));
global max_L;
max_L = str2double(fgetl(fid2));

fvert = fopen(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_LOCAL_RESULTS_VERTICES_HKS_', sigma,'_', numiter,'.txt'));
global rec 
rec = str2double(fgetl(fvert));
global n_vert
n_vert = str2double(fgetl(fvert));

act_vert = round(n_vert/2);
set(handles.slider1, 'Max', n_vert);
set(handles.text14, 'String', n_vert);
set(handles.text13, 'String', '1');
set(handles.slider1, 'Value', round(act_vert));
set(handles.text3, 'String', round(act_vert));
set(handles.slider1, 'SliderStep', [1/(n_vert), 0.1])

set(handles.slider2, 'Min', min_L);
set(handles.text8, 'String', min_L);
set(handles.slider2, 'Max', max_L);
set(handles.text9, 'String', max_L);
set(handles.slider2, 'Value', round(ini));
set(handles.text6, 'String', round(ini));
set(handles.slider2, 'SliderStep', [1/(max_L-min_L), 0.1])

set(handles.slider3, 'Min', min_L);
set(handles.text10, 'String', min_L);
set(handles.slider3, 'Max', max_L);
set(handles.text11, 'String', max_L);
set(handles.slider3, 'Value', round(fin));
set(handles.text7, 'String', round(fin));
set(handles.slider3, 'SliderStep', [1/(max_L-min_L), 0.1])

global VValues;
VValues = fscanf(fvert, '%f');

n = zeros(1, rec);
for i=1:rec
   n(1,i) = VValues(rec*(act_vert-1)+i,1); 
end

global r;

r = zeros(1, rec);
for i=min_L:max_L
    r(1,i) = i;
end

% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área asociada al vértice para la l máxima

N = log(n)';
R = log(r)';

scatter(R, N);
hold on;
 
% linear regression computation
Rr = R(ini : fin); % R limited to the selected range of boxes
Nr = N(ini : fin); % N limited to the selected range of boxes
 
x = [ones(length(Rr), 1) Rr]; % adds a column of ones to Rr
b = x \ Nr; % b(1) is the y-intercept and b(2) is the slope of the line 
y = x * b; % linear regression
corr = 1 - sum((Nr - y).^2) / sum((Nr - mean(Nr)).^2); % correlation

set(handles.text12, 'String', sprintf('Linear regression correlation = %f', corr));
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);
xlabel('log(l)');
ylabel('log(normalized average area)');




% --- Outputs from this function are returned to the command line.
function varargout = FractalDimensionLocal_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on slider movement.
function slider1_Callback(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
set(handles.text3, 'String', round(get(handles.slider1, 'Value')));
set(handles.slider1, 'Value', round(get(handles.slider1, 'Value')));

global rec;
global VValues;
global ini;
ini = get(handles.slider2, 'Value');
global fin;
fin = get(handles.slider3, 'Value');
act_vert = get(handles.slider1, 'Value');
global r;

n = zeros(1, rec);
for i=1:rec
   n(1,i) = VValues(rec*(act_vert-1)+i,1); 
end

cla(handles.axes1);

% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área asociada al vértice para la l máxima

N = log(n)';
R = log(r)';

scatter(R, N);
hold on;
 
% linear regression computation
Rr = R(ini : fin); % R limited to the selected range of boxes
Nr = N(ini : fin); % N limited to the selected range of boxes
 
x = [ones(length(Rr), 1) Rr]; % adds a column of ones to Rr
b = x \ Nr; % b(1) is the y-intercept and b(2) is the slope of the line 
y = x * b; % linear regression
corr = 1 - sum((Nr - y).^2) / sum((Nr - mean(Nr)).^2); % correlation

set(handles.text12, 'String', sprintf('Linear regression correlation = %f', corr));
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);




% --- Executes during object creation, after setting all properties.
function slider1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% saving new regression values
fid = fopen('../Config/config.txt');
old_sigma = fgetl(fid);
old_niter = fgetl(fid);
for i=1:3
    old_minGlob = fgetl(fid);
end
old_maxGlob = fgetl(fid);

fclose(fid);
fsave = fopen('../Config/config.txt', 'w');
fprintf(fsave, '%s \n', old_sigma);
fprintf(fsave, '%s \n', old_niter);
fprintf(fsave, '%d \n', get(handles.slider2, 'Value'));
fprintf(fsave, '%d \n', get(handles.slider3, 'Value'));
fprintf(fsave, '%s \n', old_minGlob);
fprintf(fsave, '%s \n', old_maxGlob);
fclose(fsave);



fview = fopen('../Config/actualview.txt');

global fpath;
fpath = fgetl(fview);
fclose(fview);

[filepath,name] = fileparts(fpath);

h = findobj('Tag', 'MainGUI');
if ~isempty(h)
    g1data = guidata(h);
    axes(g1data.axes2);
    compute_local_shfd(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_LOCAL_RESULTS_VERTICES_HKS_', old_sigma,'_', old_niter,'.txt'), int32(get(handles.slider2, 'Value')), int32(get(handles.slider3, 'Value')));
    visualiza_mapa_local_shfd(strcat(filepath,'\',name,'_temp','\',name,'.m'), strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_LOCAL_RESULTS_VERTICES_HKS_',old_sigma,'_',old_niter,'.local_shfd_',int2str(get(handles.slider2, 'Value')),'_',int2str(get(handles.slider3, 'Value')),'.txt'));
end
    
axes(handles.axes1);

close

%fvset = fopen ('../Config/actualview.txt');
%fpath = fgetl(fvset);
%[filepath,name] = fileparts(fpath);
%fclose(fvset);

%fig = get(groot,'CurrentFigure');
%figure(fig)
%set(0, 'CurrentFigure', fig);

%compute_local_shfd(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_LOCAL_RESULTS_VERTICES_HKS_', old_sigma,'_', old_niter,'.txt'), int32(get(handles.slider2, 'Value')), int32(get(handles.slider3, 'Value')));

% --------------------------------------------------------------------
function uipushtool1_ClickedCallback(hObject, eventdata, handles)
% hObject    handle to uipushtool1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close


% --- Executes on slider movement.
function slider2_Callback(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
set(handles.text6, 'String', round(get(handles.slider2, 'Value')));
set(handles.slider2, 'Value', round(get(handles.slider2, 'Value')));

if ( get(handles.slider3, 'Value') <= get(handles.slider2, 'Value') )
    errordlg('Max L value must be greater than Min L value');
    set(handles.text6, 'String', round(get(handles.slider2, 'Value'))-1);
    set(handles.slider2, 'Value', round(get(handles.slider2, 'Value'))-1);
else

global rec;
global VValues;
global ini;
ini = get(handles.slider2, 'Value');
global fin;
fin = get(handles.slider3, 'Value');
act_vert = get(handles.slider1, 'Value');
global r;

n = zeros(1, rec);
for i=1:rec
   n(1,i) = VValues(rec*(act_vert-1)+i,1); 
end

cla(handles.axes1);

% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área asociada al vértice para la l máxima

N = log(n)';
R = log(r)';

scatter(R, N);
hold on;
 
% linear regression computation
Rr = R(ini : fin); % R limited to the selected range of boxes
Nr = N(ini : fin); % N limited to the selected range of boxes
 
x = [ones(length(Rr), 1) Rr]; % adds a column of ones to Rr
b = x \ Nr; % b(1) is the y-intercept and b(2) is the slope of the line 
y = x * b; % linear regression
corr = 1 - sum((Nr - y).^2) / sum((Nr - mean(Nr)).^2); % correlation

set(handles.text12, 'String', sprintf('Linear regression correlation = %f', corr));
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);

end


% --- Executes during object creation, after setting all properties.
function slider2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function slider3_Callback(hObject, eventdata, handles)
% hObject    handle to slider3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
set(handles.text7, 'String', round(get(handles.slider3, 'Value')));
set(handles.slider3, 'Value', round(get(handles.slider3, 'Value')));

if ( get(handles.slider3, 'Value') <= get(handles.slider2, 'Value') )
    errordlg('Max L value must be greater than Min L value');
    set(handles.text7, 'String', round(get(handles.slider2, 'Value'))+1);
    set(handles.slider3, 'Value', round(get(handles.slider2, 'Value'))+1);
else

global rec;
global VValues;
global min_L;
global max_L;
global ini;
ini = get(handles.slider2, 'Value');
global fin;
fin = get(handles.slider3, 'Value');
act_vert = get(handles.slider1, 'Value');
global r;

n = zeros(1, rec);
for i=1:rec
   n(1,i) = VValues(rec*(act_vert-1)+i,1); 
end

cla(handles.axes1);

% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área asociada al vértice para la l máxima

N = log(n)';
R = log(r)';

scatter(R, N);
hold on;
 
% linear regression computation
Rr = R(ini : fin); % R limited to the selected range of boxes
Nr = N(ini : fin); % N limited to the selected range of boxes
 
x = [ones(length(Rr), 1) Rr]; % adds a column of ones to Rr
b = x \ Nr; % b(1) is the y-intercept and b(2) is the slope of the line 
y = x * b; % linear regression
corr = 1 - sum((Nr - y).^2) / sum((Nr - mean(Nr)).^2); % correlation

set(handles.text12, 'String', sprintf('Linear regression correlation = %f', corr));
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);

end


% --- Executes during object creation, after setting all properties.
function slider3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close
