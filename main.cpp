#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" 
{
	#include"./SDL2-2.0.10/include/SDL.h"
	#include"./SDL2-2.0.10/include/SDL_main.h"
}

const int SCREEN_WIDTH = 1080; //pixels
const int SCREEN_HEIGHT = 720;

const int spawnX = SCREEN_WIDTH/2;
const int spawnY = SCREEN_HEIGHT/2;

const char* menuText = "WELCOME TO THE GAME";

const int distanceBetweenColumns = 34;
const int distanceBetweenRows = 17;

const int platformCount = 5;


typedef struct colors_t
{
	int black;
	int red;
	int green;
	int blue;
};


typedef struct hitbox_t
{
	int top;
	int left;
	int right;
	int bottom;
};


typedef struct player_t
{
	SDL_Surface* sprite;
	int playerWidth;
	int playerHeight;
	int playerX;
	int playerY; // im mniejszy tym wyzej
	double verticalVelocity; 
	double horizontalVelocity;
	hitbox_t hitbox;
};


typedef struct platform_t
{
	SDL_Surface* sprite;
	int platformWidth;
	int platformHeight;
	int platformX;
	int platformY;
	hitbox_t hitbox;
};


typedef struct screen_t
{
	SDL_Surface* screen;
	SDL_Surface* charset;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* scrtex;
};

SDL_Surface* initSpecificBMP(screen_t* screen, char* path, int columnIndex, int rowIndex, float scaleFactor);


void initAll(screen_t* screen, colors_t* colors, player_t* player, platform_t** platform);

#pragma region colors
void initColors(colors_t* colors, screen_t screen);
#pragma endregion

#pragma region platforms
void initPlatform(platform_t** platform, screen_t screen);
void initPlatformSprite(platform_t* platform, screen_t screen);
void initPlatformSpawn(platform_t* platform);
void initPlatformHitbox(platform_t* platform);
platform_t* allocatePlatforms();
#pragma endregion

#pragma region player
void initPlayer(player_t* player, screen_t screen);//player initialization
void initPlayerSprite(player_t* player, screen_t screen);
void initPlayerSpawn(player_t* player);
void initPlayerVelocity(player_t* player);
void initPlayerHitbox(player_t* player);

void updatePlayerPosition(player_t* player, double valueX, double valueY);
void updatePlayerHitbox(player_t* player, double valueX, double valueY);
#pragma endregion

#pragma region draw
void drawString(screen_t screen, int x, int y, const char* text);
void drawSurface(screen_t screen, SDL_Surface* sprite, int x, int y);
void drawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void drawLine(screen_t screen, int x, int y, int l, int dx, int dy, Uint32 color);
void drawRectangle(screen_t screen, int x, int y, int width, int height, Uint32 outlineColor, Uint32 fillColor);
void drawMenu(screen_t screen, colors_t colors, const char* menuText);
void drawScene(screen_t screen, colors_t colors, player_t player, platform_t* platform);
// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
#pragma endregion

void initWindowAndRenderer(screen_t* screen);

SDL_Surface* initBMP(screen_t* screen, char* path);//BMP's

void freeAll(screen_t* screen, platform_t** platform);
void freeSurfaces(screen_t* screen);
void freePlatforms(platform_t** platform);

void handleX(player_t player, double* distanceX, double delta);
void handleY(player_t player, double* distanceY, double delta);
void handleXY(player_t player, double* distanceX, double* distanceY, double delta);

void gameLoop(int t1, screen_t screen, player_t* player, char* text, SDL_Event event, colors_t colors, platform_t* platform);

void eventHandler(int* quit, player_t* player, platform_t* platform, SDL_Event event);
void handleKeyDown(SDL_Event event, int* quit, player_t* player, platform_t* platform);

void gravity(player_t* player, platform_t* platform);
int detectColissionX(player_t player, platform_t* platform);
int checkInRange(int x, int platformL, int platformR);

void handleJump(player_t* player, platform_t* platform);
void handleMovement(player_t* player, char side, platform_t* platform);


#ifdef __cplusplus
extern "C"
#endif


int main(int argc, char** argv) 
{
	int t1 = SDL_GetTicks();
	SDL_Event event = { 0 };
	screen_t screen;
	colors_t colors;
	player_t mario;
	platform_t* platform;

	//platform_t platform;
	initAll(&screen, &colors, &mario, &platform);

	char text[128];

	gameLoop(t1, screen, &mario, text, event, colors, platform);

	freeSurfaces(&screen);
	SDL_Quit();

	return 0;
}


void initAll(screen_t* screen, colors_t* colors, player_t* player, platform_t** platform)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	initWindowAndRenderer(screen);
	initColors(colors, *screen);
	initPlayer(player, *screen);
	initPlatform(platform, *screen);
	screen->charset = initBMP(screen, "./cs8x8.bmp");
	SDL_SetColorKey(screen->charset, true, 0x000000);
}


platform_t* allocatePlatforms()
{
	platform_t* platforms = (platform_t*)malloc(platformCount * sizeof(platform_t));

	if (platforms == NULL)
	{
		perror("Error - allocating memory for platforms");
		exit(EXIT_FAILURE);
	}

	return platforms;
}


void initPlatform(platform_t** platform, screen_t screen)
{
	*platform = allocatePlatforms();

	for (int i = 0; i < platformCount; i++)
	{
		initPlatformSprite(&(*platform)[i], screen);
		initPlatformSpawn(&(*platform)[i]);          
		initPlatformHitbox(&(*platform)[i]); 
	}
}


void initPlatformSprite(platform_t* platform, screen_t screen)
{
	platform->sprite = initBMP(&screen, "./DonkeyKongTextures2/ramp.bmp");
	platform->platformHeight = platform->sprite->h;
	platform->platformWidth = platform->sprite->w;
}


void initPlatformSpawn(platform_t* platform)
{
	static int a = -2;
	platform->platformX = 150 * a+ spawnX - 20;
	platform->platformY = spawnY + 100;
	a++;
}


void initPlatformHitbox(platform_t* platform)
{
	
	platform->hitbox.top = platform->platformY - (platform->platformHeight / 2);
	platform->hitbox.bottom = platform->platformY + (platform->platformHeight / 2);
	platform->hitbox.left = platform->platformX - (platform->platformWidth / 2);
	platform->hitbox.right = platform->platformX + (platform->platformWidth / 2);

}


void initColors(colors_t* colors, screen_t screen)
{
	colors->black = SDL_MapRGB(screen.screen->format, 0x00, 0x00, 0x00);
	colors->green = SDL_MapRGB(screen.screen->format, 0x00, 0xFF, 0x00);
	colors->red = SDL_MapRGB(screen.screen->format, 0xFF, 0x00, 0x00);
	colors->blue = SDL_MapRGB(screen.screen->format, 0x11, 0x11, 0xCC);
}


void initPlayer(player_t* player, screen_t screen)
{
	initPlayerSprite(player, screen);
	initPlayerSpawn(player);
	initPlayerHitbox(player);
	initPlayerVelocity(player);
}


void initPlayerSprite(player_t* player, screen_t screen)
{
	//player->sprite = initSpecificBMP(&screen, "./DonkeyKongTextures2/mario.bmp", 3, 0, 1.5);
	player->sprite = initBMP(&screen, "./DonkeyKongTextures2/mario2.bmp");
	player->playerHeight = player->sprite->h;
	player->playerWidth = player->sprite->w;
}


void initPlayerHitbox(player_t* player)
{
	player->hitbox.top = player->playerY - (player->playerHeight / 2);
	player->hitbox.bottom = player->playerY + (player->playerHeight / 2);
	player->hitbox.left = player->playerX - (player->playerWidth / 2);
	player->hitbox.right = player->playerX + (player->playerWidth / 2);
}


void updatePlayerHitbox(player_t* player, double valueX, double valueY)
{
	player->hitbox.top += valueY;
	player->hitbox.bottom += valueY;
	player->hitbox.left += valueX;
	player->hitbox.right += valueX;
}


void initPlayerSpawn(player_t* player)
{
	player->playerX = spawnX;
	player->playerY = spawnY;
}


void initPlayerVelocity(player_t* player)
{
	player->verticalVelocity = 0;
	player->horizontalVelocity = 0;
}


void updatePlayerPosition(player_t* player, double valueX, double valueY)
{
	player->playerX += valueX;
	player->playerY += valueY;
	updatePlayerHitbox(player, valueX, valueY);
}


void initWindowAndRenderer(screen_t* screen)
{
	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, /*SDL_WINDOW_FULLSCREEN_DESKTOP */ 0, &screen->window, &screen->renderer) != 0)
	{
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_RenderSetLogicalSize(screen->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, 255);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_SetWindowTitle(screen->window, "Donkey Kong - 198290");
	//SDL_ShowCursor(SDL_DISABLE);

	screen->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	screen->scrtex = SDL_CreateTexture(screen->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}


SDL_Surface* initBMP(screen_t* screen, char* path)
{
	SDL_Surface* bmp = SDL_LoadBMP(path);

	if (bmp == NULL)
	{
		printf("SDL_LoadBMP(%s) error: %s\n", path, SDL_GetError());
		SDL_FreeSurface(screen->screen);
		SDL_DestroyTexture(screen->scrtex);
		SDL_DestroyWindow(screen->window);
		SDL_DestroyRenderer(screen->renderer);
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	return bmp;
}


SDL_Surface* initSpecificBMP(screen_t* screen, char* path, int columnIndex, int rowIndex, float scaleFactor)
{
	SDL_Surface* bmp = initBMP(screen, path);

	int offsetX = columnIndex * distanceBetweenColumns;
	int offsetY = rowIndex * distanceBetweenRows;

	// Create a new surface representing the specific image
	SDL_Rect imageRect = { offsetX, offsetY, distanceBetweenColumns, distanceBetweenRows };
	SDL_Surface* specificImageSurface = SDL_CreateRGBSurface(0, distanceBetweenColumns, distanceBetweenRows, 32, 0, 0, 0, 0);
	SDL_BlitSurface(bmp, &imageRect, specificImageSurface, NULL);

	SDL_FreeSurface(bmp);

	// Scale the image
	SDL_Surface* scaledImage = SDL_CreateRGBSurface(0, specificImageSurface->w * scaleFactor, specificImageSurface->h * scaleFactor, 32, 0, 0, 0, 0);

	// Scale the image using SDL_BlitScaled
	SDL_Rect scaledRect = { 0, 0, scaledImage->w, scaledImage->h };
	SDL_BlitScaled(specificImageSurface, NULL, scaledImage, &scaledRect);

	SDL_FreeSurface(specificImageSurface);

	return scaledImage;
}


void freeAll(screen_t* screen, platform_t** platform)
{
	freeSurfaces(screen);
	freePlatforms(platform);
}


void freeSurfaces(screen_t* screen)
{
	SDL_FreeSurface(screen->charset);
	SDL_FreeSurface(screen->screen);
	SDL_DestroyTexture(screen->scrtex);
	SDL_DestroyRenderer(screen->renderer);
	SDL_DestroyWindow(screen->window);
}


void freePlatforms(platform_t** platform)
{
	free(platform);
}


void drawString(screen_t screen, int x, int y, const char* text)
{
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text)
	{
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(screen.charset, &s, screen.screen, &d);
		x += 8;
		text++;
	}
}


void drawSurface(screen_t screen, SDL_Surface* sprite, int x, int y)
{
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen.screen, &dest);
}


void drawPixel(SDL_Surface* surface, int x, int y, Uint32 color) 
{
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
}


void drawRectangle(screen_t screen, int x, int y, int width, int height, Uint32 outlineColor, Uint32 fillColor) 
{
	drawLine(screen, x, y, height, 0, 1, outlineColor);
	drawLine(screen, x + width - 1, y, height, 0, 1, outlineColor);
	drawLine(screen, x, y, width, 1, 0, outlineColor);
	drawLine(screen, x, y + height - 1, width, 1, 0, outlineColor);

	for (int i = y + 1; i < y + height - 1; i++)
		drawLine(screen, x + 1, i, width - 2, 1, 0, fillColor);
}


void drawLine(screen_t screen, int x, int y, int l, int dx, int dy, Uint32 color)
{
	for (int i = 0; i < l; i++) 
	{
		drawPixel(screen.screen, x, y, color);
		x += dx;
		y += dy;
	}
}


void drawMenu(screen_t screen, colors_t colors, const char* menuText)
{
	drawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/10, colors.red, colors.black);
	drawString(screen, SCREEN_WIDTH/2 - 4 * strlen(menuText), SCREEN_HEIGHT/10/2, menuText);
}


void drawScene(screen_t screen, colors_t colors, player_t player, platform_t* platform)
{
	SDL_FillRect(screen.screen, NULL, colors.black);

	drawSurface(screen, player.sprite, player.playerX, player.playerY);

	for(int i = 0; i < platformCount; i++)
		drawSurface(screen, platform[i].sprite, platform[i].platformX, platform[i].platformY);

	drawMenu(screen, colors, menuText);
}


void handleX(player_t player, double* distanceX, double delta)
{
	*distanceX = player.horizontalVelocity * delta;

	if (*distanceX > 0)
		*distanceX += 1.0;
}


void handleY(player_t player, double* distanceY, double delta)
{
	*distanceY = player.verticalVelocity * delta;

	if (*distanceY > 0)
		*distanceY += 1;
}


void handleXY(player_t player, double* distanceX, double* distanceY, double delta)
{
	handleX(player, distanceX, delta);
	handleY(player, distanceY, delta);
}


void gameLoop(int t1, screen_t screen, player_t* player, char* text, SDL_Event event, colors_t colors, platform_t* platform)
{
	int quit = 0, jumpFlag = 1;
	double t2, delta, frames = 0, distanceX = 0, distanceY = 0;

	while (!quit)
	{
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		gravity(player, platform);
		handleXY(*player, &distanceX, &distanceY, delta);

		drawScene(screen, colors, *player, platform);

		SDL_UpdateTexture(screen.scrtex, NULL, screen.screen->pixels, screen.screen->pitch);
		SDL_RenderCopy(screen.renderer, screen.scrtex, NULL, NULL);
		SDL_RenderPresent(screen.renderer);

		//		SDL_RenderClear(renderer);

		eventHandler(&quit, player, platform, event);

		updatePlayerPosition(player, distanceX, distanceY);

		frames++;
	}
}


void eventHandler(int* quit, player_t* player, platform_t* platform, SDL_Event event)
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				handleKeyDown(event, quit, player, platform);
				break;

			case SDL_KEYUP:
				player->horizontalVelocity = 0;
				player->verticalVelocity = 0;
				break;

			case SDL_QUIT:
				*quit = 1;
				break;
		}
	}
}


void handleKeyDown(SDL_Event event, int* quit, player_t* player, platform_t* platform)
{
	switch (event.key.keysym.sym)
	{
	case SDLK_ESCAPE:
		*quit = 1;
		break;

	case SDLK_LEFT:
		handleMovement(player, 'L', platform);
		break;

	case SDLK_RIGHT:
		handleMovement(player, 'R', platform);
		break;

	case SDLK_SPACE:
		handleJump(player, platform);
		break;
	}
}


void handleJump(player_t* player, platform_t* platform)
{
	for (int i = 0; i < platformCount; i++)
	{
		if (platform[i].hitbox.top == player->hitbox.bottom)
		{
			player->verticalVelocity = -50.0 * platformCount;
			player->playerY -= 1;
			updatePlayerHitbox(player, 0, -1);
		}
	}
}


void handleMovement(player_t* player, char side, platform_t* platform)
{
	if (detectColissionX(*player, platform) == 0)
	{
		switch (side)
		{
			case 'R':
				player->horizontalVelocity = 0.5;
				break;

			case 'L':
				player->horizontalVelocity = -0.5;
				break;
		}
	}
		
}


//0 jesli tak 1 jesli nie
int checkInRange(int x, int platformL, int platformR)
{
	if (x >= platformL && x <= platformR)
		return 0;
	else
		return 1;
}


int detectColissionX(player_t player, platform_t* platform)
{
	int playerL = player.hitbox.left, playerR = player.hitbox.right;

	for (int i = 0; i < platformCount; i++)
	{
		int platformTop = platform[i].hitbox.top, platformBot = platform[i].hitbox.bottom;

		if ((playerL == platform[i].hitbox.right) && ((checkInRange(player.playerY, platformTop, platformBot) == 0) || (checkInRange(player.hitbox.top, platformTop, platformBot) == 0) || (checkInRange(player.hitbox.bottom, platformTop, platformBot) == 0)))
		{
			player.horizontalVelocity = 0;
			return 1;
		}

		else if ((playerR == platform[i].hitbox.left) && ((checkInRange(player.playerY, platformTop, platformBot) == 0) || (checkInRange(player.hitbox.top, platformTop, platformBot) == 0) || (checkInRange(player.hitbox.bottom, platformTop, platformBot) == 0)))
		{
			player.horizontalVelocity = 0;
			return 1;
		}
	}

	return 0;
}


void gravity(player_t* player, platform_t* platform)
{
	for (int i = 0; i < platformCount; i++)
	{
		int L = platform[i].hitbox.left, R = platform[i].hitbox.right;

		if (player->hitbox.bottom != platform[i].hitbox.top)
			player->verticalVelocity += 0.5;

		else if (checkInRange(player->hitbox.left, L, R) == 1 && checkInRange(player->hitbox.right, L, R) == 1 && checkInRange(player->playerX, L, R) == 1)
			player->verticalVelocity += 0.5;

		else
		{
			player->verticalVelocity = 0;
			break;
		}
	}
}