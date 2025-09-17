1. sudo apt update
2. sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
3. sudo apt-get install libgstrtspserver-1.0-dev  
   lub jak nie zadziala to sudo apt-get install libgstreamer-rtsp-server-1.0-dev
5. git clone https://github.com/mateuszrybakowicz/VoIP-Smartlite.git
6. cd VoIP-Smartlite/rpi
7. sudo nano config.ini  
   należy wpisac poprawne adresy IP
9. make
10.  GST_DEBUG=3,timing:5 ./voip_rpi  
\
aplikacja przetestowana z uzyciem behringera UMC202HD podlaczonym przez usb
