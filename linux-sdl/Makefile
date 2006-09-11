INCLUDE = -I/usr/X11R6/include -I/usr/include/SDL
LIBDIR  = -L/usr/lib -L/usr/X11R6/lib -L.

COMPILERFLAGS = -Wall -g -funroll-loops -fomit-frame-pointer -ffast-math
CC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -rdynamic -ldl -lXext -lX11 -lm -lpthread -lSDL -lXmu -lXext -lXi -lSDL_image

OBJS = dungeon.o data.o draw.o actions.o
TDMC_OBJS = $(OBJS) testmain.o
DMC_OBJS = $(OBJS) main.o

all : dmc tdmc
dmc : $(DMC_OBJS)
	$(CC) $(CFLAGS) -o dmc $(LIBDIR) $(DMC_OBJS) $(LIBRARIES)
tdmc : $(TDMC_OBJS)
	$(CC) $(CFLAGS) -o tdmc $(LIBDIR) $(TDMC_OBJS) $(LIBRARIES)
tgfx : graphics.o
	$(CC) $(CFLAGS) -o tgfx $(LIBDIR) graphics.o $(LIBRARIES)
clean :
	rm *.o
	rm dmc

