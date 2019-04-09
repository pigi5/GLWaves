/**
 * Author: Ford Hash
 * Last Modified: 12/7/2017
 * I used Keith Lantz's example skybox texture
 * https://www.keithlantz.net/wp-content/uploads/2011/10/skybox_texture-300x225.jpg
 */

#ifndef SKYBOX_H
#define SKYBOX_H

#include "GL/glut.h"
#include "bmp.h"
#include <iostream>

class Skybox
{
private:
    struct bitmap *topBMP, *frontBMP, *backBMP, *leftBMP, *rightBMP;
    GLuint top, front, back, left, right;
    int width, height, length, heightOffset;

    /*	===============================================
    Desc:	Loads a new array of colors of a specified dimension
    	    into our object. (FROM LAB 7)
    Precondition:
    Postcondition:
    =============================================== */
    GLuint loadTexture(unsigned int width, unsigned int height, unsigned char* pixels)
    {
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     width,
                     height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     pixels);
        return textureId;
    }
public:
    Skybox(int width, int height, int length)
    {
        this->width = width;
        this->height = height;
        this->length = length;
        heightOffset = height / 4;

        if (loadBmp("top.bmp", topBMP))
        {
            top = loadTexture(topBMP->width, topBMP->height, topBMP->pixels);
            std::cout << "Loaded top.bmp as " << top << std::endl;
        }
        else
        {
            std::cout << "Failed to load top.bmp" << std::endl;
        }

        if (loadBmp("front.bmp", frontBMP))
        {
            front = loadTexture(frontBMP->width, frontBMP->height, frontBMP->pixels);
            std::cout << "Loaded front.bmp as " << front << std::endl;
        }
        else
        {
            std::cout << "Failed to load front.bmp" << std::endl;
        }

        if (loadBmp("back.bmp", backBMP))
        {
            back = loadTexture(backBMP->width, backBMP->height, backBMP->pixels);
            std::cout << "Loaded back.bmp as " << back << std::endl;
        }
        else
        {
            std::cout << "Failed to load back.bmp" << std::endl;
        }

        if (loadBmp("left.bmp", leftBMP))
        {
            left = loadTexture(leftBMP->width, leftBMP->height, leftBMP->pixels);
            std::cout << "Loaded left.bmp as " << left << std::endl;
        }
        else
        {
            std::cout << "Failed to load left.bmp" << std::endl;
        }

        if (loadBmp("right.bmp", rightBMP))
        {
            right = loadTexture(rightBMP->width, rightBMP->height, rightBMP->pixels);
            std::cout << "Loaded right.bmp as " << right << std::endl;
        }
        else
        {
            std::cout << "Failed to load right.bmp" << std::endl;
        }
    };
    ~Skybox()
    {
        freeBmp(topBMP);
        freeBmp(frontBMP);
        freeBmp(backBMP);
        freeBmp(leftBMP);
        freeBmp(rightBMP);
    };

    void draw()
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glDisable(GL_LIGHTING);

        glBindTexture(GL_TEXTURE_2D, top);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(0, height - heightOffset, 0);
        glTexCoord2f(1, 0);
        glVertex3f(width, height - heightOffset, 0);
        glTexCoord2f(1, 1);
        glVertex3f(width, height - heightOffset, length);
        glTexCoord2f(0, 1);
        glVertex3f(0, height - heightOffset, length);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, front);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(0, -heightOffset, 0);
        glTexCoord2f(1, 0);
        glVertex3f(width, -heightOffset, 0);
        glTexCoord2f(1, 1);
        glVertex3f(width, height - heightOffset, 0);
        glTexCoord2f(0, 1);
        glVertex3f(0, height - heightOffset, 0);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, back);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(width, -heightOffset, length);
        glTexCoord2f(1, 0);
        glVertex3f(0, -heightOffset, length);
        glTexCoord2f(1, 1);
        glVertex3f(0, height - heightOffset, length);
        glTexCoord2f(0, 1);
        glVertex3f(width, height - heightOffset, length);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, left);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(0, -heightOffset, length);
        glTexCoord2f(1, 0);
        glVertex3f(0, -heightOffset, 0);
        glTexCoord2f(1, 1);
        glVertex3f(0, height - heightOffset, 0);
        glTexCoord2f(0, 1);
        glVertex3f(0, height - heightOffset, length);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, right);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(width, -heightOffset, 0);
        glTexCoord2f(1, 0);
        glVertex3f(width, -heightOffset, length);
        glTexCoord2f(1, 1);
        glVertex3f(width, height - heightOffset, length);
        glTexCoord2f(0, 1);
        glVertex3f(width, height - heightOffset, 0);
        glEnd();

        glEnable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
    }
};

#endif
