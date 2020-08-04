call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\vsdevcmd\ext\vcvars.bat"
start /wait MSBuild %CD%\xz-utilz\windows\vs2017\liblzma_dll.vcxproj /property:Configuration=Release;Platform=win32;OutDir=..\..\..\..\libs\win32\release
start /wait MSBuild %CD%\hidapi\windows\hidapi.vcxproj /property:Configuration=Release;Platform=win32;OutDir=..\..\..\libs\win32\release
cd LimeReport && start "" "C:\Qt\5.14.2\msvc2017\bin\qmake.exe" -recursive
start /wait nmake sub-3rdparty sub-limereport 
cd ..
xcopy LimeReport\build\5.14.2\win32\release\lib\*.* ..\libs\win32\release
xcopy LimeReport\build\5.14.2\win32\release\lib\include ..\includes\limereport
cd QtXlsxWriter && start "" "C:\Qt\5.14.2\msvc2017\bin\qmake.exe" -recursive
start /wait nmake sub-src
xcopy lib\Qt5Xlsx.*  ..\..\libs\win32\release
cd ..

