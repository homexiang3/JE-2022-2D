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
	}
}

//TUTORIAL STAGE

void TutorialStage::Render(Image& framebuffer) {

	Image minifont = Game::instance->world.minifont;
	framebuffer.fill(Color::BLUE);
	framebuffer.drawText("While you were travelling on the", 12, 10, minifont, 4, 6);
	framebuffer.drawText("sushi galaxy you received a help", 12, 25, minifont, 4, 6);
	framebuffer.drawText("message from the maki planet.", 15, 40, minifont, 4, 6);
	framebuffer.drawText("The autoproclamated soy sauce king", 10, 60, minifont, 4, 6);
	framebuffer.drawText("challenge you on a puzzle game.", 15, 75, minifont, 4, 6);
	framebuffer.drawText("conquer the trials and make him quit.", 8, 90, minifont, 4, 6);
}

void TutorialStage::Update(float seconds_elapsed) {
	if (Input::wasKeyPressed(SDL_SCANCODE_F))
	{
		Game::instance->world.currentStage = PLAY;
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
	renderSprite(&framebuffer, game->world.totem, game->world.camOffset);
	//render player
	game->world.player.renderPlayer( &framebuffer, game->time, game->world.sprite, game->world.camOffset);

	
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
	//collisions
	Vector2 target = player->position + movement * seconds_elapsed;
	Vector2 oldPlayerPos = player->position;

	if (isTotem(target, totem->position)) {
		totemLogic(totem, player);
		openDoor(totem, GetCurrentMap(game->world.player.currentMap, game->world.maps));
	}

	if (isValid(target, GetCurrentMap(game->world.player.currentMap, game->world.maps))) {
		player->position = target;	
	}
	else if (isValid(Vector2(target.x,player->position.y), GetCurrentMap(game->world.player.currentMap, game->world.maps))) {
		player->position = Vector2(target.x, player->position.y);
	}
	else if (isValid(Vector2(player->position.x, target.y), GetCurrentMap(game->world.player.currentMap, game->world.maps))) {
		player->position = Vector2(player->position.x, target.y);
	}
	//update movement
	  //player->position += movement * seconds_elapsed;
		player->isMoving = oldPlayerPos.x != player->position.x || oldPlayerPos.y != player->position.y;
	//oscilator example
	    Game::instance->world.music.playMelody();


	//example of 'was pressed'DEBUG SOUNDS
	if (Input::wasKeyPressed(SDL_SCANCODE_F)) //if key F was pressed example sound
	{
		Game::instance->synth.playSample("data/hit.wav", 1, false);
	}

	/*to read the gamepad state
	if (Input::gamepads[0].isButtonPressed(A_BUTTON)) //if the A button is pressed
	{
	}

	if (Input::gamepads[0].direction & PAD_UP) //left stick pointing up
	{
		//bgcolor.set(0, 255, 0);
	}*/
}

//END STAGE

void EndStage::Render(Image& framebuffer) {
	Game::instance->synth.osc1.amplitude = 0;
	framebuffer.fill(Color::PURPLE);
}

void EndStage::Update(float seconds_elapsed) {
	if (Input::wasKeyPressed(SDL_SCANCODE_F))
	{
		Game::instance->must_exit = true;
	}
}
