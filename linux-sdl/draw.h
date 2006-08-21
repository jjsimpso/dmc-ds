#ifndef DRAW_H_DEFINED
#define DRAW_H_DEFINED

/* Functions Defs */
void drawTitle(SDL_Surface *surf);
void drawTest(int x, int y, Uint8 *cell, SDL_Surface *surf);
void drawView(int x, int y, int facing, SDL_Surface *surf);

#endif
