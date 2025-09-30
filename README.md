# VoIP-Smartlite
instrukcje jak zainstalowac aplikacje na rpi, znajduja sie  w README.md w folderze rpi  
\
\
main.c pokazuje caly kod, ale moze byc nieaktualny. Najaktualniejsze pliki na PC znajduja sie w branchu master. Jeśli nie ma potrzeby zmiany kodu, lub go kompilowania wystarczy pobrac 
najnowszy release na PC. Z moich doswiadczen wynika ze pobieranie moze sie wywalic i dziala z niskimi predkosciami, pobranie poprzez komende zalagodzilo sprawe, nalezy poprawic link tak aby w nazwie byl aktualny release  
curl -L -O https://github.com/mateuszrybakowicz/VoIP-Smartlite/releases/download/v1.0.4/VoIP_1_0_4.zip  
\
Rozpakowac go, zaktualizowac plik config.ini (ustawic poprawne adresy IP). Na koniec wystarczy w zmiennych srodowiskowych dodac do PATH 
...\dist\gstreamer\bin.  
\
aby wlaczyc aplikacje, nalezy uruchumic run_VoIP.bat i zezwolic na wszystko co tam chca. Przy pierwszym uruchomieniu aplikacja może wymagać zaakceptowania programu VS do C++, należy sie zgodzić. Pierwsze uruchomienie może zajac kilka minut nim zacznie dzialac, radze kontrolowac plik z logami.  
\
dodawanie aplikacji do autostartu windows 11:
-windows+R
-wpisujemy shell:startup
-przenosimy do otwartego folderu SKROT do pliku run_VoIp.bat
-warto sie upewnic w menedzerze zadan czy na pewno jest wlaczony autostart na tej aplikacji. 
\
aplikacja przetestowana z wpietym headsetem poprzez SoundBlastera usb
