@echo off
echo ======================================
echo   VoIP.exe - uruchamianie
echo ======================================

if not exist "%SystemRoot%\System32\vcruntime140.dll" (
    echo Instalowanie Microsoft Visual C++ Redistributable...
    start /wait vc_redist.x64.exe /install /quiet /norestart
)

set "GST_PLUGIN_PATH=%~dp0gstreamer\lib\gstreamer-1.0"
set "PATH=%~dp0gstreamer\bin;%PATH%"

ECHO Uruchamianie aplikacji VoIP...
ECHO.

VoIP.exe >> log.txt 2>&1

if %ERRORLEVEL% NEQ 0 (
    echo [!] Program zakonczyl sie bledem. Sprawdz plik log.txt
) else (
    echo [OK] Program zakonczyl sie pomyslnie.
)

ECHO.
ECHO Aplikacja zakonczyla dzialanie.
pause