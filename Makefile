OUTPUT = client server
CFLAGS = -g -Wall -Wvla -I inc -D_REENTRANT -pthread
LFLAGS = -L lib -lSDL2 -lSDL2_image -lSDL2_ttf

%.o: %.c %.h
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

all: $(OUTPUT)

runclient: $(OUTPUT)
	LD_LIBRARY_PATH=lib ./client localhost 9012
server: server.o
	gcc -pthread csapp.c $(CFLAGS) -o $@ $^ $(LFLAGS)

client: client.o csapp.o
	gcc $(CFLAGS) -o $@ $^ $(LFLAGS)

clean:
	rm -f $(OUTPUT) *.o
