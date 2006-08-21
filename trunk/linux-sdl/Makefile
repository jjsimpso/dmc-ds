INCLUDE = -I/usr/X11R6/include -I/usr/include/SDL
LIBDIR  = -L/usr/lib -L/usr/X11R6/lib -L.

COMPILERFLAGS = -Wall -g -funroll-loops -fomit-frame-pointer -ffast-math
CC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -rdynamic -ldl -lXext -lX11 -lm -lpthread -lSDL -lXmu -lXext -lXi -lSDL_image

OBJS = dungeon.o data.o draw.o actions.o main.o
TESTOBJS = dungeon.o data.o draw.o actions.o testmain.o

all : dmc tdmc
dmc : $(OBJS)
	$(CC) $(CFLAGS) -o dmc $(LIBDIR) $(OBJS) $(LIBRARIES)
tdmc : $(TESTOBJS)
	$(CC) $(CFLAGS) -o tdmc $(LIBDIR) $(TESTOBJS) $(LIBRARIES)
tgfx : graphics.o
	$(CC) $(CFLAGS) -o tgfx $(LIBDIR) graphics.o $(LIBRARIES)
clean :
	rm *.o
	rm dmc
