#include "DrawUtils.h"

#include<GL/GL.h>
#include<GL/glew.h>
#include<SDL.h>
#undef main

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

const char* WINDOW_TITLE = "CS 134 Homework #1 Thuy Nguyen";

/* To Handle Input */	
unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 }; // The previous frame's keyboard state	
const unsigned char* kbState = NULL;// The current frame's keyboard state

/* Art to draw */
GLuint sonicTex;
int imgX = WINDOW_WIDTH/ 2;
int imgY = WINDOW_HEIGHT / 2;
int imgW = 64;
int imgH = 64;

int shouldExit = 0;
int main(void)
{		
	/***  Ia. Initialize SDL	*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Unable to initialize graphics library!\n");
		return 1;
	}
	
	/*** Ib. Create the window, OpenGL context	*/
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	SDL_Window* window = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL);
	
	if (!window)
	{
		fprintf(stderr, "Could not create window.ErrorCode = %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}	
	SDL_GL_CreateContext(window);

	/*** II. Make sure we have a recent version of OpenGL */
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		fprintf(stderr, "Colud not initialize glew.ErrorCode = %s\n", glewGetErrorString(glewError));
		SDL_Quit();
		return 1;
	}	
	if (!GL_VERSION_3_0)
	{
		fprintf(stderr, "OpenGL max supported version is too low.\n");		
		return 1;
	} 

	/*** III. Setup OpenGL state */		
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); //What portion of the screen will be displayed		
	glMatrixMode(GL_PROJECTION);// 2D rendering
	glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 100);	// right to bottom
	glEnable(GL_TEXTURE_2D);  

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*** IV. The game loop - the main loop */			 
	kbState = SDL_GetKeyboardState(NULL); //Keep a pointer to SDL's internal keyboard state
	while (!shouldExit)// including HANDLING INPUT (or EVENTs), LOGIC, RENDERING
	{	
		/* a. HANDLING INPUT */
		/* kbState is updated by the message pump. Copy over the old state before the pump!	*/
		memcpy(kbPrevState, kbState, sizeof(kbPrevState));
		/* b. EVENTS */
		/* Handle OS message pump! */
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
		/* IV.1. Save last frame's values (was just keyboard update)*/
		/* IV.2. Handle OS message pump */
			switch (event.type)
			{					
			case SDL_QUIT: // when the window is closed
				shouldExit = 1;
				break;
			}
		//logic that should happen for a certain event
		}// end of inner while

		/* VI.3. Timing update */
		/* IV.4. LOGIC: Game logic goes here - Update + draw game */
		if (kbState[SDL_SCANCODE_LEFT])  /// if the left arrow is pressed
		{
			imgX -= 2;
			if (imgX <= 0)imgX = 0;
		}
		if (kbState[SDL_SCANCODE_UP])  /// if the left arrow is pressed
		{
			imgY -= 2;
			if (imgY <= 0)imgY = 0;
		}
		if (kbState[SDL_SCANCODE_DOWN])  /// if the left arrow is pressed
		{
			imgY += 2;
			if (imgY >= 480 - imgH)imgY = 480 - imgH;
		}
		if (kbState[SDL_SCANCODE_RIGHT])  /// if the left arrow is pressed
		{
			imgX += 2;
			if (imgX >= 640 - imgW)imgX = 640 - imgW;
		}
		/* c. RENDERING: */
		/* IV.5.1. Rendering: */
		glClearColor(0, 0, 0, 1);  // specific the clear color		
		glClear(GL_COLOR_BUFFER_BIT); //rendering the screen
		/* IV.5.2. load the textures - load all your images before the game loop */
		sonicTex = glTexImageTGAFile("sonic.tga", NULL, NULL);
		glDrawSprite(sonicTex, imgX, imgY, imgW, imgH);
		/* IV.5.3. update a window with OpenGL rendering */
		SDL_GL_SwapWindow(window);
	}// END of outer while

	SDL_Quit();
	return 0;
}
