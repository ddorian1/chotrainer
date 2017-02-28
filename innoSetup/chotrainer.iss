[Setup]
AppName=Chotrainer
AppVersion=0.1.0
DefaultDirName={pf}\Chotrainer
DefaultGroupName=Chotrainer
SetupIconFile=..\windowsicon\icon.ico
OutputBaseFilename=ChotrainerSetup-0.1.0
ChangesAssociations=yes

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"

[Types]
Name: "normal"; Description: "Chotrainer"
Name: "extended"; Description: "Chotrainer/Creator"

[Components]
Name: "Chotrainer"; Description: "Chotrainer"; Types: normal extended
Name: "Creator"; Description: "Chotrainer Creator"; Types: extended

[Files]
Source: "files\Qt5Core.dll"; DestDir: "{app}"
Source: "files\Qt5Gui.dll"; DestDir: "{app}"
Source: "files\Qt5Svg.dll"; DestDir: "{app}"
Source: "files\Qt5Widgets.dll"; DestDir: "{app}"
Source: "files\chotrainer.exe"; DestDir: "{app}"
Source: "files\chotrainerCreator.exe"; DestDir: "{app}"; Components: Creator
Source: "files\core.dll"; DestDir: "{app}"
Source: "files\libbz2.dll"; DestDir: "{app}"
Source: "files\libfluidsynth.dll"; DestDir: "{app}"
Source: "files\libfreetype-6.dll"; DestDir: "{app}"
Source: "files\libgcc_s_sjlj-1.dll"; DestDir: "{app}"
Source: "files\libglib-2.0-0.dll"; DestDir: "{app}"
Source: "files\libharfbuzz-0.dll"; DestDir: "{app}"
Source: "files\libiconv-2.dll"; DestDir: "{app}"
Source: "files\libintl-8.dll"; DestDir: "{app}"
Source: "files\libpcre-1.dll"; DestDir: "{app}"
Source: "files\libpcre16-0.dll"; DestDir: "{app}"
Source: "files\libpng16-16.dll"; DestDir: "{app}"
Source: "files\libstdc++-6.dll"; DestDir: "{app}"
Source: "files\zlib1.dll"; DestDir: "{app}"
Source: "files\iconengines\*"; DestDir: "{app}\iconengines"
Source: "files\platforms\*"; DestDir: "{app}\platforms"

[Icons]
Name: "{group}\Chotrainer"; Filename: "{app}\chotrainer.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"; WorkingDir: "{app}"
Name: "{group}\Chotrainer Creator"; Filename: "{app}\chotrainerCreator.exe"; WorkingDir: "{app}"; Components: Creator

[Registry]
Root: HKCR; Subkey: ".chotrainer"; ValueData: "Chotrainer"; Flags: uninsdeletevalue; ValueType: string;
Root: HKCR; Subkey: "Chotrainer"; ValueData: "Program Chotrainer"; Flags: uninsdeletekey; ValueType: string;
Root: HKCR; Subkey: "Chotrainer\DefaultIcon"; ValueData: "{app}\chotrainer.exe,0"; ValueType: string;
Root: HKCR; Subkey: "Chotrainer\shell\open\command"; ValueData: """{app}\chotrainer.exe"" ""%1"""; ValueType: string;
