@echo off
echo [INFO] Kompilacja VoIP.exe przy użyciu MinGW-w64...

REM Ustaw ścieżki
set "ROOT=%~dp0"
set "SRC_PATH=%ROOT%src\src"
set "MAIN_PATH=%ROOT%src"
set "INC_PATH=%ROOT%src\include"
set "GST_PATH=%ROOT%gstreamer"
set "MINGW_PATH=%ROOT%mingw64\bin"

REM Uruchom kompilację
"%MINGW_PATH%\gcc.exe" "%MAIN_PATH%\main.c" "%SRC_PATH%\*.c" -o "%ROOT%VoIP.exe" ^
   -I "%INC_PATH%" ^
   -I "%GST_PATH%\include\gstreamer-1.0" ^
   -I "%GST_PATH%\include\glib-2.0" ^
   -I "%GST_PATH%\lib\glib-2.0\include" ^
   -L "%GST_PATH%\lib" ^
   -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0

echo [DONE] Kompilacja zakonczona.
pause
