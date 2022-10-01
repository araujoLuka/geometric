CFLAGS = -Wall -g 
LIBS = -lallegro `pkg-config --libs $(AL_LIBS)` -lm
AL_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5
PROGRAM = geometric
SOURCES=$(wildcard sources/*.c)
OBJECTS=$(patsubst sources/%.c, bin/%.o, $(SOURCES))

all: $(PROGRAM)

play: all
	./$(PROGRAM)

$(PROGRAM): main.o $(OBJECTS)
	gcc $(CFLAGS) main.o $(OBJECTS) -o $(PROGRAM) $(LIBS)

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o $(LIBS)

tar: all
	tar -czf ../ballz.tar.gz ../ballz/

clean:
	-rm -f *.o *~f

purge: clean
	-rm -f $(PROGRAM)