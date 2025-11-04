# VoIP-Smartlite
instrukcje jak zainstalowac aplikacje na rpi, znajduja sie  w README.md w folderze rpi  
\
\
Na PC nie ma potrzeby pobierania calego repozytorium. Nalezy pobrac
najnowszy release na PC. Z moich doswiadczen wynika ze pobieranie moze sie wywalic i dziala z niskimi predkosciami, pobranie poprzez komende zalagodzilo sprawe, nalezy poprawic link tak aby w nazwie byl aktualny release  
curl -L -O https://github.com/mateuszrybakowicz/VoIP-Smartlite/releases/download/v1.0.4/VoIP_1_0_4.zip  
\
Rozpakowac go, zaktualizowac plik config.ini, który znajduje sie folderze src (ustawic poprawne adresy IP). Na koniec wystarczy w zmiennych srodowiskowych dodac do PATH 
...\dist\gstreamer\bin.  
\
aby wlaczyc aplikacje, nalezy uruchumic run_VoIP.bat i zezwolic na wszystko co tam chca. Pierwsze uruchomienie może zajac kilka minut nim zacznie dzialac, radze kontrolowac plik z logami.  
\
dodawanie aplikacji do autostartu windows 11:  
-windows+R  
-wpisujemy shell:startup  
-przenosimy do otwartego folderu SKROT do pliku run_VoIp.bat  
-warto sie upewnic w menedzerze zadan czy na pewno jest wlaczony autostart na tej aplikacji. 
\
aby skompilowac i zaktualizowac aplikacje, uruchomic plik build_VoIP.bat, skompiluje sie kod znajdujacy sie w folderze src. Plik .exe sam sie podmieni na nowy
