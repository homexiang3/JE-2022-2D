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

void renderGameMap(Image& framebuffer, Image tileset, GameMap* map) {
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
			int screenx = x * cs; //place offset here if you want
			int screeny = y * cs;
			//avoid rendering out of screen stuff
			if (screenx < -cs || screenx > framebuffer.width ||
				screeny < -cs || screeny > framebuffer.height)
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

bool isValid(Vector2 worldPos) { //mejorable
	Vector2i cellCoord = WorldToCell(worldPos, cellSize);

	//poliza
	if (cellCoord.x < 0 || cellCoord.y < 0 || cellCoord.x >= Game::instance->world.map->width || cellCoord.y >= Game::instance->world.map->height)
	{

		return false;
	}
	
	return  Game::instance->world.map->getCell(cellCoord.x, cellCoord.y).type == 0;//con dos layers preguntar si es navegable o no

};

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

void renderPlayer(sPlayer& player, Image* framebuffer, float time, Image sprite) {

	int start_x = (int(time * player.animSpeed) % 4) * player.spriteWidth;
	start_x = player.isMoving ? start_x : 0;
	int start_y = (int)player.dir * player.spriteHeight;
	framebuffer->drawImage(sprite, player.position.x - player.spriteWidth/2, player.position.y - player.spriteHeight/1.2, Area(start_x, start_y, player.spriteWidth, player.spriteHeight));
	

};

int synthMusic::notesLength() {
	return sizeof(this->notes) / sizeof(this->notes[0]);
}

void synthMusic::playMelody() {
	int length = this->notesLength();
	int noteIndex = int(Game::instance->time*this->noteSpeed) % length;
	Game::instance->synth.osc1.setNote(this->notes[noteIndex]);
}

void World::loadWorld() {

	font.loadTGA("data/bitmap-font-white.tga"); //load bitmap-font image
	minifont.loadTGA("data/mini-font-white-4x6.tga"); //load bitmap-font image
	sprite.loadTGA("data/astronaut.tga"); //example to load an sprite
	intro.loadTGA("data/intro.tga");

	//read file example
		//std::string s;
		//readFile("data/test.txt", s);
		//std::cout << s << std::endl;

	//load map/tileset example
	tileset.loadTGA("data/tileset.tga");
	map = loadGameMap("data/mymap.map");
}