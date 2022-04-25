#include "stage.h"
#include "world.h"
#include "game.h"
#include "input.h"

//STAGE UTIL FUNCTIONS

Stage* GetStage(STAGE_ID id, std::vector<Stage*>& stages) { return stages[(int)id]; };
Stage* GetCurrentStage(STAGE_ID currentStage, std::vector<Stage*>& stages) { return GetStage(currentStage, stages); };
void SetStage(STAGE_ID id, STAGE_ID &currentStage) { currentStage = id; };



void InitStages(std::vector<Stage*>& stages) {
	stages.reserve(4);
	stages.push_back(new IntroStage());
	stages.push_back(new TutorialStage());
	stages.push_back(new PlayStage());
	stages.push_back(new EndStage());
}

//INTRO STAGE

void IntroStage::Render(Image& framebuffer) {
	framebuffer.drawImage(Game::instance->world.intro, 0, 0);
	framebuffer.drawText("SUSHI SAVIOR", 40, 10, Game::instance->world.font);
	if (int(Game::instance->time) % 2 == 0) {
		framebuffer.drawText("Press F to continue", 35, 100, Game::instance->world.minifont, 4, 6);
	}
}


void IntroStage::Update(float seconds_elapsed) {
	if (Input::wasKeyPressed(SDL_SCANCODE_F))
	{
		Game::instance->world.currentStage = TUTORIAL;
		Game::instance->synth.playSample("data/hit.wav", 1, false);
	}
}

//TUTORIAL STAGE

void TutorialStage::Render(Image& framebuffer) {

	framebuffer.drawImage(Game::instance->world.tutorial, 0, 0);
	if (int(Game::instance->time) % 2 == 0) {
		framebuffer.drawText("Press F to continue", 35, 110, Game::instance->world.minifont, 4, 6);
	}

}

void TutorialStage::Update(float seconds_elapsed) {
	if (Input::wasKeyPressed(SDL_SCANCODE_F))
	{
		Game::instance->world.currentStage = PLAY;
		Game::instance->synth.osc1.amplitude = 0.5;
		Game::instance->synth.playSample("data/hit.wav", 1, false);
	}
}

//PLAY STAGE

void PlayStage::Render(Image& framebuffer) {
	Game* game = Game::instance;

	//calculate camera
	game->world.camOffset = computeCamera(game->world.player.position, game->world.playerToCam, game->framebuffer_width, game->framebuffer_height);
	//render map
	renderGameMap(framebuffer, game->world.tileset, GetCurrentMap(game->world.player.currentMap, game->world.maps), game->world.camOffset);
	//render enemies/totems
	game->world.player.position = game->world.player.target;

	//lerp?
	lerp(&(game->world.totem), game->time);

	//render totem
	game->world.totem.Render(&framebuffer, game->time, game->world.camOffset);
	//render player
	game->world.player.Render(&framebuffer, game->time, game->world.camOffset);
	//render enemies
	GameMap* currentLayer = GetCurrentMap(game->world.player.currentMap, game->world.maps_layer);
	for (int i = 0; i < currentLayer->enemies.size(); i++){
		currentLayer->enemies[i]->Render(&framebuffer, game->time, game->world.camOffset);
	}
	//Attract available
	if (game->world.player.attractAvailable) {
		framebuffer.drawRectangle(135, 5, 20, 20, Color::GRAY);
		framebuffer.drawText("X", 142.5, 10, Game::instance->world.font);
	}
	
}

void PlayStage::Update(float seconds_elapsed) {
	Game* game = Game::instance;
	sPlayer* player = &(game->world.player);
	Sprite* totem = &(game->world.totem);
	Vector2 movement;
	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (Input::isKeyPressed(SDL_SCANCODE_W)) //up
	{
		movement.y -= player->moveSpeed;
		player->dir = PLAYER_DIR::UP;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S)) //down
	{
		movement.y += player->moveSpeed;
		player->dir = PLAYER_DIR::DOWN;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D)) //right
	{
		movement.x += player->moveSpeed;
		player->dir = PLAYER_DIR::RIGHT;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_A)) //left
	{
		movement.x -= player->moveSpeed;
		player->dir = PLAYER_DIR::LEFT;
	}
	//enemies movement
	
	Vector2 target = player->position + movement * seconds_elapsed;
	GameMap* currentMap = GetCurrentMap(game->world.player.currentMap, game->world.maps);
	GameMap* currentLayer = GetCurrentMap(game->world.player.currentMap, game->world.maps_layer);

	for (int i = 0; i < currentLayer->enemies.size(); i++) {
		enemiesMovement(currentLayer->enemies[i], game->world.player, seconds_elapsed);
	}

	//win condition
	if (isWin(player->position, currentLayer)) {
		int nextMapIndex = (player->currentMap + 1);
		if (nextMapIndex < game->world.maps.size()) {
			SetMap(nextMapIndex, player->currentMap);
			loadLevel(GetCurrentMap(game->world.player.currentMap, game->world.maps_layer), player, totem);
		}
		else {
			Game::instance->world.currentStage = END;
		}
	}
	
	//Death condition
	else if (isDeath(target, currentLayer) || isEnemy(target,currentLayer)) {
		Game::instance->synth.playSample("data/die.wav", 1, false);
		currentMap->getCell(currentLayer->doorPoint.x, currentLayer->doorPoint.y).type = (eCellType)11;
		loadLevel(currentLayer, player, totem); 

	}
	else {
		//collisions
		if (isTotem(target, totem->position)) {
			Game::instance->synth.playSample("data/hit.wav", 1, false);
			Vector2 targetTotem = totemLogic(totem, player);
			collisionLogic(targetTotem, currentLayer, totem);
			if (openDoor(totem, currentLayer, currentMap)){
				Game::instance->synth.playSample("data/win.wav", 1, false);
			};
		}
		else {
			collisionLogic(target, currentLayer, player);
			};
		//update movement
		player->isMoving = player->position.x != player->target.x || player->position.y != player->target.y;

		//oscilator
		Game::instance->world.music.playMelody();

		//attract totem to player
		float distance = player->position.distance(totem->position);
		float maxDistance = 20.0f;
		if (distance < maxDistance) {

			player->attractAvailable = true;
			if (Input::wasKeyPressed(SDL_SCANCODE_X))
			{

				float totemSpeed = abs(distance - 10.0f);
				Vector2 targetTotem = callTotem(totem, player, totemSpeed);
				Game::instance->synth.playSample("data/attract.wav", 1, false);
				collisionLogic(targetTotem, currentLayer, totem);

			}
		
		}
		else {
			player->attractAvailable = false;
		}
	};
}

//END STAGE

void EndStage::Render(Image& framebuffer) {
	Game::instance->synth.osc1.amplitude = 0;
	framebuffer.drawImage(Game::instance->world.end, 0, 0);
	framebuffer.drawText("YOU WIN", 50, 10, Game::instance->world.font);
	if (int(Game::instance->time) % 2 == 0) {
		framebuffer.drawText("Press F to restart or ESC to close", 10, 110, Game::instance->world.minifont, 4, 6);
	}
}

void EndStage::Update(float seconds_elapsed) {
	if (Input::wasKeyPressed(SDL_SCANCODE_F))
	{
		Game* game = Game::instance;
		GameMap* currentLayer = GetCurrentMap(game->world.player.currentMap, game->world.maps_layer);
		sPlayer* player = &(game->world.player);
		Sprite* totem = &(game->world.totem);

		SetMap(0, player->currentMap);
		loadLevel(GetCurrentMap(game->world.player.currentMap, game->world.maps_layer), player, totem);
		Game::instance->world.currentStage = PLAY;
		Game::instance->synth.osc1.amplitude = 0.5;
	}
}
