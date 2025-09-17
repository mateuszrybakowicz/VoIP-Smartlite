CC=gcc
TARGET=voip_rpi
SRC=VoIP-rpi.c

OBJ=$(SRC:.c=.o)
CFLAGS=`pkg-config --cflags gstreamer-1.0 gstreamer-rtsp-server-1.0`
LIBS=`pkg-config --libs gstreamer-1.0 gstreamer-rtsp-server-1.0`
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
clean:
	rm -f $(TARGET) *.o
