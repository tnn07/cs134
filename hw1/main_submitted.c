#include <stdio.h>
#include<GL\glew.h>
#include<SDL.h>

#include<stdlib.h>
#include<string.h>
#include<assert.h>
#undef main // http://stackoverflow.com/questions/24620143/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc

char shouldExit = 0;
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const char* WINDOW_TITLE = "CS 134 Homework #1 Thuy Nguyen";

int numKeys;
/*
* 6. The previous frame's keyboard state - to handle input
*/
unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 };

/*
* 7. The current frame's keyboard state - to handle input
*/
const unsigned char* kbState = NULL;

/*
* 11. Art to draw
*/
GLuint sonicTex;
GLuint marioTex;
//#######################

int main(void)
{
    printf("CS 134 HW01 Thuy Nguyen 6519");
    int imgX = WINDOW_WIDTH / 2;
    int imgY = WINDOW_HEIGHT / 2;
    int imgWidth = 64;
    int imgHeight = 64;

    //movement
    int deltaX = 64;
    int deltaY = 64;
    /*
        1. Initialize SDL
    */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Unable to initialize graphics library!\n");
        return 1;
    }

    /*
    * 2. Create the window, OpenGL context
    */
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_OPENGL);
    if (!window)
    {
        fprintf(stderr, "Could not create window.ErrorCode = %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GL_CreateContext(window);

    /*
    * 3. Make sure we have a recent version of OpenGL
    */
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        fprintf(stderr, "Colud not initialize glew.ErrorCode = %s\n", glewGetErrorString(glewError));
        SDL_Quit();
        return 1;
    }
    /*
    if (!GLEW_VERSION_3_0)
    {
        fprintf(stderr, "OpenGL max supported version is too low.\n");
        SDL_Quit();
        return 1;
    } */

    /*
    * 4. Setup OpenGL state
    */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 100);
    glEnable(GL_TEXTURE_2D);

    //after submit
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //#######################################
    /*
    * 8. Keep a pointer to SDL's internal keyboard state - to handle input
    */
    kbState = SDL_GetKeyboardState(NULL);

    //unsigned char* keys = SDL_GetKeyboardState(&numKeys);
    //if (keys[SDL_SCANCODE_LEFT]) // if the left arrow is pressed

    /*
    * 12. load the textures
    */
    sonicTex = glTexImageTGAFile("mario.tga", NULL, NULL); //to load all your images before the game loop
    //marioTex = glTexImageTGAFile("sonic.tga", NULL, NULL);
    //######################################

    /*
    * 5. The game loop
    */
    while (!shouldExit)
    {
        /*
        *9. kbState is updated by the message pump. Copy over the old state before the pump!
        */
        memcpy(kbPrevState, kbState, sizeof(kbPrevState)); //to handle input

        /*
        * 10. Handle OS message pump!
        */
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDLK_LEFT:
                imgX -= deltaX;
                break;
            case SDLK_RIGHT:
                imgX += deltaX;
                break;
            case SDLK_UP:
                imgY -= deltaY;
                break;
            case SDLK_DOWN:
                imgY += deltaY;
                break;
            case SDL_QUIT:
                shouldExit = 1;
                break;
            } //switch
            glDrawSprite(sonicTex, imgX, imgY, imgWidth, imgHeight);
        }// end of inner while
        /* 13. */
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawSprite(sonicTex, imgX, imgY, imgWidth, imgHeight); //to draw an image – you may   need to remember how big the image is.
        //glDrawSprite(marioTex, 100, 0, 64, 64);
        //glDrawSprite(sonicTex, 10, 10, 64, 64);
        //glDrawSprite(marioTex, 100, 10, 64, 64);

        /*
        * 6. Game logic goes here
        */
        SDL_GL_SwapWindow(window);
    }// END of outer while

    SDL_Quit();


    getchar();
    return 0;
}

/*
1. Create a window, initialize OpenGL in it (SDL)
2. Run the message pump (SDL)
3. Use the latest version of OpenGL (GLEW)
4. Get keyboard and joystick input (SDL)
5. Load and draw images (DrawUtils.c)
*/