CC = gcc
LDFLAGS = -lSDL2

audio:	
	 $(CC) -o test test.c $(LDFLAGS) -Wno-deprecated-declarations
