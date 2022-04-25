#include "game.h"
#include "utils.h"
#include "image.h"
#include "world.h"
#include "textparser.h"

const int cellSize = 8;

//example of parser of .map from rogued editor
GameMap* loadGameMap(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL) //file not found
		return NULL;

	sMapHeader header; //read header and store it in the struct
	fread(&header, sizeof(sMapHeader), 1, file);
	assert(header.bytes == 1); //always control bad cases!!


	//allocate memory for the cells data and read it
	unsigned char* cells = new unsigned char[header.w*header.h];
	fread(cells, header.bytes, header.w*header.h, file);
	fclose(file); //always close open files
	//create the map where we will store it
	GameMap* map = new GameMap(header.w, header.h);

	for (int x = 0; x < map->width; x++)
		for (int y = 0; y < map->height; y++) {
			map->getCell(x, y).type = (eCellType)cells[x + y * map->width];
			//take layer values
			sCell& cell = map->getCell(x, y);
			Vector2i pos = Vector2i(x, y);
			switch (cell.type) {
			case START: map->spawnPoint = pos; break;
			case TOTEM: map->totemPoint = pos; break;
			case ENDCELL: map->winPoint = pos; break;
			case DOOR: map->doorPoint = pos; break;
			case MARK: map->markPoint = pos; break;
			case ENEMY: map->enemySpawnPoints.push_back(pos); break;
			case TRAP: map->trapPoints.push_back(pos); break;
			}
		}

	delete[] cells; //always free any memory allocated!

	return map;
};

void renderGameMap(Image& framebuffer, Image tileset, GameMap* map, Vector2 camOffset) {
	//size in pixels of a cell, we assume every row has 16 cells so the cell size must be image.width / 16
	int cs = tileset.width / 16;
	//for every cell
	for (int x = 0; x < map->width; ++x)
		for (int y = 0; y < map->height; ++y)
		{
			//get cell info
			sCell& cell = map->getCell(x, y);
			if (cell.type == 0) //skip empty
				continue;
			int type = (int)cell.type;
			//compute tile pos in tileset image
			int tilex = (type % 16) * cs; 	//x pos in tileset
			int tiley = floor(type / 16) * cs;	//y pos in tileset
			Area area(tilex, tiley, cs, cs); //tile area
			int screenx = (x * cs)-camOffset.x;//place offset here if you want
			int screeny = (y * cs)-camOffset.y;
			//avoid rendering out of screen stuff
			if (screenx < -cs || screenx > (int)framebuffer.width ||
				screeny < -cs || screeny > (int)framebuffer.height)
				continue;
			//draw region of tileset inside framebuffer
			framebuffer.drawImage(tileset, 		//image
				screenx, screeny, 	//pos in screen
				area); 		//area
		}
};

void loadLevel(GameMap* map, sPlayer* player, Sprite* totem) { //first time that enter o restart level...
	
	map->opened = false;

	Vector2 playerPos = CellToWorld(map->spawnPoint, cellSize);
	Vector2 totemPos = CellToWorld(map->totemPoint, cellSize);
	player -> position = playerPos;
	player -> target = playerPos;

	totem -> position = totemPos;
	totem -> target = totemPos;

	for (int i = 0; i < map->enemySpawnPoints.size(); i++)
	{

	}
};


Vector2i WorldToCell(Vector2 worldPos, int cellsize) {
	return worldPos / cellsize;
};

Vector2 CellToWorld(Vector2i cellPos, int cellsize) { //esquina superior izquierda
	return cellPos * cellSize;
}

Vector2 CellToWorldCenter(Vector2i cellPos, int cellsize) { //center

	return (cellPos * cellSize) + Vector2i(cellsize/2,cellsize/2) ;

}

Vector2 computeCamera(Vector2 playerPos, Vector2 playerToCam, int w, int h) {

	Vector2 camera = playerPos + playerToCam;
	//avoid out of screen bounds
	if (camera.x < 0) { camera.x = 0; }
	if (camera.y < 0) { camera.y = 0; }
	if (camera.x > w) { camera.x = w; }
	if (camera.y > h) { camera.y = h; }

	return camera;
}

bool isTotem(Vector2 worldPos, Vector2 totemPos) {

	float distance = worldPos.distance(totemPos);
	float maxDistance = 6.0f;

	return  distance <= maxDistance;
}
bool isDeath(Vector2 worldPos, GameMap* layer) {
	Vector2i cellCoord = WorldToCell(worldPos, cellSize);

	return layer->getCell(cellCoord.x, cellCoord.y).type == DEATH || layer->getCell(cellCoord.x, cellCoord.y).type == TRAP;
};

Vector2 totemLogic(Sprite* totem, sPlayer* player) {

	Vector2 movement;
	float totemSpeed = (float)cellSize;

	if (player->position.x < totem->position.x && player->dir == RIGHT) {
		
		movement.x += totemSpeed;
	}
	if (player->position.x >= totem->position.x && player->dir == LEFT) {

		movement.x -= totemSpeed;
	}
	if (player->position.y < totem->position.y && player->dir == DOWN) {

		movement.y += totemSpeed;
	}
	if (player->position.x >= totem->position.x && player->dir == UP) {
		movement.y -= totemSpeed;
	}
	Vector2 target = totem->position + movement;
	return target;


}

bool openDoor(Sprite* totem, GameMap* layer, GameMap* map) {
	if (!layer->opened) {
		Vector2i totemCell = WorldToCell(totem->position, cellSize);
		int distance = totemCell.distance(layer->markPoint);
		if (distance < 2) {
			layer->opened = true;
			//falta cambiar puerta por suelo
			map->getCell(layer->doorPoint.x, layer->doorPoint.y).type = (eCellType)20;
			return true;
		}
	}
	return false;
}

Vector2 callTotem(Sprite* totem, sPlayer* player) {
	//codigo repetido
	float distance = player->position.distance(totem->position);
	float maxDistance = 20.0f;
	if (distance < maxDistance) {
		Vector2 movement;
		float totemSpeed = abs(distance - 10.0f);

		if (player->position.x < totem->position.x && player->dir == RIGHT) {

			movement.x -= totemSpeed;
		}
		if (player->position.x >= totem->position.x && player->dir == LEFT) {

			movement.x += totemSpeed;
		}
		if (player->position.y < totem->position.y && player->dir == DOWN) {

			movement.y -= totemSpeed;
		}
		if (player->position.x >= totem->position.x && player->dir == UP) {
			movement.y += totemSpeed;
		}
		Vector2 target = totem->position + movement;
		return target;
	}
	return totem->position;
	
}

bool isWin(Vector2 worldPos, GameMap* map) {
	Vector2i playerCell = WorldToCell(worldPos, cellSize);
	int distance = playerCell.distance(map->winPoint);
	if (distance < 2) {
		return true;
	}
	return false;
};

bool isValid(Vector2 worldPos, GameMap* layer) { //mejorable
	Vector2i cellCoord = WorldToCell(worldPos, cellSize);

	//poliza
	if (cellCoord.x < 0 || cellCoord.y < 0 || cellCoord.x >= layer->width || cellCoord.y >= layer->height)
	{

		return false;
	}
	
	return  layer->getCell(cellCoord.x, cellCoord.y).type == FLOOR || 
			layer->getCell(cellCoord.x, cellCoord.y).type == TOTEM ||
			layer->getCell(cellCoord.x, cellCoord.y).type == MARK ||
			layer->getCell(cellCoord.x, cellCoord.y).type == ENEMY ||
		(layer->getCell(cellCoord.x, cellCoord.y).type == DOOR && layer->opened);//con dos layers preguntar si es navegable o no

};

void collisionLogic(Vector2 target, GameMap* layer,sPlayer* object) {

	if (isValid(target, layer)) {
	object->target = target;
		}
	else if (isValid(Vector2(target.x, object->position.y), layer)) {
	object->target = Vector2(target.x, object->position.y);
		}
	else if (isValid(Vector2(object->position.x, target.y), layer)) {
	object->target = Vector2(object->position.x, target.y);
		}
};

void collisionLogic(Vector2 target, GameMap* layer, Sprite* object) {

	if (isValid(target, layer)) {
		object->target = target;
	}
	else if (isValid(Vector2(target.x, object->position.y), layer)) {
		object->target = Vector2(target.x, object->position.y);
	}
	else if (isValid(Vector2(object->position.x, target.y), layer)) {
		object->target = Vector2(object->position.x, target.y);
	}
};

Vector2 EaseInOutSine(Vector2 a, Vector2 b, float t) {
	float n = -(cos(PI*t) - 1.0f) / 2.0f;

	return Vector2(n*(b.x - a.x), n*(b.y - a.y));

	/*usage example
	float lerpTime = 3.0f;
	float t = fmod(time,lerpTime) /lerTime;
	float min = 10.0f;
	float max = 30.0f;
	float easedT = EaseInOutSine(min,max,t);

	framebuffer.setPixel(min+easedT,10, Color::CYAN);
	*/
	
}

void lerp(Sprite* object, float time) {
	float lerpTime = object->lerpTime;
	float t = fmod(time, lerpTime) / lerpTime;
	Vector2 min = object->position;
	Vector2 max = object->target;
	Vector2 easedT = EaseInOutSine(min, max, t);

	object->position += easedT;
};


void sPlayer::Render( Image* framebuffer, float time, Vector2 camOffset) {

	int start_x = (int(time * this->animSpeed) % 4) * this->spriteWidth;
	start_x = this->isMoving ? start_x : 0;
	int start_y = (int)this->dir * this->spriteHeight;
	Vector2 playerRender = this->position - camOffset;
	framebuffer->drawImage(this->sprite, playerRender.x - this->spriteWidth/2 ,playerRender.y - this->spriteHeight/1.2, Area(start_x, start_y, this->spriteWidth, this->spriteHeight));

};

void Sprite::Render(Image* framebuffer, float time, Vector2 camOffset) {
	Vector2 spriteRender = this->position - camOffset;
	framebuffer->drawImage(this->sprite, spriteRender.x - this->spriteWidth/2, spriteRender.y - this->spriteHeight / 2);
}

int synthMusic::notesLength() {
	return sizeof(this->notes) / sizeof(this->notes[0]);
}

void synthMusic::playMelody() {
	int length = this->notesLength();
	int noteIndex = int(Game::instance->time*this->noteSpeed) % length;
	Game::instance->synth.osc1.setNote(this->notes[noteIndex]);
}


GameMap* GetMap(int id, std::vector<GameMap*> &maps) { return maps[id]; };
GameMap* GetCurrentMap(int currentMap, std::vector<GameMap*> &maps) { return GetMap(currentMap, maps); };
void SetMap(int id, int &currentMap) { currentMap = id; };

void InitMaps(std::vector<GameMap*>& maps, char* filename) {

	GameMap* map;
	TextParser tp;
	if (tp.create(filename) == false)
		std::cout << "database file not found" << std::endl;

	//while there are words to read
	char* w;
	while (w = tp.getword()) //returns null at end
	{
		map = loadGameMap(w);
		maps.push_back(map);
		tp.nextline();

	}

}
void World::loadWorld() {
	//constants
	playerToCam = Vector2(-Game::instance->framebuffer_width / 2, -Game::instance->framebuffer_height / 2); //cam centered on screen
	//load sprites
	font.loadTGA("data/bitmap-font-white.tga"); //load bitmap-font image
	minifont.loadTGA("data/mini-font-white-4x6.tga"); //load bitmap-font image
	intro.loadTGA("data/intro.tga");
	tutorial.loadTGA("data/tutorial.tga");
	end.loadTGA("data/end.tga");

	totem.sprite.loadTGA("data/totem.tga");
	enemy.sprite.loadTGA("data/enemy.tga");
	player.sprite.loadTGA("data/astronaut.tga"); //example to load an sprite

	//load map/tileset/stages example
	tileset.loadTGA("data/tileset.tga");
	InitMaps(maps, "data/levels_db.txt");
	InitMaps(maps_layer, "data/levels_layer_db.txt");
	InitStages(stages);

	//init position on map 0 
	loadLevel(maps_layer[0], &player, &totem);

}