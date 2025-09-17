1. sudo apt update
2. sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
3. git clone https://github.com/mateuszrybakowicz/VoIP-Smartlite.git
4. cd VoIP-Smartlite/rpi
5. make
6.  GST_DEBUG=3,timing:5 ./VoIP-rpi
