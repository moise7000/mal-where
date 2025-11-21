@echo off


if "%1"=="" (
    echo [Error] Missing argument
    pause
    exit /b 1
)


set ARG=%1

if not "!ARG:~8,1!"=="" (
    echo.
    powershell -Command "Write-Host '[ERREUR]' -ForegroundColor Red -NoNewline; Write-Host ' Bad argument length'"
    echo.
    pause
    exit /b 1
)

powershell -Command "Write-Host '[OK]' -ForegroundColor Green -NoNewline; Write-Host ' Valid argument'"
echo.
echo Launching main.exe "%ARG%" ...
echo.

"C:\chemin\vers\votre\main.exe" "%ARG%"