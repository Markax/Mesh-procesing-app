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

% Last Modified by GUIDE v2.5 24-Jul-2018 22:41:08

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
% uiwait(handles.figure1);
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

%for i=1:str2double(max_L)
 %   n = [n, A(i:1)];
%end
% log-log plot of the box-counting
%n = [0.096593 0.110952 0.102548 0.100624 0.104658 0.105673 0.110766 0.119729 0.122741 0.138679 0.155067 0.160009 0.175161 0.173712 0.179591 0.187113 0.195919 0.203796 0.216895 0.227390 0.231673 0.236557 0.240494 0.243389 0.245827 0.250310 0.255133 0.260036 0.262843 0.266324 0.270710 0.273035 0.273974 0.276957 0.280406 0.283960 0.286744 0.291785 0.294298 0.296123 0.299394 0.301453 0.304474 0.306305 0.308668 0.310322 0.311554 0.313884 0.316318 0.317648 0.319276 0.320332 0.322380 0.323294 0.324709 0.325900 0.327397 0.328772 0.330166 0.331255];
%r = [1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60];





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

if ( get(handles.slider3, 'Value') <= get(handles.slider2, 'Value') )
    errordlg('Max L value must be greater than Min L value');
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

n = zeros(1, rec);
for i=1:rec
   n(1,i) = VValues(rec*(act_vert-1)+i,1); 
end

r = zeros(1, rec);
for i=min_L:max_L
    r(1,i) = i;
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

end

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


% --- Executes during object creation, after setting all properties.
function slider3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
