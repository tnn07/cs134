#ifndef DRAWUTILS_H

#include<GL/glew.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#define DRAWUTILS_H

#ifdef __cplusplus
extern "C" {
#endif
//GLuint ~ unsigned int
GLuint glTexImageTGAFile(const char* filename, int* outWidth, int* outHeight);
void glDrawSprite(GLuint tex, int x, int y, int w, int h);

#ifdef __cplusplus
}
#endif
    
#endif
