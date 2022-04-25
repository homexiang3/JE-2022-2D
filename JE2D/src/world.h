#ifndef WORLD_H
#define WORLD_H

#include "stage.h"
#include "includes.h"

//PLAYER FUNCTIONS

enum PLAYER_DIR {
	DOWN = 0,
	RIGHT = 1,
	LEFT = 2,
	UP = 3
};

class Sprite {
public:
	Vector2 position;
	Vector2 target;
	float lerpTime = 2.0f;
	Image sprite;
	int spriteWidth = 8;
	int spriteHeight = 12;
	float moveSpeed = 8.0f;

	virtual void Render(Image* framebuffer, float time, Vector2 camOffset);
};
class sPlayer :public Sprite {
public:
	Vector2 position;
	Vector2 target;
	float lerpTime = 2.0f;
	Image sprite;
	int spriteWidth = 14;
	int spriteHeight = 18;
	float moveSpeed = 50.0f;
	//player different variables
	bool isMoving = false;
	PLAYER_DIR dir = DOWN;
	bool attractAvailable = false;
	int currentMap = 0;
	float animSpeed = 4.0f;

	void Render(Image* framebuffer, float time, Vector2 camOffset);
};

//MAP FUNCTIONS

enum eCellType : uint8 {
	EMPTY, 
	FLOOR,
	WALL,
	START,
	MARK,
	DOOR,
	ENDCELL, 
	TOTEM,
	DEATH,
	TRAP,
	ENEMY
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
	bool opened = false;
	Vector2i spawnPoint;
	Vector2i markPoint;
	Vector2i doorPoint;
	Vector2i totemPoint;
	Vector2i winPoint;
	std::vector<Sprite*> enemies;
	std::vector<Vector2i> enemySpawnPoint;
	std::vector<Vector2i> trapPoints;

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



//CAMERA
Vector2 computeCamera(Vector2 playerPos, Vector2 playerToCam, int w, int h);


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
	Image intro;
	Image tutorial;
	Image end;
	Sprite totem;
	Sprite enemy;
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
	std::vector<GameMap*> maps_layer;
	//stages
	std::vector<Stage*> stages;
	STAGE_ID currentStage = STAGE_ID::INTRO;

	void loadWorld();

};
//Util functions

GameMap* GetMap(int id, std::vector<GameMap*> &maps);
GameMap* GetCurrentMap(int currentMap, std::vector<GameMap*> &maps);
void SetMap(int id, int &currentMap);
void InitMaps(std::vector<GameMap*> &maps, char* filename);

Vector2 EaseInOutSine(Vector2 a, Vector2 b, float t);
void lerp(Sprite* object, float time);
void loadLevel(GameMap* map, sPlayer* player, Sprite* totem);

Vector2i WorldToCell(Vector2 worldPos, int cellsize);
Vector2 CellToWorld(Vector2i cellPos, int cellsize);
Vector2 CellToWorldCenter(Vector2i cellPos, int cellsize);

bool isValid(Vector2 worldPos, GameMap* map);
bool isTotem(Vector2 worldPos, Vector2 totemPos);
Vector2 totemLogic(Sprite* totem, sPlayer* player);
bool openDoor(Sprite* totem, GameMap* layer, GameMap* map);
Vector2 callTotem(Sprite* totem, sPlayer* player, float totemSpeed);
bool isWin(Vector2 worldPos, GameMap* layer);
bool isDeath(Vector2 worldPos, GameMap* layer);
void collisionLogic(Vector2 target, GameMap* layer, sPlayer* object);
void collisionLogic(Vector2 target, GameMap* layer, Sprite* object);
bool isEnemy(Vector2 worldPos, GameMap* layer);
void enemiesMovement(Sprite* enemy, sPlayer player, float seconds_elapsed);

#endif