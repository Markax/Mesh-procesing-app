function varargout = FractalDimensionGlobal(varargin)
% FRACTALDIMENSIONGLOBAL MATLAB code for FractalDimensionGlobal.fig
%      FRACTALDIMENSIONGLOBAL, by itself, creates a new FRACTALDIMENSIONGLOBAL or raises the existing
%      singleton*.
%
%      H = FRACTALDIMENSIONGLOBAL returns the handle to a new FRACTALDIMENSIONGLOBAL or the handle to
%      the existing singleton*.
%
%      FRACTALDIMENSIONGLOBAL('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in FRACTALDIMENSIONGLOBAL.M with the given input arguments.
%
%      FRACTALDIMENSIONGLOBAL('Property','Value',...) creates a new FRACTALDIMENSIONGLOBAL or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before FractalDimensionGlobal_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to FractalDimensionGlobal_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help FractalDimensionGlobal

% Last Modified by GUIDE v2.5 27-Jul-2018 13:35:08

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @FractalDimensionGlobal_OpeningFcn, ...
                   'gui_OutputFcn',  @FractalDimensionGlobal_OutputFcn, ...
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


% --- Executes just before FractalDimensionGlobal is made visible.
function FractalDimensionGlobal_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to FractalDimensionGlobal (see VARARGIN)

% Choose default command line output for FractalDimensionGlobal
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes FractalDimensionGlobal wait for user response (see UIRESUME)
% uiwait(handles.figure1);
axes(handles.axes1);
cla(handles.axes1);
fid = fopen('../Config/config.txt');

sigma = fgetl(fid);
numiter = fgetl(fid);
global ini;
global fin;
ini = str2double(fgetl(fid)); 
fin = str2double(fgetl(fid));
ini = str2double(fgetl(fid)); 
fin = str2double(fgetl(fid));

fid2 = fopen('../Config/actualview.txt');

fpath = fgetl(fid2);
[filepath,name] = fileparts(fpath);
global min_L
min_L = str2double(fgetl(fid2));
global max_L
max_L = str2double(fgetl(fid2));
global rec;
rec = max_L-min_L;

global VAreas;

set(handles.slider2, 'Min', min_L);
set(handles.text5, 'String', min_L);
set(handles.slider2, 'Max', max_L);
set(handles.text3, 'String', max_L);
set(handles.slider2, 'Value', round(ini));
set(handles.text4, 'String', round(ini));
set(handles.slider2, 'SliderStep', [1/(max_L-min_L), 0.1])

set(handles.slider3, 'Min', min_L);
set(handles.text11, 'String', min_L);
set(handles.slider3, 'Max', max_L);
set(handles.text9, 'String', max_L);
set(handles.slider3, 'Value', round(fin));
set(handles.text10, 'String', round(fin));
set(handles.slider3, 'SliderStep', [1/(max_L-min_L), 0.1])

global r;
r = zeros(1, rec);
for i=min_L:max_L
    r(1,i) = i;
end


f_area = fopen(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_GLOBAL_SHFD_',int2str(ini),'_',int2str(fin),'.txt'));

A = fgetl(f_area);
    
while ischar(A)
    A = fgetl(f_area);
    if (A~=-1)
        VAreas = fscanf(f_area, '%f');
    end
end

global n;
n = VAreas(1:rec+1);
n = n';
% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área de la reconstrucción con l máxima

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
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);
xlabel('log(l)');
ylabel('log(normalized area)');

set(handles.text6, 'String', sprintf('Linear regression correlation = %f', corr));



% --- Outputs from this function are returned to the command line.
function varargout = FractalDimensionGlobal_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;

% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% saving new regression values
fid = fopen('../Config/config.txt');
old_sigma = fgetl(fid);
old_niter = fgetl(fid);
old_minLoc = fgetl(fid);
old_maxLoc = fgetl(fid);

fclose(fid);
fsave = fopen('../Config/config.txt', 'w');
fprintf(fsave, '%s \n', old_sigma);
fprintf(fsave, '%s \n', old_niter);
fprintf(fsave, '%s \n', old_minLoc);
fprintf(fsave, '%s \n', old_maxLoc);
fprintf(fsave, '%d \n', get(handles.slider2, 'Value'));
fprintf(fsave, '%d \n', get(handles.slider3, 'Value'));
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
    compute_global_shfd(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf'),int32(get(handles.slider2, 'Value')), int32(get(handles.slider3, 'Value')));
    
    fset = fopen(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_GLOBAL_SHFD_',int2str(get(handles.slider2, 'Value')),'_',int2str(get(handles.slider3, 'Value')),'.txt'));
    disp(strcat(filepath,'\',name,'_temp','\template_',name,'_OL_2O_1_0_des_orig.surf_GLOBAL_SHFD_',int2str(get(handles.slider2, 'Value')),'_',int2str(get(handles.slider3, 'Value')),'.txt'));
    A = fgetl(fset);
    
    for i=1:3
        A = fgets(fset);
        if (A~=-1)
            B = fscanf(fset, '%f');
        end
    end
    globalshfd = B(1:1);
    correlation = B(end:end);

    fclose(fset);
    
    set(g1data.GlobalSHFD, 'String', globalshfd);
    
end
    
axes(handles.axes1);

close

% --- Executes on slider movement.
function slider2_Callback(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
set(handles.text4, 'String', round(get(handles.slider2, 'Value')));
set(handles.slider2, 'Value', round(get(handles.slider2, 'Value')));

if ( get(handles.slider3, 'Value') <= get(handles.slider2, 'Value') )
    errordlg('Max L value must be greater than Min L value');
        set(handles.text4, 'String', round(get(handles.slider2, 'Value'))-1);
    set(handles.slider2, 'Value', round(get(handles.slider2, 'Value'))-1);
else
    
global rec;
global min_L;
global max_L;
global ini;
ini = get(handles.slider2, 'Value');
global fin;
fin = get(handles.slider3, 'Value');
global VAreas;
global r;
global n;

cla(handles.axes1);

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
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);
xlabel('log(l)');
ylabel('log(normalized area)');

set(handles.text6, 'String', sprintf('Linear regression correlation = %f', corr));
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
set(handles.text10, 'String', round(get(handles.slider3, 'Value')));
set(handles.slider3, 'Value', round(get(handles.slider3, 'Value')));

if ( get(handles.slider3, 'Value') <= get(handles.slider2, 'Value') )
    errordlg('Max L value must be greater than Min L value');
    set(handles.text10, 'String', round(get(handles.slider3, 'Value'))+1);
    set(handles.slider3, 'Value', round(get(handles.slider3, 'Value'))+1);
else
    
global rec;
global min_L;
global max_L;
global ini;
ini = get(handles.slider2, 'Value');
global fin;
fin = get(handles.slider3, 'Value');
global VAreas;
global r;
global n;

cla(handles.axes1);

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
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);
xlabel('log(l)');
ylabel('log(normalized area)');

set(handles.text6, 'String', sprintf('Linear regression correlation = %f', corr));
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

% --------------------------------------------------------------------
function uipushtool1_ClickedCallback(hObject, eventdata, handles)
% hObject    handle to uipushtool1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close


% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close