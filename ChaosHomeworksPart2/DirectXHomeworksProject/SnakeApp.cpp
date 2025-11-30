#include "SnakeApp.h"
SnakeApp::SnakeApp() {}

void SnakeApp::onIdleTick() { 
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
	//mainWindow->updateViewport(frame);
}

bool SnakeApp::init()
{
	if (false == initWindow()) {
		return false;
	}
	mainWindow->show();

	renderer = DXRenderer();

	const QLabel* renderFrame = mainWindow->getRenderFrame();

	renderer.prepareForRendering(renderFrame);

	idleTimer = new QTimer(mainWindow);
	QObject::connect(idleTimer, &QTimer::timeout, this, &SnakeApp::onIdleTick);
	idleTimer->start(FPS_Uncapped);

	fpsTimer = new QTimer(mainWindow);
	QObject::connect(fpsTimer, &QTimer::timeout, this, &SnakeApp::updateRenderStats);
	fpsTimer->start(1'000);

	return true;
}

void SnakeApp::getFrameColor(int i, float out[3]) {
	// Use sine waves to smoothly cycle R, G, B over frames
	float speed = 0.02f; // smaller = slower cycling
	out[0] = 0.5f + 0.5f * sinf(i * speed + 0.0f);       // Red
	out[1] = 0.5f + 0.5f * sinf(i * speed + 2.094f);     // Green (120° phase)
	out[2] = 0.5f + 0.5f * sinf(i * speed + 4.188f);     // Blue (240° phase)
}

void SnakeApp::updateRenderStats()
{
	mainWindow->setFPSCounter(frameIndex);
	frameIndex = 0;
}

bool SnakeApp::initWindow()
{
	mainWindow = new MainWindow();
	
	return true;
}
