CC=gcc
CFLAGS=`pkg-config --cflags gstreamer-1.0 gstreamer-rtsp-server-1.0`
LIBS=`pkg-config --libs gstreamer-1.0 gstreamer-rtsp-server-1.0`
TARGET=voip_rpi

all: $(TARGET)

$(TARGET): main.o
	$(CC) -o $(TARGET) main.o $(LIBS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

clean:
	rm -f $(TARGET) *.o
