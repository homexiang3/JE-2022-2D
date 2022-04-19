#include "stage.h"
#include "world.h"
#include "game.h"
#include "input.h"


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

void IntroStage::Render(Image& framebuffer) {
	framebuffer.fill(Color::RED);
}

void IntroStage::Update(float seconds_elapsed) {
	
}


void TutorialStage::Render(Image& framebuffer) {
	framebuffer.fill(Color::BLUE);
}

void TutorialStage::Update(float seconds_elapsed) {

}

void PlayStage::Render(Image& framebuffer) {
	Game* game = Game::instance;
	renderGameMap(framebuffer, game->world.tileset, game->world.map);
	renderPlayer(game->world.player, &framebuffer, game->time, game->world.sprite);
	
}

void PlayStage::Update(float seconds_elapsed) {
	Game* game = Game::instance;
	sPlayer* player = &(game->world.player);
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

	if (isValid(target)) {
		player->position = target;
	}
	else if (isValid(Vector2(target.x,player->position.y))) {
		player->position = Vector2(target.x, player->position.y);
	}
	else if (isValid(Vector2(player->position.x, target.y))) {
		player->position = Vector2(player->position.x, target.y);
	}

	//update movement
	  //player->position += movement * seconds_elapsed;
	player->isMoving = oldPlayerPos.x != player->position.x || oldPlayerPos.y != player->position.y;
	//oscilator example
	    Game::instance->world.music.playMelody();

	//example of 'was pressed'
	if (Input::wasKeyPressed(SDL_SCANCODE_F)) //if key F was pressed example sound
	{
		Game::instance->synth.playSample("data/hit.wav", 1, false);
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_Z)) //if key Z was pressed
	{
	}

	//to read the gamepad state
	if (Input::gamepads[0].isButtonPressed(A_BUTTON)) //if the A button is pressed
	{
	}

	if (Input::gamepads[0].direction & PAD_UP) //left stick pointing up
	{
		//bgcolor.set(0, 255, 0);
	}
}

void EndStage::Render(Image& framebuffer) {
	framebuffer.fill(Color::PURPLE);
}

void EndStage::Update(float seconds_elapsed) {

}
