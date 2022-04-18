#include "stage.h"
#include "world.h"


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
	framebuffer.fill(Color::CYAN);
}

void PlayStage::Update(float seconds_elapsed) {

}

void EndStage::Render(Image& framebuffer) {
	framebuffer.fill(Color::PURPLE);
}

void EndStage::Update(float seconds_elapsed) {

}
