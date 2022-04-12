#pragma once
class Stage
{

};
class PlayStage
{

};

class World
{

};
class Sprite
{

};

enum PLAYER_DIR {
	DOWN = 0,
	RIGHT = 1,
	LEFT = 2,
	UP = 3
};

struct sPlayer {
	Vector2 position;
	bool isMoving;
	PLAYER_DIR dir;
};

//MAP FUNCTIONS

enum eCellType : uint8 {
	EMPTY, START,
	WALL,
	DOOR,
	CHEST
};

enum eItemType : uint8 {
	NOTHING,
	SWORD,
	POTION
};

struct sCell {
	eCellType type;
	eItemType item;
};

class GameMap {
public:
	int width;
	int height;
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

struct sMapHeader {
	int w; //width of map
	int h; //height of map
	unsigned char bytes; //num bytes per cell
	unsigned char extra[7]; //filling bytes, not used
};

