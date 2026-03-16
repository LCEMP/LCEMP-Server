@echo off
set "ACTION=%~1"
if "%ACTION%"=="" set "ACTION=Release"
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
for /f "usebackq delims=" %%P in (`wsl wslpath "%SCRIPT_DIR%"`) do set "WSL_DIR=%%P"
wsl bash "%WSL_DIR%/build.sh" "%ACTION%"
exit /b %ERRORLEVEL%
