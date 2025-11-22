#include <string>
#include "Engine.h"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	MainWindow window;
	window.show();

	DXRenderer renderer;

	const QLabel* renderFrame = window.getRenderFrame();
	
	renderer.prepareForRendering(renderFrame);

	Engine engine(&window, renderer);

	engine.start();
	QTimer* timer = new QTimer(&app);
	QObject::connect(timer, &QTimer::timeout, [&engine]() {
		engine.update();
	});

	timer->start(0);

	return app.exec();
}