#pragma once
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
