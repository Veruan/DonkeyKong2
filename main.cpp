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

const int SCREEN_WIDTH = 1200; //pixels
const int SCREEN_HEIGHT = 840;

const int levelWidth = 9;
const int levelHeight = 33;

const int xModifier = 120;
const int yModifier = 16;

const int yPlatformOffset = 10;
const int yLadderOffset = 8;
const int yPlayerOffset = 8;

const char* menuText = "WELCOME TO THE GAME";
const double menuTextScale = 5.0;
const int menuOptions = 4;
const int titleFlag = 10;
const int pseudonymStartPlace = 50;

const double jumpStrength = -50.0;


typedef struct menu_t
{
	int active[menuOptions];
};


typedef struct colors_t
{
	int black;
	int red;
	int green;
	int blue;
	int yellowOrange;
	int platinum;
	int darkPurple;
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
	int ladderFlag;
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


typedef struct ladder_t
{
	SDL_Surface* sprite;
	int ladderY;
	int ladderX;
	int ladderHeight;
	int ladderWidth;
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


typedef struct level_t
{
	int platformCount;
	int ladderCount;
	int* platformsX;
	int* platformsY;
	int* laddersX;
	int* laddersY;
	int playerSpawn[2];
	char rampColor[10];
};


typedef struct playerSprites_t
{
	SDL_Surface* left1;
	SDL_Surface* left2;

	SDL_Surface* right1;
	SDL_Surface* right2;

	SDL_Surface* ladder1;
	SDL_Surface* ladder2;

	SDL_Surface* jumpLeft;
	SDL_Surface* jumpRight;
};


typedef struct items_t
{
	player_t player;
	platform_t** platform;
	ladder_t** ladder;
};


void initAll(screen_t* screen, colors_t* colors, player_t* player, platform_t** platform, level_t* level, ladder_t** ladder);

void initStatic(screen_t* screen, colors_t* colors);
void initDynamic(screen_t* screen, player_t* player, platform_t** platform, level_t* level, int levelNum, ladder_t** ladder);

#pragma region menu
void menu(screen_t screen, colors_t colors, int* levelNumber);
void initMenu(menu_t* menu);
void drawMenu(screen_t screen, colors_t colors, menu_t menu);
void drawMenuText(screen_t screen, colors_t colors, menu_t menu);
void drawMenuString(screen_t screen, colors_t colors, const char* text, int y, menu_t menu, int index);
void menuEvent(SDL_Event event, int* levelEntered, int* option, menu_t* menu);
void menuKeyDown(SDL_Event event, int* levelEntered, int* option, menu_t* menu);
int findActive(menu_t menu);
void optionView(int option, screen_t screen, SDL_Event event);
void enterPseudonym(screen_t screen, SDL_Event event);
void handleInput(SDL_Event event, int* pseudonymEntered, screen_t screen);
#pragma endregion

#pragma region colors
void initColors(colors_t* colors, screen_t screen);
#pragma endregion

#pragma region platforms
void initPlatform(platform_t** platform, screen_t screen, level_t level);
void initPlatformSprite(platform_t* platform, screen_t screen, level_t level);
void initPlatformSpawn(platform_t* platform, level_t level, int index);
void initPlatformHitbox(platform_t* platform);
platform_t* allocatePlatforms(level_t level);
#pragma endregion

#pragma region ladders
void initLadder(ladder_t** ladder, screen_t screen, level_t level);
void initLadderSprite(ladder_t* ladder, screen_t screen, level_t level);
void initLadderSpawn(ladder_t* ladder, level_t level, int index);
void initLadderHitbox(ladder_t* ladder);
ladder_t* allocateLadders(level_t level);
#pragma endregion

#pragma region player
void initPlayer(player_t* player, screen_t screen, level_t level);//player initialization
void initPlayerSprite(player_t* player, screen_t screen);
void initPlayerSpawn(player_t* player, level_t level);
void initPlayerVelocity(player_t* player);
void initPlayerHitbox(player_t* player);

void updatePlayerPosition(player_t* player, double valueX, double valueY);
void updatePlayerHitbox(player_t* player, double valueX, double valueY);
#pragma endregion

#pragma region level
void initLevel(level_t* level, int levelNum);
char* handleFilePath(int levelNum);
void writeLevelInfo(char** levelTxt, const char* filePath, level_t* level);
char** allocateLevelTxt();

void interpretLevelData(level_t* level, char** levelTxt);
void interpretItemCount(level_t* level, char** levelTxt);
void allocatePlatformsCoords(level_t* level, int platformCount);
void allocateLaddersCoords(level_t* level, int ladderCount);
void interpretItemsPosition(level_t* level, char** levelTxt);
#pragma endregion

#pragma region draw
void updateScreen(screen_t screen);
void drawString(screen_t screen, int x, int y, const char* text, double scale);
void drawSurface(screen_t screen, SDL_Surface* sprite, int x, int y);
void drawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void drawLine(screen_t screen, int x, int y, int l, int dx, int dy, Uint32 color);
void drawRectangle(screen_t screen, int x, int y, int width, int height, Uint32 outlineColor, Uint32 fillColor);
void drawGameMenu(screen_t screen, colors_t colors, const char* menuText);
void drawScene(screen_t screen, colors_t colors, player_t player, platform_t* platform, ladder_t* ladder, level_t level);
void draw(screen_t screen, colors_t colors, player_t* player, platform_t* platform, ladder_t* ladder, level_t level);
// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
#pragma endregion

void initWindowAndRenderer(screen_t* screen);

SDL_Surface* initBMP(screen_t* screen, char* path);

#pragma region free
void freeAll(screen_t* screen, platform_t** platform);
void freeSurfaces(screen_t* screen);
void freePlatforms(platform_t** platform);
void freeLevelTxt(char** levelTxt);
#pragma endregion

#pragma region movementAppliance
void handleX(player_t player, double* distanceX, double delta);
void handleY(player_t player, double* distanceY, double delta);
void handleXY(player_t player, double* distanceX, double* distanceY, double delta);
#pragma endregion

void gameLoop(int t1, screen_t screen, player_t* player, char* text, SDL_Event event, colors_t colors, platform_t* platform, ladder_t* ladder, level_t level);

#pragma region events
void eventHandler(int* quit, player_t* player, platform_t* platform, ladder_t* ladder, SDL_Event event, level_t level);
void handleKeyDown(SDL_Event event, int* quit, player_t* player, platform_t* platform, ladder_t* ladder, level_t level);
#pragma endregion

#pragma region physics
void physics(player_t* player, platform_t* platform, ladder_t* ladder, level_t level, double delta);
void gravity(player_t* player, platform_t* platform, level_t level);
int detectColissionX(player_t* player, platform_t* platform, level_t level, double delta);
int detectBottomColissionY(player_t player, platform_t* platform, level_t level);
int detectTopCollisionY(player_t player, platform_t* platform, level_t level);
int checkInRange(int x, int smaller, int greater);
int canEnterLadder(player_t player, ladder_t* ladder, level_t level, double modifier);
int isOnLadder(player_t player, ladder_t* ladder, level_t level);
int checkLadderPlayerY(player_t player, ladder_t ladder);
#pragma endregion

#pragma region movement
void handleJump(player_t* player, platform_t* platform, level_t level);
void handleMovement(player_t* player, char side, platform_t* platform, level_t level);
void handleLadder(player_t* player, ladder_t* ladder, level_t level, char sign);
void enterLadder(player_t* player, double modifier);
#pragma endregion

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
	level_t level;
	ladder_t* ladder;


	initAll(&screen, &colors, &mario, &platform, &level, &ladder);

	char text[128];

	gameLoop(t1, screen, &mario, text, event, colors, platform, ladder, level);

	freeSurfaces(&screen);
	SDL_Quit();

	return 0;
}


void initAll(screen_t* screen, colors_t* colors, player_t* player, platform_t** platform, level_t* level, ladder_t** ladder)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	initStatic(screen, colors);

	int levelNumber;
	menu(*screen, *colors, &levelNumber);

	initDynamic(screen, player, platform, level, levelNumber, ladder);
}


void initStatic(screen_t* screen, colors_t* colors)
{
	initWindowAndRenderer(screen);
	initColors(colors, *screen);
	screen->charset = initBMP(screen, "./cs8x8.bmp");
	SDL_SetColorKey(screen->charset, true, 0x000000);
}


void menu(screen_t screen, colors_t colors, int* levelNumber)
{
	int levelEntered = 0, option;

	menu_t menu;
	initMenu(&menu);
	SDL_Event event = { 0 };


	while(!levelEntered)
	{
		drawMenu(screen, colors, menu);

		menuEvent(event, &levelEntered, &option, &menu);
	}

	*levelNumber = option + 1;

	event = { 0 };
	optionView(option, screen, event);
}


void initDynamic(screen_t* screen, player_t* player, platform_t** platform, level_t* level, int levelNum, ladder_t** ladder)
{
	initLevel(level, levelNum);
	initPlatform(platform, *screen, *level);
	initPlayer(player, *screen, *level);
	initLadder(ladder, *screen, *level);
}


void initColors(colors_t* colors, screen_t screen)
{
	colors->black = SDL_MapRGB(screen.screen->format, 0x00, 0x00, 0x00);
	colors->green = SDL_MapRGB(screen.screen->format, 0x00, 0xFF, 0x00);
	colors->red = SDL_MapRGB(screen.screen->format, 0xFF, 0x00, 0x00);
	colors->blue = SDL_MapRGB(screen.screen->format, 0x11, 0x11, 0xCC);
	colors->yellowOrange = SDL_MapRGB(screen.screen->format, 0xFF, 0xAA, 0x47);
	colors->platinum = SDL_MapRGB(screen.screen->format, 0xE6, 0xE8, 0xE6);
	colors->darkPurple = SDL_MapRGB(screen.screen->format, 0x42, 0x00, 0x39);
}



void initMenu(menu_t* menu)
{
	for (int i = 0; i < menuOptions; i++)
		menu->active[i] = 0;

	menu->active[0] = 1;
}


void menuEvent(SDL_Event event, int* levelEntered, int* option, menu_t* menu)
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			menuKeyDown(event, levelEntered, option, menu);
			break;

		case SDL_KEYUP:
			break;

		case SDL_QUIT:
			*levelEntered = 1;
			break;
		}
	}
}


void menuKeyDown(SDL_Event event, int* levelEntered, int* option, menu_t* menu)
{
	int active = findActive(*menu);

	switch (event.key.keysym.sym)
	{
		case SDLK_ESCAPE:
			exit(EXIT_SUCCESS);
			break;

		case SDLK_UP:
			menu->active[active] = 0;

			if (active == 0)
				active = menuOptions;

			menu->active[active - 1] = 1;
			break;

		case SDLK_DOWN:
			menu->active[active] = 0;

			if (active == menuOptions - 1)
				active = -1;

			menu->active[active + 1] = 1;
			break;

		case SDLK_RETURN:
			active = findActive(*menu);
			*option = active;
			*levelEntered = 1;
			break;
	}
}


int findActive(menu_t menu)
{
	for (int i = 0; i < menuOptions; i++)
	{
		if (menu.active[i] == 1)
			return i;
	}

	perror("Error - no active menu option found");
	exit(EXIT_FAILURE);
}


void optionView(int option, screen_t screen, SDL_Event event)
{
	SDL_RenderClear(screen.renderer);
	updateScreen(screen);

	if (option != menuOptions - 1);
	{
		enterPseudonym(screen, event);
	}

		//
}


void enterPseudonym(screen_t screen, SDL_Event event)
{
	int pseudonymEntered = 0;

	while (!pseudonymEntered)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				handleInput(event, &pseudonymEntered, screen);
			}

		updateScreen(screen);
		}
	}
}


void handleInput(SDL_Event event, int* pseudonymEntered, screen_t screen)
{
	int cursorPosition = 0;

	switch (event.key.keysym.sym)
	{
	case SDLK_ESCAPE:
		exit(EXIT_SUCCESS);
		break;

	case SDLK_BACKSPACE:
		cursorPosition--;
		break;

	case SDLK_RETURN:
		*pseudonymEntered = 1;
		break;

	default:

		if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z)
		{
			char* letter = (char*)malloc(1 * sizeof(char));
			if (letter == NULL)
			{
				perror("Error - error allocating memory for user input");
				exit(EXIT_FAILURE);
			}

			*letter = char(event.key.keysym.sym);

			drawString(screen, pseudonymStartPlace + int(menuTextScale) * cursorPosition, SCREEN_HEIGHT / 2, letter, menuTextScale);
			cursorPosition++;
		}
		break;
	}
}


void drawMenu(screen_t screen, colors_t colors, menu_t menu)
{
	SDL_FillRect(screen.screen, NULL, colors.blue);
	drawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, colors.platinum, colors.darkPurple);
	drawMenuText(screen, colors, menu);
	updateScreen(screen);
}


void drawMenuText(screen_t screen, colors_t colors, menu_t menu)
{
	drawMenuString(screen, colors, "DONKEY KONG BY 198290", SCREEN_HEIGHT / 10, menu, titleFlag);
	drawMenuString(screen, colors, "Level 1", 3 * SCREEN_HEIGHT / 10, menu, 0);
	drawMenuString(screen, colors, "Level 2", 4 * SCREEN_HEIGHT / 10, menu, 1);
	drawMenuString(screen, colors, "Level 3", 5 * SCREEN_HEIGHT / 10, menu, 2);
	drawMenuString(screen, colors, "High Score Table", 7 * SCREEN_HEIGHT / 10, menu, 3);
}


void drawMenuString(screen_t screen, colors_t colors, const char* text, int y, menu_t menu, int index)
{
	drawString(screen, SCREEN_WIDTH / 2 - 4 * int(menuTextScale) * strlen(text), y, text, menuTextScale);

	if(index != titleFlag && menu.active[index] == 1)
		drawRectangle(screen, SCREEN_WIDTH / 2 - 4 * int(menuTextScale) * strlen(text), y + 8 * int(menuTextScale), strlen(text) * 8 * int(menuTextScale), 3, colors.yellowOrange, colors.yellowOrange);
}


platform_t* allocatePlatforms(level_t level)
{
	platform_t* platforms = (platform_t*)malloc(level.platformCount * sizeof(platform_t));

	if (platforms == NULL)
	{
		perror("Error - allocating memory for platforms");
		exit(EXIT_FAILURE);
	}

	return platforms;
}


void initPlatform(platform_t** platform, screen_t screen, level_t level)
{
	*platform = allocatePlatforms(level);

	for (int i = 0; i < level.platformCount; i++)
	{
		initPlatformSprite(&(*platform)[i], screen, level);
		initPlatformSpawn(&(*platform)[i], level, i);          
		initPlatformHitbox(&(*platform)[i]); 
	}
}


void initPlatformSprite(platform_t* platform, screen_t screen, level_t level)
{
	char rampPath[40];
	snprintf(rampPath, sizeof(rampPath), "%s-%s.bmp", "./DonkeyKongTextures2/ramp", level.rampColor);

	platform->sprite = initBMP(&screen, rampPath);
	platform->platformHeight = platform->sprite->h;
	platform->platformWidth = platform->sprite->w;
}


void initPlatformSpawn(platform_t* platform, level_t level, int index)
{
	platform->platformX = level.platformsX[index];
	platform->platformY = level.platformsY[index];
}


void initPlatformHitbox(platform_t* platform)
{
	
	platform->hitbox.top = platform->platformY - (platform->platformHeight / 2);
	platform->hitbox.bottom = platform->platformY + (platform->platformHeight / 2);
	platform->hitbox.left = platform->platformX - (platform->platformWidth / 2);
	platform->hitbox.right = platform->platformX + (platform->platformWidth / 2);

}


void initLadder(ladder_t** ladder, screen_t screen, level_t level)
{
	*ladder = allocateLadders(level);

	for (int i = 0; i < level.ladderCount; i++)
	{
		initLadderSprite(&(*ladder)[i], screen, level);
		initLadderSpawn(&(*ladder)[i], level, i);
		initLadderHitbox(&(*ladder)[i]);
	}
}


ladder_t* allocateLadders(level_t level)
{
	ladder_t* ladders = (ladder_t*)malloc(level.ladderCount * sizeof(ladder_t));

	if (ladders == NULL)
	{
		perror("Error - allocating memory for platforms");
		exit(EXIT_FAILURE);
	}

	return ladders;
}


void initLadderSprite(ladder_t* ladder, screen_t screen, level_t level)
{
	char ladderPath[40];
	snprintf(ladderPath, sizeof(ladderPath), "%s-%s.bmp", "./DonkeyKongTextures2/ladder", level.rampColor);

	ladder->sprite = initBMP(&screen, ladderPath);
	ladder->ladderHeight = ladder->sprite->h;
	ladder->ladderWidth = ladder->sprite->w;
}


void initLadderSpawn(ladder_t* ladder, level_t level, int index)
{
	ladder->ladderX = level.laddersX[index];
	ladder->ladderY = level.laddersY[index];
}


void initLadderHitbox(ladder_t* ladder)
{

	ladder->hitbox.top = ladder->ladderY - (ladder->ladderHeight / 2);
	ladder->hitbox.bottom = ladder->ladderY + (ladder->ladderHeight / 2);
	ladder->hitbox.left = ladder->ladderX - (ladder->ladderWidth / 2);
	ladder->hitbox.right = ladder->ladderX + (ladder->ladderWidth / 2);

}


void initPlayer(player_t* player, screen_t screen, level_t level)
{
	initPlayerSprite(player, screen);
	initPlayerSpawn(player, level);
	initPlayerHitbox(player);
	initPlayerVelocity(player);
	player->ladderFlag = 0;
}


void initPlayerSprite(player_t* player, screen_t screen)
{
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


void initPlayerSpawn(player_t* player, level_t level)
{
	player->playerY = level.playerSpawn[0];
	player->playerX = level.playerSpawn[1];
}


void initPlayerVelocity(player_t* player)
{
	player->verticalVelocity = 0;
	player->horizontalVelocity = 0;
}


void initLevel(level_t* level, int levelNum)
{
	char* filePath = handleFilePath(levelNum);
	char** levelTxt = allocateLevelTxt();

	writeLevelInfo(levelTxt, filePath, level);

	interpretLevelData(level, levelTxt);

	freeLevelTxt(levelTxt);
	free(filePath);
}


char* handleFilePath(int levelNum)
{
	char base[15] = "./levels/level";
	char levelNumTxt[2]; 

	snprintf(levelNumTxt, sizeof(levelNumTxt), "%d", levelNum);

	char* filePath = (char*)malloc(strlen(base) + strlen(levelNumTxt) + 5);

	if (filePath == NULL)
	{
		perror("Error - error allocating memory for file path");
		exit(EXIT_FAILURE);
	}

	strcpy(filePath, base);
	strcat(filePath, levelNumTxt);
	strcat(filePath, ".txt");

	return filePath;
}


char** allocateLevelTxt()
{
	char** levelTxt = (char**)malloc(levelHeight * sizeof(char*));

	if (levelTxt == NULL)
	{
		perror("Error - error allocating memory for levelTxt");
		exit(EXIT_FAILURE);
	}


	for (int i = 0; i < levelHeight; i++)
	{
		levelTxt[i] = (char*)malloc(levelWidth * sizeof(char));

		if (levelTxt[i] == NULL)
		{
			perror("Error - error allocating memory for levelTxt");
			exit(EXIT_FAILURE);
		}
	}

	return levelTxt;
}


void writeLevelInfo(char** levelTxt, const char* filePath, level_t* level)
{
	FILE* file = fopen(filePath, "r");

	if (file == NULL)
	{
		perror("Error - error opening level file");
		exit(EXIT_FAILURE);
	}

	fscanf(file, "%s", level->rampColor);

	for (int i = 0; i < levelHeight; i++)
	{
		for (int j = 0; j < levelWidth; j++) 
		{
			char fileChar;

			fileChar = fgetc(file);

			if (fileChar == '#' || fileChar == 'X' || fileChar == 'O' || fileChar == 'H')
				levelTxt[i][j] = fileChar;

			else
				j--;
		}
	}

	fclose(file);
}


void interpretLevelData(level_t* level, char** levelTxt)
{
	interpretItemCount(level, levelTxt);
	allocatePlatformsCoords(level, level->platformCount);
	allocateLaddersCoords(level, level->ladderCount);
	interpretItemsPosition(level, levelTxt);
}


void interpretItemCount(level_t* level, char** levelTxt)
{
	int platformCount = 0;
	int ladderCount = 0;

	for (int i = 0; i < levelHeight; i++)
	{
		for (int j = 0; j < levelWidth; j++)
		{
			if (levelTxt[i][j] == 'X' || levelTxt[i][j] == 'x')
			{
				platformCount++;
			}

			if (levelTxt[i][j] == 'H' || levelTxt[i][j] == 'h')
			{
				ladderCount++;
			}
		}
	}

	level->platformCount = platformCount;
	level->ladderCount = ladderCount;
}


void allocatePlatformsCoords(level_t* level, int platformCount)
{
	level->platformsX = (int*)malloc(platformCount * sizeof(int));
	level->platformsY = (int*)malloc(platformCount * sizeof(int));

	if (level->platformsX == NULL || level->platformsY == NULL)
	{
		perror("Error - error allocating memory for level platform info");
		exit(EXIT_FAILURE);
	}
}


void allocateLaddersCoords(level_t* level, int ladderCount)
{
	level->laddersX = (int*)malloc(ladderCount * sizeof(int));
	level->laddersY = (int*)malloc(ladderCount * sizeof(int));

	if (level->laddersX == NULL || level->laddersY == NULL)
	{
		perror("Error - error allocating memory for level ladder info");
		exit(EXIT_FAILURE);
	}
}


void interpretItemsPosition(level_t* level, char** levelTxt)
{
	int platformCounter = 0;
	int ladderCounter = 0;

	for (int i = 0; i < levelHeight; i++)
	{
		for (int j = 0; j < levelWidth; j++)
		{

			switch (levelTxt[i][j])
			{
				case 'X':
					level->platformsY[platformCounter] = (i + yPlatformOffset) * yModifier;
					level->platformsX[platformCounter] = (j + 1) * xModifier;

					platformCounter++;
					break;

				case 'O':
					level->playerSpawn[0] = (i + yPlayerOffset)* yModifier;
					level->playerSpawn[1] = (j + 1) * xModifier;
					break;

				case 'H':
					level->laddersY[ladderCounter] = (i + yLadderOffset) * yModifier;
					level->laddersX[ladderCounter] = (j + 1) * xModifier;

					ladderCounter++;
					break;
			}
		}
	}
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


void freeLevelTxt(char** levelTxt)
{
	for (int i = 0; i < levelHeight; i++)
	{
		free(levelTxt[i]);
	}

	free(levelTxt);
}


void drawString(screen_t screen, int x, int y, const char* text, double scale)
{
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8 * scale;
	d.h = 8 * scale;

	while (*text)
	{
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;

		s.x = px;
		s.y = py;

		d.x = x;
		d.y = y;

		SDL_BlitScaled(screen.charset, &s, screen.screen, &d);
		x += 8 * scale;
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


void drawGameMenu(screen_t screen, colors_t colors, const char* menuText)
{
	drawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/10, colors.red, colors.black);
	drawString(screen, SCREEN_WIDTH/2 - 4 * strlen(menuText), SCREEN_HEIGHT/10/2, menuText, 1);
}


void drawScene(screen_t screen, colors_t colors, player_t player, platform_t* platform, ladder_t* ladder, level_t level)
{
	SDL_FillRect(screen.screen, NULL, colors.black);

	for (int i = 0; i < level.platformCount; i++)
		drawSurface(screen, platform[i].sprite, platform[i].platformX, platform[i].platformY);

	for (int i = 0; i < level.ladderCount; i++)
		drawSurface(screen, ladder[i].sprite, ladder[i].ladderX, ladder[i].ladderY);

	drawSurface(screen, player.sprite, player.playerX, player.playerY);

	drawGameMenu(screen, colors, menuText);
}


void updateScreen(screen_t screen)
{
	SDL_UpdateTexture(screen.scrtex, NULL, screen.screen->pixels, screen.screen->pitch);
	SDL_RenderCopy(screen.renderer, screen.scrtex, NULL, NULL);
	SDL_RenderPresent(screen.renderer);
}


void draw(screen_t screen, colors_t colors, player_t* player, platform_t* platform, ladder_t* ladder, level_t level)
{
	drawScene(screen, colors, *player, platform, ladder, level);
	updateScreen(screen);
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


void gameLoop(int t1, screen_t screen, player_t* player, char* text, SDL_Event event, colors_t colors, platform_t* platform, ladder_t* ladder, level_t level)
{
	int quit = 0;
	double t2, delta, frames = 0, distanceX = 0, distanceY = 0;

	while (!quit)
	{
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		physics(player, platform, ladder, level, delta);
		handleXY(*player, &distanceX, &distanceY, delta);

		draw(screen, colors, player, platform, ladder, level);

		eventHandler(&quit, player, platform, ladder, event, level);

		updatePlayerPosition(player, distanceX, distanceY);

		frames++;
	}
}


void eventHandler(int* quit, player_t* player, platform_t* platform, ladder_t* ladder, SDL_Event event, level_t level)
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				handleKeyDown(event, quit, player, platform, ladder, level);
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


void handleKeyDown(SDL_Event event, int* quit, player_t* player, platform_t* platform, ladder_t* ladder, level_t level)
{
	switch (event.key.keysym.sym)
	{
		case SDLK_ESCAPE:
			*quit = 1;
			break;

		case SDLK_LEFT:
			handleMovement(player, 'L', platform, level);
			break;

		case SDLK_RIGHT:
			handleMovement(player, 'R', platform, level);
			break;

		case SDLK_SPACE:
			handleJump(player, platform, level);
			break;

		case SDLK_UP:
			handleLadder(player, ladder, level, '+');
			break;

		case SDLK_DOWN:
			handleLadder(player, ladder, level, '-');
			break;
	}
}


void handleJump(player_t* player, platform_t* platform, level_t level)
{
	for (int i = 0; i < level.platformCount; i++)
	{
		if (platform[i].hitbox.top == player->hitbox.bottom && player->verticalVelocity == 0)
		{
			player->verticalVelocity = jumpStrength;
			player->playerY -= 1;
			updatePlayerHitbox(player, 0, -1);
		}
	}
}


void handleMovement(player_t* player, char side, platform_t* platform, level_t level)
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


void handleLadder(player_t* player, ladder_t* ladder, level_t level, char sign)
{
	double modifier;

	switch (sign)
	{
		case '+':
			modifier = -1.0;
			break;
		case '-':
			modifier = 1.0;
			break;
		default:
			perror("Error - unknown ladder modifier sign");
			exit(EXIT_FAILURE);
	}

	if (player->ladderFlag == 0 && canEnterLadder(*player, ladder, level, modifier) == 0)
		enterLadder(player, modifier);

	else if (player->ladderFlag == 1)
	{
		player->verticalVelocity = modifier * 0.5;
	}
}


int canEnterLadder(player_t player, ladder_t* ladder, level_t level, double modifier)
{
	for (int i = 0; i < level.ladderCount; i++)
	{
		if ((player.verticalVelocity == 0 || player.ladderFlag == 1)&&
			checkInRange(player.playerX, ladder[i].hitbox.left, ladder[i].hitbox.right) == 0 &&
			checkLadderPlayerY(player, ladder[i]) == 0)
		{
			if (modifier < 0)
			{
				if(ladder[i].hitbox.top < player.hitbox.top)
					return 0;
			}

			if (modifier > 0)
			{
				if (ladder[i].hitbox.bottom > player.hitbox.bottom)
					return 0;
			}

		}
	}

	return 1;
}


int isOnLadder(player_t player, ladder_t* ladder, level_t level)
{
	for (int i = 0; i < level.ladderCount; i++)
	{
		if ((player.verticalVelocity == 0 || player.ladderFlag == 1) &&
			checkInRange(player.playerX, ladder[i].hitbox.left, ladder[i].hitbox.right) == 0 &&
			checkLadderPlayerY(player, ladder[i]) == 0)
		{
			return 0;
		}
	}

	return 1;
}


void enterLadder(player_t* player, double modifier)
{
	int value = 10 * int(modifier);

	player->ladderFlag = 1;
	player->playerY += value;
	updatePlayerHitbox(player, 0, value);
}


int checkLadderPlayerY(player_t player, ladder_t ladder)
{
	if (checkInRange(player.playerY, ladder.hitbox.top, ladder.hitbox.bottom) == 0 ||
		checkInRange(player.hitbox.top, ladder.hitbox.top, ladder.hitbox.bottom) == 0 ||
		checkInRange(player.hitbox.bottom, ladder.hitbox.top, ladder.hitbox.bottom) == 0)
		return 0;

	else
		return 1;
}


int checkInRange(int x, int smaller, int greater) //0 jesli tak 1 jesli nie
{
	if (x >= smaller && x <= greater)
		return 0;

	else
		return 1;
}


void physics(player_t* player, platform_t* platform, ladder_t* ladder, level_t level, double delta)
{
	if (player->ladderFlag == 1 && isOnLadder(*player, ladder, level) != 0) //check if still on ladder
		player->ladderFlag = 0;

	if (player->ladderFlag == 0)
	{
		if (detectTopCollisionY(*player, platform, level) == 1) //make him bounce back from the top
			player->verticalVelocity = 0;
	}

	if (player->ladderFlag == 0)
	{
		if (detectColissionX(player, platform, level, delta) == 1)
			player->horizontalVelocity = 0;
	}

	gravity(player, platform, level);
}


void gravity(player_t* player, platform_t* platform, level_t level)
{
	int colissionFlag = detectBottomColissionY(*player, platform, level);

	if (player->ladderFlag == 0 && colissionFlag == 0)
		player->verticalVelocity += 1;

	if (colissionFlag == 1)
	{
		player->verticalVelocity = 0;
		player->ladderFlag = 0;
	}
}


int detectBottomColissionY(player_t player, platform_t* platform, level_t level)
{
	for (int i = 0; i < level.platformCount; i++)
	{
		int L = platform[i].hitbox.left, R = platform[i].hitbox.right;

		if (player.hitbox.bottom != platform[i].hitbox.top)
			continue;

		else if (checkInRange(player.hitbox.left, L, R) == 1 && checkInRange(player.hitbox.right, L, R) == 1 && checkInRange(player.playerX, L, R) == 1)
			continue;

		else
		{
			return 1;
		}
	}

	return 0;
}


int detectTopCollisionY(player_t player, platform_t* platform, level_t level)
{
	for (int i = 0; i < level.platformCount; i++)
	{
		int L = platform[i].hitbox.left, R = platform[i].hitbox.right;

		if (player.hitbox.top != platform[i].hitbox.bottom)
			continue;

		else if (checkInRange(player.hitbox.left, L, R) == 1 && checkInRange(player.hitbox.right, L, R) == 1 && checkInRange(player.playerX, L, R) == 1)
			continue;

		else
		{
			return 1;
		}
	}

	return 0;
}


int detectColissionX(player_t* player, platform_t* platform, level_t level, double delta)
{
	int playerPos, distance;
	double modifier;

	if (player->horizontalVelocity < 0)
	{
		modifier = -1.0;
		playerPos = player->hitbox.left;
	}

	else if (player->horizontalVelocity >= 0)
	{
		modifier = 1.0;
		playerPos = player->hitbox.right;
	}

	else
	{
		perror("Error - unknown side");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < level.platformCount; i++)
	{
		int T = platform[i].hitbox.top, B = platform[i].hitbox.bottom,  L = platform[i].hitbox.left, R = platform[i].hitbox.right;

		if (player->horizontalVelocity < 0)
		{
			distance = R - playerPos;
		}

		else if (player->horizontalVelocity >= 0)
		{
			distance = L - playerPos;
		}

		if(checkInRange(player->playerY, T, B) == 0 || checkInRange(player->hitbox.top, T, B) == 0) //|| checkInRange(player->hitbox.bottom, T, B) == 0)
		{ 
			if (checkInRange(playerPos + modifier * player->horizontalVelocity * delta, L, R) == 0)
			{
				updatePlayerPosition(player, distance, 0);
				return 1;
			}
		}
	}

	return 0;
}