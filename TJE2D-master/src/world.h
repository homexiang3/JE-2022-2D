#ifndef WORLD_H
#define WORLD_H

#include "stage.h"
#include "includes.h"

//MAP FUNCTIONS

enum eCellType : uint8 {
	EMPTY, 
	START,
	WALL,
	DOOR,
	TOTEM,
	WATER, 
	FLOOR
};

enum eItemType : uint8 {
	NOTHING,
	//SWORD,
	//POTION
};

struct sCell {
	eCellType type;
	eItemType item;
};

struct sMapHeader {
	int w; //width of map
	int h; //height of map
	unsigned char bytes; //num bytes per cell
	unsigned char extra[7]; //filling bytes, not used
};

class GameMap {
public:
	int width;
	int height;

	Vector2i spawnPoint = Vector2i(8,8);
	Vector2i totemPoint = Vector2i(11, 11);
	Vector2i winPoint = Vector2i(12, 12);
	std::vector<Vector2i> enemySpawnPoints;

	sCell* data;

	GameMap()
	{
		width = height = 0;
		data = NULL;
	}

	GameMap(int w, int h)
	{
		width = w;
		height = h;
		data = new sCell[w*h];
	}

	sCell& getCell(int x, int y)
	{
		return data[x + y * width];
	}
	
};

GameMap* loadGameMap(const char* filename);
void renderGameMap(Image& framebuffer, Image tileset, GameMap* map, Vector2 camOffset);


Vector2 EaseInOutSine(Vector2 a, Vector2 b, float t);

//CAMERA
Vector2 computeCamera(Vector2 playerPos, Vector2 playerToCam, int w, int h);

//PLAYER FUNCTIONS

enum PLAYER_DIR {
	DOWN = 0,
	RIGHT = 1,
	LEFT = 2,
	UP = 3
};

class sPlayer {
public:
	Vector2 position;
	Vector2 target;
	float lerpTime = 3.0f;
	bool isMoving = false;
	PLAYER_DIR dir = DOWN;
	bool isDead = false;
	int currentMap = 0;
	//prev constants of game
	float moveSpeed = 50.0f;
	float animSpeed = 4.0f;
	int spriteWidth = 14;
	int spriteHeight = 18;

	void renderPlayer( Image* framebuffer, float time, Image sprite, Vector2 camOffset);
};

struct Sprite {
	Vector2 position;
	Vector2 target;
	float lerpTime = 3.0f;
	Image sprite;
	int spriteWidth = 8;
	int spriteHeight = 12;
};

void renderSprite(Image* framebuffer, Sprite sprite, Vector2 camOffset);

//Music class

class synthMusic {
public:
	int notes[5] = { 45,75,60,72,82 };
	float noteSpeed = 2.0f;
	int notesLength();

	void playMelody();
};

//WORLD

//CLASS WORLD
class World {
public:
	Image font;
	Image minifont;
	Image sprite;
	Image intro;
	Sprite totem;
	//Color bgcolor(130, 80, 100);
	//added player from world.h
	sPlayer player;
	Vector2 camOffset;
	Vector2 playerToCam;
	//constants of the game
	synthMusic music;
	//map functions
	Image tileset;
	std::vector<GameMap*> maps; //cargas fichero level_db.txt y metes todos los niveles en maps
	//stages
	std::vector<Stage*> stages;
	STAGE_ID currentStage = STAGE_ID::PLAY;

	void loadWorld();

};

GameMap* GetMap(int id, std::vector<GameMap*> &maps);
GameMap* GetCurrentMap(int currentMap, std::vector<GameMap*> &maps);
void SetMap(int id, int &currentMap);
void InitMaps(std::vector<GameMap*> &maps);

//We assume maps always starts at (0,0)
Vector2i WorldToCell(Vector2 worldPos, int cellsize);
Vector2 CellToWorld(Vector2i cellPos, int cellsize);
Vector2 CellToWorldCenter(Vector2i cellPos, int cellsize);

bool isValid(Vector2 worldPos, GameMap* map);
bool isTotem(Vector2 worldPos, Vector2 totemPos);
void totemLogic(Sprite* totem, sPlayer* player);
void openDoor(Sprite* totem, GameMap* map);
void callTotem(Sprite* totem, sPlayer* player);
bool isWin(Vector2 worldPos, GameMap* map);

#endif