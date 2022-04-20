; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Val3 Tools"
#define MyAppVersion "4.2.8"
#define MyAppPublisher "SAXE Process AB"
#define MyAppURL "http://www.saxe-group.com"
#define QT "D:\Qt\5.15.2\msvc2019_64"
;#define MINGW "C:\MinGW"


[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{22A102F5-E18C-421B-8A0C-80C01031AAFA}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\Saxe Val3 Tools
DefaultGroupName={#MyAppName}
OutputDir=.
OutputBaseFilename={#MyAppName}-setup-{#MyAppVersion}
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\cs8ProgramComponent\bin\extractPrototype.exe"; DestDir: "{app}"; Flags: ignoreversion
;Source: "..\cs8ProgramComponent\bin\val3PreCompilerSettings.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\Val3 Documentation Editor.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\val3LinterControlPanel.exe"; DestDir: "{app}"; Flags: ignoreversion
;Source: "..\cs8ProgramComponent\bin\val3Check.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\val3CreateAPI.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\val3Compact.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\extractDoc.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\preCompilerPackage\*.exe"; DestDir: "{app}\preCompilerPackage"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Core.dll"; DestDir: "{app}\preCompilerPackage"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Xml.dll"; DestDir: "{app}\preCompilerPackage"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Gui.dll"; DestDir: "{app}\preCompilerPackage"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Positioning.dll"; DestDir: "{app}\preCompilerPackage"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5PrintSupport.dll"; DestDir: "{app}\preCompilerPackage"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\helpFilePackage\*.chm"; DestDir: "{app}\helpFilePackage"; Flags: ignoreversion
Source: "..\cs8ProgramComponent\bin\doxygenTemplateData\*.*"; DestDir: "{app}\doxygenTemplateData"; Flags: ignoreversion recursesubdirs

Source: "{#QT}\bin\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Xml.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5WebEngineWidgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5WebEngineCore.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5WebChannel.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Quick.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Positioning.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5PrintSupport.dll"; DestDir: "{app}"; Flags: ignoreversion

Source: "{#QT}\bin\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\libGLESV2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\libEGL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\D3Dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5QuickWidgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5Qml.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\Qt5QmlModels.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\bin\QtWebEngineProcess.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\resources\*"; DestDir: "{app}\resources\"; Flags: ignoreversion
Source: "{#QT}\translations\qtwebengine_locales\*"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT}\plugins\platforms\qwindows.dll"; DestDir: "{app}\plugins\platforms"; Flags: ignoreversion
Source: "{#QT}\plugins\styles\qwindowsvistastyle.dll"; DestDir: "{app}\plugins\styles\"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[InstallDelete]
Type: files; Name:  "{app}\preCompilerPackage\*.*";

[Icons]
Name: "{group}\Val3 Documentation Editor"; Filename: "{app}\Val3 Documentation Editor.exe"
Name: "{group}\Val3 Linter Deployment"; Filename: "{app}\val3LinterControlPanel.exe"
Name: "{group}\Val3 Create API"; Filename: "{app}\val3CreateAPI.exe"
;Name: "{group}\Val3 Compiler Settings"; Filename: "{app}\val3PreCompilerSettings.exe"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\val3LinterControlPanel.EXE"; Parameters: "--install"; Description: "Install SAXE Val3 Linter for SRC7.x and SRC8.x versions"; Flags: postinstall nowait shellexec


[UninstallRun]
Filename: "{app}\val3LinterControlPanel.EXE"; Parameters: "--uninstall --exitWhenReady";

