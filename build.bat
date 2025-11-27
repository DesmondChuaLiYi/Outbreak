@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "%~dp0"
msbuild ProgrammingProject.sln /p:Configuration=Debug /p:Platform=x64 /t:Rebuild /m /v:minimal
