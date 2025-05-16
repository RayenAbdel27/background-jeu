CC = gcc
CFLAGS = -Wall -g `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer

SRC = main.c background.c
TARGET = game

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o

