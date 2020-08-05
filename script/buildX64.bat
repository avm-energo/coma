call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\vsdevcmd\ext\vcvars.bat"
call MSBuild "%cd%\xz-utilz\windows\vs2017\liblzma_dll.vcxproj" /property:Configuration=Release;Platform=x64;OutDir=..\..\..\..\libs\win64\release
call MSBuild "%cd%\hidapi\windows\hidapi.vcxproj" /property:Configuration=Release;Platform=x64;OutDir=..\..\..\libs\win64\release
cd LimeReport && start /wait "" nmake clean
start /wait "" "C:\Qt\5.14.2\msvc2017_64\bin\qmake.exe" -recursive
start /wait "" nmake sub-3rdparty sub-limereport 
cd ..
xcopy LimeReport\build\5.14.2\win64\release\lib\*.* ..\libs\win64\release /H /Y /C /R /S /I
xcopy LimeReport\build\5.14.2\win64\release\lib\include ..\includes\limereport /I /Y /S
cd QtXlsxWriter && start /wait "" nmake clean
start /wait "" "C:\Qt\5.14.2\msvc2017_64\bin\qmake.exe" -recursive
start /wait "" nmake sub-src
xcopy lib\Qt5Xlsx.*  ..\..\libs\win64\release /H /Y /C /R /S /I
cd ..