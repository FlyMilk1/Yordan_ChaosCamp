#include "Engine.h"
Engine::Engine(MainWindow* mainWindow, const DXRenderer& renderer) : window(mainWindow), renderer(renderer)  {}

void Engine::update() { 
	++frameIndex;
	FLOAT frameColorRGB[3];
	getFrameColor(frameIndex, frameColorRGB);
	FLOAT frameColor[4] = {
		frameColorRGB[0],
		frameColorRGB[1],
		frameColorRGB[2],
		1.0f
	};

	QImage frame = renderer.renderFrame(frameColor, FALSE);
	window->fillFrame(frame);
	secondsPassedFromLastFrame.stop();
	if (secondsPassedFromLastFrame.getDurationMilli().count() >= 1'000) {
		window->setFPSCounter(frameIndex);
		frameIndex = 0;
		secondsPassedFromLastFrame.start();
	}
}

void Engine::start()
{
	secondsPassedFromLastFrame.start();
}

void Engine::getFrameColor(int i, float out[3]) {
	// Use sine waves to smoothly cycle R, G, B over frames
	float speed = 0.02f; // smaller = slower cycling
	out[0] = 0.5f + 0.5f * sinf(i * speed + 0.0f);       // Red
	out[1] = 0.5f + 0.5f * sinf(i * speed + 2.094f);     // Green (120° phase)
	out[2] = 0.5f + 0.5f * sinf(i * speed + 4.188f);     // Blue (240° phase)
}
