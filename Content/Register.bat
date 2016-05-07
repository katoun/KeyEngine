set KEY_ENGINE_SDK_PATH=%~dp0..
setx KEY_ENGINE_SDK_PATH %~dp0..

REG DELETE HKEY_CLASSES_ROOT\.keyproject /f
REG DELETE HKEY_CLASSES_ROOT\KeyEngine.ProjectFile /f

REG ADD HKEY_CLASSES_ROOT\.keyproject /d "KeyEngine.ProjectFile" /f
REG ADD HKEY_CLASSES_ROOT\KeyEngine.ProjectFile /f
REG ADD HKEY_CLASSES_ROOT\KeyEngine.ProjectFile\DefaultIcon /d "\"%~dp0..\Content\Icon.ico\"" /f
REG ADD HKEY_CLASSES_ROOT\KeyEngine.ProjectFile\shell /f
REG ADD HKEY_CLASSES_ROOT\KeyEngine.ProjectFile\shell\open /d "Open" /f
REG ADD HKEY_CLASSES_ROOT\KeyEngine.ProjectFile\shell\open\command /d "\"%~dp0..\Binaries\Win64\KeyEditor_d.exe\" -o \"%%1%%\"" /f