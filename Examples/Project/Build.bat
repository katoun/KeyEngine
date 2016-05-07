set MSBUILD_EXE="C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe"

rmdir /s /q Binaries
mkdir Binaries

rmdir /s /q Build
mkdir Build

%MSBUILD_EXE% Project.sln /target:Project /p:Platform=Win64;Configuration="Debug Editor"
%MSBUILD_EXE% Project.sln /target:Project /p:Platform=Win64;Configuration="Debug Game"