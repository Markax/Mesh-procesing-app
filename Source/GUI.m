function varargout = GUI(varargin)
% GUI MATLAB code for GUI.fig
%      GUI, by itself, creates a new GUI or raises the existing
%      singleton*.
%
%      H = GUI returns the handle to a new GUI or the handle to
%      the existing singleton*.
%
%      GUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in GUI.M with the given input arguments.
%
%      GUI('Property','Value',...) creates a new GUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before GUI_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to GUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help GUI

% Last Modified by GUIDE v2.5 20-Jun-2018 17:09:09

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @GUI_OpeningFcn, ...
                   'gui_OutputFcn',  @GUI_OutputFcn, ...
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


% --- Executes just before GUI is made visible.
function GUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to GUI (see VARARGIN)

% Choose default command line output for GUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes GUI wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = GUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



% --------------------------------------------------------------------
function uipushtool1_ClickedCallback(hObject, eventdata, handles)
% hObject    handle to uipushtool1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

[file,path] = uigetfile('../Models/*.*');
[filepath,name,ext] = fileparts(file);
            if isequal(file,0)
               disp('User selected Cancel');
            else
               global fpath;
               fpath = fullfile(path,file);
               disp(['User selected ', fullfile(path,file)]);
            end
            if (isequal(ext, '.obj'))
                obj = readObj(fullfile(path,file));
                set(handles.uipushtool2,'enable','on');
                set(handles.uitoggletool1,'enable','on');
                set(handles.uitoggletool3,'enable','on');
                set(handles.uitoggletool4,'enable','on');
                set(handles.MFileButton,'enable','on');
                trisurf(obj.f , obj.v(:,1), obj.v(:,2), obj.v(:,3),'FaceColor',[0.26,0.33,1.0 ]);
                shading interp
                colormap gray(256);
                lighting phong;
                %light('Position',[-1 0 0],'Style','local');
                camproj('perspective');
                axis square; 
                axis off;
                axis equal;
                axis tight;  
                camlight('headlight')
                cameratoolbar;
                zoom on;
            else
                [vertex_coords, faces] = read_freesurfer_surf(fullfile(path,file));
                set(handles.uipushtool2,'enable','on');
                set(handles.uitoggletool1,'enable','on');
                set(handles.uitoggletool3,'enable','on');
                set(handles.uitoggletool4,'enable','on');
                set(handles.MFileButton,'enable','on');
                trisurf(faces, vertex_coords(:,1), vertex_coords(:,2), vertex_coords(:,3),'FaceColor',[0.26,0.33,1.0 ]);
                shading interp
                colormap gray(256);
                lighting phong;
                %light('Position',[-1 0 0],'Style','local');
                camproj('perspective');
                axis square; 
                axis off;
                axis equal;
                axis tight;
                camlight('headlight');
                cameratoolbar;
                zoom on;
            end
           


% --------------------------------------------------------------------
function uitoggletool4_ClickedCallback(hObject, eventdata, handles)
% hObject    handle to uitoggletool4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
rotate3d on;


% --------------------------------------------------------------------
function uipushtool2_ClickedCallback(hObject, eventdata, handles)
% hObject    handle to uipushtool2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
delete(findall(gcf,'Type','light'));
camlight('headlight');


% --- Executes on button press in MFileButton.
function MFileButton_Callback(hObject, eventdata, handles)
% hObject    handle to MFileButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global fpath;
generate_m(fpath);