#include "stage.h"
#include "world.h"



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
	framebuffer.fill(Color::CYAN);
}

void PlayStage::Update(float seconds_elapsed) {

}

void EndStage::Render(Image& framebuffer) {
	framebuffer.fill(Color::PURPLE);
}

void EndStage::Update(float seconds_elapsed) {

}
