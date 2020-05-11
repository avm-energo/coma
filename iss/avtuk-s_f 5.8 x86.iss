; -- avtuk-s_f x86.iss --

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#define Name "АВМ-Сервис"
#define GroupName "АВМ-Сервис"
#define EngName "АВМ-Сервис"
#define Version "0.0.20"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Service.exe"
#define SetupName "AVM-Service-0.0.20-x86"
#define Prefix "D:\out-5.8_x32"

[CustomMessages]
Version={#Version}

[Languages]
Name: Russian; MessagesFile: "compiler:Languages\Russian.isl" 

[InstallDelete]
Type: files; Name: "{app}\*.dll"
Type: filesandordirs; Name: "{app}\platforms"
Type: files; Name: "{app}\{#ExeName}"
; Type: files; Name: "{app}\КОМА Руководство пользователя.pdf"

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
Source: "{#Prefix}\coma\ermsgs.dat"; DestDir: "{localappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\reports\*.*"; DestDir: "{localappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\images\*.*"; DestDir: "{app}\images"; Flags: ignoreversion
Source: "{#Prefix}\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
Source: "{#Prefix}\coma\readme.txt"; DestDir: "{app}"

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\Руководство пользователя КОМА"; Filename: "{app}\КОМА Руководство пользователя.pdf"
Name: "{group}\Удалить программу {#Name}"; Filename: "{uninstallexe}"

[Run]
; add the Parameters, WorkingDir and StatusMsg as you wish, just keep here
; the conditional installation Check
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; Check: not IsRequiredVC2015Detected; StatusMsg: Устанавливается пакет MSVC2015 Redistributable...

[Code]
//-----------------------------------------------------------------------------
//  Проверка наличия нужной версии VC2015
//  https://habrahabr.ru/post/255807/
//-----------------------------------------------------------------------------
function IsVC2015Detected(): boolean;

var 
    reg_key: string; // Просматриваемый подраздел системного реестра
    success: boolean; // Флаг наличия запрашиваемой версии VC
    key_value: string; // Прочитанное из реестра значение ключа

begin
    success := false;
    reg_key := 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86';
    
    success := RegQueryStringValue(HKLM, reg_key, 'Version', key_value);
    success := success and (Pos('v14.0.24215', key_value) = 1);
    result := success;
end;

//-----------------------------------------------------------------------------
//  Функция-обертка для детектирования конкретной нужной нам версии
//-----------------------------------------------------------------------------
function IsRequiredVC2015Detected(): boolean;
begin
    result := IsVC2015Detected();
end;

//-----------------------------------------------------------------------------
//    Callback-функция, вызываемая при инициализации установки
//-----------------------------------------------------------------------------
function InitializeSetup(): boolean;
begin

  // Если нет тербуемой версии .NET выводим сообщение о том, что инсталлятор
  // попытается установить её на данный компьютер
//  if not IsVC2015Detected() then
//    begin
//      MsgBox('{#Name} требует установки Microsoft VC2015 redistributable v14.0.24215'#13#13
//             'Установщик запустит установку MSVC после установки основного пакета', mbInformation, MB_OK);
//    end;   

  result := true;
end;