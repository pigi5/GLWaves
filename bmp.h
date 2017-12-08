/**
 * Source: https://www.daniweb.com/programming/software-development/threads/482083/reading-a-24-bit-bmp-image-file
 * Authors: triumphost
 *          Ford Hash
 * Date Modified: 12/7/17
 */

#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct bitmap
{
    unsigned int width, height;
    unsigned char *pixels;
};

void freeBmp(struct bitmap* bmp)
{
    if (bmp && bmp->pixels)
    {
        bmp->width = 0;
        bmp->height = 0;
        free(bmp->pixels);
        free(bmp);
        bmp = NULL;
    }
}

bool loadBmp(const char *filepath, struct bitmap*& bmp)
{
    bmp = NULL;
    FILE *f = fopen(filepath, "rb");
    if (f)
    {
        bmp = (bitmap*)malloc(sizeof(bitmap));
        bmp->width = 0;
        bmp->height = 0;
        bmp->pixels = NULL;
        unsigned char info[54] = {0};
        fread(info, sizeof(unsigned char), 54, f);
        bmp->width = *(unsigned int *)&info[18];
        bmp->height = *(unsigned int *)&info[22];
        unsigned int size = bmp->width * bmp->height * 3;
        bmp->pixels = (unsigned char*)malloc(size);
        fread(bmp->pixels, sizeof(unsigned char), size, f);
        fclose(f);
        for(int i = 0; i < size; i += 3)
        {
            unsigned char tmp = bmp->pixels[i];
            bmp->pixels[i] = bmp->pixels[i + 2];
            bmp->pixels[i + 2] = tmp;
        }
        return true;
    }
    return false;
}

#endif