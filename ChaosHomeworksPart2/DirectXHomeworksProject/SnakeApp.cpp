#include "SnakeApp.h"
SnakeApp::SnakeApp() {}

void SnakeApp::onIdleTick() { 
	++frameIndex;

	QImage frame = renderer.renderFrame(frameData, FALSE);
	//mainWindow->updateViewport(frame);
}

void SnakeApp::onCameraPan(const QPoint& offsetFromStart)
{
	frameData.offsetX = offsetFromStart.x();
	frameData.offsetY = offsetFromStart.y();
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

	QObject::connect(mainWindow->getViewportLabel(), &ViewportLabel::drag, this, &SnakeApp::onCameraPan);

	return true;
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
