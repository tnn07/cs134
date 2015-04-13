#include "DrawUtils.h"
#include <stdbool.h>

#include <GL/glew.h>
#include <SDL.h>
#undef main

#define WINDOW_TITLE "CS 134 Homework #3 Thuy Nguyen"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 624

///struct =======================
/* Define a single frame used in an animation */
typedef struct AnimFrameDef {// combined with the AnimDef's name to make the actual texture name
	int frameNum;
	float frameTime;
} AnimFrameDef;
/* Define a single animation used by a character */
typedef struct AnimDef { // or AnimationDef 
	const char* name;
	AnimFrameDef* frames[20];
	int numFrame;
} AnimDef;
/* Runtime state for an animation */
typedef struct AnimData {
	AnimDef* def;
	int curFrame;
	float timeToNextFrame;
	bool isPlaying;
}AnimData;
//Axis-Aligned Bounding Box
typedef struct AABB {
	int x, y, w, h;
} AABB;

/// ======================  
typedef struct ActorDef{
	const char* name;
	const char* walkAnim;
	const char* attackAnim;
};
typedef struct ActorData {
	float pos[2];
	float health;
	bool isWalking;
	AnimData curAnimation;
} ActorData;
/// *=====================
typedef struct LevelActorDef {
	const char* actor;
	float initialPos[2];
	float initialHealth;
} LevelActorDef;
typedef struct LevelActorData {
	float pos[2];
	float health;
	ActorData* actor;
} LevelActorData;
/* Timer */
typedef struct glut_timer_t
{
	double current_time;
	double last_time;

} timer;

typedef struct Player {
	AnimData amin;
	float posX;
	float posY;
	GLuint *imgTGA;
	int direction; // 1forLeft, 2forUp, 3forDown, and 4forRight
} Player;

/* PROTOTYPES */
/// Animation
/* Update the animation for time passing */
/*
* Update the current animation
* @param data - Runtime state for an animation
* @param dt - time to next frame
* @return - void
*/
void amimTick(AnimData* data, float dt);
/*
* Set the current animation playing
* @param anim - Runtime state for an animation
* @param toPlay -
* @return - void
*/
void animSet(AnimData* anim, AnimDef* toPlay);
/*
* Restart the current animation
* @param anim - Runtime state for an animation
* @return - void
*/
void animReset(AnimData* anim);
/*
* Draw the state of the animation
* Drawing a sprite is just drawing its current animation
* @param anim
* @param x
* @param y
* @param w
* @param h
* @return - void
*/
void animDraw(AnimData* anim, int x, int y, int w, int h);
bool AABBIntersect(const AABB *box1, const AABB *box2);

GLuint testPlayers[4][8];
Player player;
Player star;

unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 };
const unsigned char* kbState = NULL;

float deltaTime;
int speed = 300;

int shouldExit = 0;

int main(void)
{
	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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

	kbState = SDL_GetKeyboardState(NULL);
	
	/// get images for the test draw =========================	
	char fname[80];
	int c, r;
	for (r = 0; r < 4; r++)
	{
		for (c = 0; c < 8; c++)
		{
			sprintf(fname, "assets/%d-%dGr.tga", c, r+1);
			testPlayers[r][c] = glTexImageTGAFile(fname, NULL, NULL);
		}
	}

/// =============================================================
	/* Load the textures */
	GLuint lTGA[8], uTGA[8], dTGA[8], rTGA[8];	
	/// 1 to load images going left
	player.direction++; 
	for (int c = 0; c < 8; c++)
	{
		sprintf(fname, "assets/%d-%dGr.tga", c, player.direction);
		lTGA[c] = glTexImageTGAFile(fname, NULL, NULL);
	}
	/// 2 to load images going up
	player.direction++;
	for (int c = 0; c < 8; c++)
	{
		sprintf(fname, "assets/%d-%dGr.tga", c, player.direction);
		uTGA[c] = glTexImageTGAFile(fname, NULL, NULL);
	}
	/// 3 to load images going down
	player.direction++;
	for (int c = 0; c < 8; c++)
	{
		sprintf(fname, "assets/%d-%dGr.tga", c, player.direction);
		dTGA[c] = glTexImageTGAFile(fname, NULL, NULL);
	}

	/// 4 to load images going right
	player.direction++;
	for (int c = 0; c < 8; c++)
	{
		sprintf(fname, "assets/%d-%dGr.tga", c, player.direction);
		rTGA[c] = glTexImageTGAFile(fname, NULL, NULL);
	}
	/// --- Timing with SDL_GetTicks
	Uint32 lastFrameMs;
	Uint32 currentFrameMs = SDL_GetTicks();
	// intial position of player
	player.amin.curFrame = 0;
	player.amin.isPlaying = true;
	//player.amin.timeToNextFrame = currentFrameMs;
	player.posX = 216.0; player.posY = 576.0;
	player.direction = 0;
	player.imgTGA = lTGA;
	// intial position of star
	star.amin.isPlaying = false;
	//star.amin.timeToNextFrame = currentFrameMs;
	star.amin.curFrame = 0;
	star.posX = 640.0 - 48.0;
	star.posY = 0.0;
	star.imgTGA = glTexImageTGAFile("assets/dollar0.tga", NULL, NULL);


	/* The Game Loop */
	while (!shouldExit)
	{
		lastFrameMs = currentFrameMs;
		///--- Timing
		currentFrameMs = SDL_GetTicks();
		deltaTime = (currentFrameMs - lastFrameMs) / 1000.0f;
		
		memcpy(kbPrevState, kbState, sizeof(kbPrevState));

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT: 
				shouldExit = 1;
				break;
			}			
		}/// end of inner while
		glClearColor(0, 1, 1, 1);  // specific the clear color		
		glClear(GL_COLOR_BUFFER_BIT); //rendering the screen

		// draw a matrix of TGA images to test.
		for (r = 0; r < 4; r++)
		{
			for (c = 0; c < 8; c++)
			{
				glDrawSprite(testPlayers[r][c], c * 48, r * 48, 48, 48);
			}
		}
		if (kbState[SDL_SCANCODE_LEFT]) {
			player.direction = 1; player.imgTGA = lTGA;			
			player.posX -= speed * deltaTime;
			if (player.posX <= 0.0)player.posX = 0.0;
		}
		if (kbState[SDL_SCANCODE_UP]) {
			player.direction = 2; player.imgTGA = uTGA;		
			player.posY -= speed * deltaTime;
			if (player.posY <= 0.0)player.posY = 0.0;
		}
		if (kbState[SDL_SCANCODE_DOWN]) {
			player.direction = 3; player.imgTGA = dTGA;
			player.posY += speed * deltaTime;
			if (player.posY >= 624.0 - 48.0) player.posY = 624.0 - 48.0;
		}
		if (kbState[SDL_SCANCODE_RIGHT]) {
			player.direction = 4; player.imgTGA = rTGA;
			player.posX += speed * deltaTime;
			if (player.posX >= 640.0 - 48.0)player.posX = 640.0 - 48.0;
		}
		/* Draw the main sprite depending on its direction */
		switch (player.direction)
		{
		case 1: 					
			for (int i = 0; i < 8; i++)
			{
				glDrawSprite(player.imgTGA[i], player.posX, player.posY, 48, 48);
				SDL_Delay(deltaTime);
			}
			break;
		case 2:			
			for (int i = 0; i < 8; i++)
			{
				glDrawSprite(player.imgTGA[i], player.posX, player.posY, 48, 48);
				SDL_Delay(deltaTime);
			}
			break;
		case 3:				
			for (int i = 0; i < 8; i++)
			{
				glDrawSprite(player.imgTGA[i], player.posX, player.posY, 48, 48);
				SDL_Delay(deltaTime);
			}
			break;
		case 4:		
			for (int i = 0; i < 8; i++)
			{
				glDrawSprite(player.imgTGA[i], player.posX, player.posY, 48, 48);
				SDL_Delay(deltaTime);
			}
			break;
		default: break;
		}
		glDrawSprite(player.imgTGA[0], player.posX, player.posY, 48, 48);
		glDrawSprite(star.imgTGA, star.posX, star.posY, 48, 48);
		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();
	return 0;
}

/* Functions */
void animTick(AnimData* data, float dt)
{
	if (!data->isPlaying)
	{
		return;
	}
	int numFrames = data->def->numFrame;
	data->timeToNextFrame -= dt;
	if (data->timeToNextFrame < 0)
	{
		++data->curFrame;
		if (data->curFrame)
		{
			//end of the animation, stop it
			data->curFrame = numFrames - 1;
			data->timeToNextFrame = 0;
			data->isPlaying = false;
		}
		else
		{
			AnimFrameDef* curFrame;
			curFrame = data->def->frames[data->curFrame];
			data->timeToNextFrame += curFrame->frameTime;
		}
	}
}
void animSet(AnimData* anim, AnimDef* toPlay)
{
	anim->def = toPlay;
	anim->curFrame = 0;
	anim->timeToNextFrame = toPlay->frames[0]->frameTime;
	anim->isPlaying = true;
}
void animReset(AnimData* anim)
{
	animSet(anim, anim->def);
}
void animDraw(AnimData* anim, int x, int y, int w, int h)
{
	AnimDef* def = anim->def;
	int curFrameNum = def->frames[anim->curFrame]->frameNum;
	//GLuint tex = textures[curFrameNum];
	//glDrawSprite(tex, x, y, w, h);
}
bool AABBIntersect(const AABB *box1, const AABB *box2)
{
	// box1 to the right
	if (box1->x > box2->x + box2->w) {
		return false;
	}
	// box1 to the left
	if (box1->x + box1->w < box2->x) {
		return false;
	}
	// box1 below
	if (box1->y > box2->y + box2->h) {
		return false;
	}
	// box1 above
	if (box1->y + box1->h < box2->y) {
		return false;
	}
	return true;
}