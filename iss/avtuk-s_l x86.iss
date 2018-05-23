; -- avtuks-L x86.iss --

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#define Name "�����-������ ��"
#define GroupName "�����-������"
#define EngName "AVTUK-S"
#define Version "2.1.233"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "avtuks-L.exe"
#define SetupName "avtuks-L-2.1.233-x86"
#define Prefix "D:\Progs\out"

[CustomMessages]
Version={#Version}

[Languages]
Name: Russian; MessagesFile: "compiler:Languages\Russian.isl" 

[InstallDelete]
Type: files; Name: "{app}\*.dll"
Type: filesandordirs; Name: "{app}\platforms"
Type: files; Name: "{app}\{#ExeName}"
; Type: files; Name: "{app}\���� ����������� ������������.pdf"

[Setup]
UsePreviousLanguage=No
AppName={#Name}
AppVersion={#Version}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
DefaultDirName={pf}\{#EngName}
DefaultGroupName={#GroupName}
; UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir="{#Prefix}\out\"
LicenseFile="{#Prefix}\coma\license.txt"

OutputBaseFilename={#SetupName}

[Dirs]
Name: {userappdata}\{#EngName}

[Files]
Source: "{#Prefix}\coma\*.dll"; DestDir: "{app}"
Source: "{#Prefix}\coma\platforms\qwindows.dll"; DestDir: "{app}\platforms"
Source: "{#Prefix}\coma\{#ExeName}"; DestDir: "{app}"; DestName: {#ExeName}; Flags: ignoreversion
Source: "{#Prefix}\coma\ermsgs.dat"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\src\reports\*.*"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\images\coma\*.*"; DestDir: "{app}\images"; Flags: ignoreversion
Source: "{#Prefix}\src\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
; Source: "coma\pdf\���� ����������� ������������.pdf"; DestDir: "{app}"

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\����������� ������������ ����"; Filename: "{app}\���� ����������� ������������.pdf"
Name: "{group}\������� ��������� {#Name}"; Filename: "{uninstallexe}"

[Run]
; add the Parameters, WorkingDir and StatusMsg as you wish, just keep here
; the conditional installation Check
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; Check: not IsRequiredVC2015Detected; StatusMsg: ��������������� ����� MSVC2015 Redistributable...

[Code]
//-----------------------------------------------------------------------------
//  �������� ������� ������ ������ VC2015
//  https://habrahabr.ru/post/255807/
//-----------------------------------------------------------------------------
function IsVC2015Detected(): boolean;

var 
    reg_key: string; // ��������������� ��������� ���������� �������
    success: boolean; // ���� ������� ������������� ������ VC
    success2: boolean; // ��������� ����
    key_value: string; // ����������� �� ������� �������� �����

begin
    success := false;
    reg_key := 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64';
    success := RegQueryStringValue(HKLM, reg_key, 'Version', key_value);
    success := success and ((Pos('v14.0.24215', key_value) = 1) or (Pos('v14.0.24210', key_value) = 1));
    reg_key := 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86';
    success2 := RegQueryStringValue(HKLM, reg_key, 'Version', key_value);
    success2 := success2 and ((Pos('v14.0.24215', key_value) = 1) or (Pos('v14.0.24210', key_value) = 1));
    success := success or success2;

    result := success;
end;

//-----------------------------------------------------------------------------
//  �������-������� ��� �������������� ���������� ������ ��� ������
//-----------------------------------------------------------------------------
function IsRequiredVC2015Detected(): boolean;
begin
    result := IsVC2015Detected();
end;

//-----------------------------------------------------------------------------
//    Callback-�������, ���������� ��� ������������� ���������
//-----------------------------------------------------------------------------
function InitializeSetup(): boolean;
begin

  // ���� ��� ��������� ������ .NET ������� ��������� � ���, ��� �����������
  // ���������� ���������� � �� ������ ���������
//  if not IsVC2015Detected() then
//    begin
//      MsgBox('{#Name} ������� ��������� Microsoft VC2015 redistributable v14.0.24215'#13#13
//             '���������� �������� ��������� MSVC ����� ��������� ��������� ������', mbInformation, MB_OK);
//    end;   

  result := true;
end;