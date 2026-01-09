#include "SnakeApp.h"
SnakeApp::SnakeApp() {}

void SnakeApp::onIdleTick() { 
	++frameIndex;

	QImage frame = renderer.renderFrame(frameData, FALSE);
	//mainWindow->updateViewport(frame);
}

void SnakeApp::onCameraPan(const QPoint& deltaFromStart,
	const QPoint& deltaFromLast)
{
	frameData.offsetX += deltaFromLast.x() * 0.002;
	frameData.offsetY += -deltaFromLast.y() * 0.002;
}

void SnakeApp::switchRenderingMode()
{
	isUsingRayTracing = !isUsingRayTracing;
	idleTimer->stop();
	fpsTimer->stop();
	renderer.prepareForRendering(mainWindow->getRenderFrame(), isUsingRayTracing);
	idleTimer->start(FPS_Uncapped);
	fpsTimer->start(1'000);
}

bool SnakeApp::init()
{
	if (false == initWindow()) {
		return false;
	}
	mainWindow->show();

	renderer = DXRenderer();

	const QLabel* renderFrame = mainWindow->getRenderFrame();

	renderer.prepareForRendering(renderFrame, isUsingRayTracing);

	idleTimer = new QTimer(mainWindow);
	QObject::connect(idleTimer, &QTimer::timeout, this, &SnakeApp::onIdleTick);
	idleTimer->start(FPS_Uncapped);

	fpsTimer = new QTimer(mainWindow);
	QObject::connect(fpsTimer, &QTimer::timeout, this, &SnakeApp::updateRenderStats);
	fpsTimer->start(1'000);
	
	movementTimer = new QTimer(mainWindow);
	QObject::connect(movementTimer, &QTimer::timeout, [this]() {
		this->mainWindow->checkMoveInput();
		});
	movementTimer->start(FPS_144);

	QObject::connect(mainWindow, &MainWindow::viewportDrag, this, &SnakeApp::onCameraPan);

	QObject::connect(mainWindow, &MainWindow::switchRenderingModeSignal, this, &SnakeApp::switchRenderingMode);

	movement = std::make_unique<Movement>(&renderer);

	QObject::connect(mainWindow, &MainWindow::moveSignal, [this](const MovementInput& input) {
		this->movement->updatePosition(input);
		});
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
