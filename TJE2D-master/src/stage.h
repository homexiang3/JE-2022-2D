#ifndef STAGE_H
#define STAGE_H

#include "image.h"

enum STAGE_ID {
	INTRO = 0,
	TUTORIAL = 1,
	PLAY = 2,
	END = 3
};


class Stage {
public:
	virtual STAGE_ID GetId() = 0;
	virtual void Render(Image& framebuffer) = 0;
	virtual void Update(float seconds_elapsed) = 0;
};

class IntroStage : public Stage {
public:
	STAGE_ID GetId() { return STAGE_ID::INTRO; };
	void Render(Image& framebuffer);
	void Update(float seconds_elapsed);

};

class TutorialStage : public Stage {
public:
	STAGE_ID GetId() { return STAGE_ID::TUTORIAL; };
	void Render(Image& framebuffer);
	void Update(float seconds_elapsed);

};

class PlayStage : public Stage {
public:
	STAGE_ID GetId() { return STAGE_ID::PLAY; };
	void Render(Image& framebuffer);
	void Update(float seconds_elapsed);

};

class EndStage : public Stage {
public:
	STAGE_ID GetId() { return STAGE_ID::END; };
	void Render(Image& framebuffer);
	void Update(float seconds_elapsed);

};

#endif
