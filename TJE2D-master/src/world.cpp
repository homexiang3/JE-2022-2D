#include "game.h"
#include "utils.h"
#include "image.h"
#include "world.h"

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
		for (int y = 0; y < map->height; y++)
			map->getCell(x, y).type = (eCellType)cells[x + y * map->width];

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
	Vector2i playerCell = WorldToCell(worldPos, cellSize);
	Vector2i totemCell = WorldToCell(totemPos, cellSize);
	return  playerCell == totemCell;
}

void totemLogic(Sprite* totem, sPlayer* player) {
	Vector2i totemCell = WorldToCell(totem->position, cellSize);
	Vector2i playerCell = WorldToCell(player->position, cellSize);


	if (playerCell.x < totemCell.x && player->dir == RIGHT) {
		
		totemCell.x += 1;
	}
	if (playerCell.x >= totemCell.x && player->dir == LEFT) {

		totemCell.x -= 1;
	}
	if (playerCell.y < totemCell.y && player->dir == DOWN) {

		totemCell.y += 1;
	}
	if (playerCell.y >= totemCell.y && player->dir == UP) {
		totemCell.y -= 1;
	}

	totem->position = CellToWorld(totemCell,cellSize);
	//int distance = playerCell.distance(totemCell);
	//std::cout << distance << std::endl;
}

bool isValid(Vector2 worldPos, GameMap* map) { //mejorable
	Vector2i cellCoord = WorldToCell(worldPos, cellSize);

	//poliza
	if (cellCoord.x < 0 || cellCoord.y < 0 || cellCoord.x >= map->width || cellCoord.y >= map->height)
	{

		return false;
	}
	
	return  map->getCell(cellCoord.x, cellCoord.y).type == 8 || map->getCell(cellCoord.x, cellCoord.y).type == 9;//con dos layers preguntar si es navegable o no

};

void openDoor(Sprite* totem, GameMap* map) {
	Vector2i totemCell = WorldToCell(totem->position, cellSize);
	
	if (map->getCell(totemCell.x, totemCell.y).type == 10) {
		std::cout << "win" << std::endl;
	}
}

float EaseInOutSine(float a, float b, float t) {
	float n = -(cos(PI*t) - 1.0f) / 2.0f;
	return n * (b - a);
	/*usage example
	float lerpTime = 3.0f;
	float t = fmod(time,lerpTime) /lerTime;
	float min = 10.0f;
	float max = 30.0f;
	float easedT = EaseInOutSine(min,max,t);

	framebuffer.setPixel(min+easedT,10, Color::CYAN);
	
	*/
}

void sPlayer::renderPlayer( Image* framebuffer, float time, Image sprite, Vector2 camOffset) {

	int start_x = (int(time * this->animSpeed) % 4) * this->spriteWidth;
	start_x = this->isMoving ? start_x : 0;
	int start_y = (int)this->dir * this->spriteHeight;
	Vector2 playerRender = this->position - camOffset;
	framebuffer->drawImage(sprite, playerRender.x - this->spriteWidth/2 ,playerRender.y - this->spriteHeight/1.2, Area(start_x, start_y, this->spriteWidth, this->spriteHeight));
	

};

void renderSprite(Image* framebuffer, Sprite sprite, Vector2 camOffset) {
	framebuffer->drawImage(sprite.sprite, sprite.position.x-camOffset.x, sprite.position.y-camOffset.y);
}

int synthMusic::notesLength() {
	return sizeof(this->notes) / sizeof(this->notes[0]);
}

void synthMusic::playMelody() {
	int length = this->notesLength();
	int noteIndex = int(Game::instance->time*this->noteSpeed) % length;
	Game::instance->synth.osc1.setNote(this->notes[noteIndex]);
}
void InitMaps(std::vector<GameMap*>& maps) {

	GameMap* map;

	std::string s;
	readFile("data/levels_db.txt", s);
	std::istringstream f(s);
	while (std::getline(f, s)) {
		const char * c = s.c_str();
		std::cout << c << std::endl;
		map = loadGameMap(c);
		maps.push_back(map);
	}
}
void World::loadWorld() {
	//constants
	playerToCam = Vector2(-Game::instance->framebuffer_width / 2, -Game::instance->framebuffer_height / 2);
	//load sprites
	font.loadTGA("data/bitmap-font-white.tga"); //load bitmap-font image
	minifont.loadTGA("data/mini-font-white-4x6.tga"); //load bitmap-font image
	sprite.loadTGA("data/astronaut.tga"); //example to load an sprite
	intro.loadTGA("data/intro.tga");
	totem.sprite.loadTGA("data/totem.tga");

	//read file example
		//std::string s;
		//readFile("data/test.txt", s);
		//std::cout << s << std::endl;

	//load map/tileset/stages example
	tileset.loadTGA("data/tileset.tga");
	InitMaps(maps);
	InitStages(stages);

}