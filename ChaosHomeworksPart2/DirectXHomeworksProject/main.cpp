#include <string>
#include "SnakeApp.h"
int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	SnakeApp snakeApp = {};

	snakeApp.init();
	
	return app.exec();
}