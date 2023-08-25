#ifndef __SCREEN_H
#define __SCREEN_H

#include <stddef.h>
#include <stdio.h>
#define SCREEN_WIDTH  30
#define SCREEN_HEIGHT 30

typedef struct {
    int video_buffer[SCREEN_HEIGHT * SCREEN_WIDTH];
} Screen;

void render_screen(Screen* screen) {
    putchar('+');
    putchar(' ');
    for (size_t i = 0; i < SCREEN_WIDTH; ++i) {
        putchar('-');
        if (i + 1 < SCREEN_WIDTH) {
            putchar(' ');
        }
    }
    putchar(' ');
    putchar('+');
    putchar('\n');

    for (size_t y = 0; y < SCREEN_HEIGHT; ++y) {
        for (size_t x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == 0) {
                printf("| ");
            }

            char c = screen->video_buffer[y * SCREEN_HEIGHT + x];

            if (!c) {
                putchar(' ');
            } else {
                putchar(c);
            }

            putchar(' ');

            if (x + 1 == SCREEN_WIDTH) {
                printf("|");
            }
        }
        printf("\n");
    } 

    putchar('+');
    putchar(' ');
    for (size_t i = 0; i < SCREEN_WIDTH; ++i) {
        putchar('-');
        if (i + 1 < SCREEN_WIDTH) {
            putchar(' ');
        }
    }
    putchar(' ');
    putchar('+');
    putchar('\n');
}

void screen_clear(Screen* s) {
    for (size_t i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; ++i) {
        s->video_buffer[i] = 0;
    }
}

#endif //__SCREEN_H
