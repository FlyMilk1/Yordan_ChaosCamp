#include "DXRenderer.h"
#include <string>

int main() {
	DXRenderer renderer;
	FLOAT color[] = {1.0f,0.5f,0.0f,1.0f};
	renderer.render(color);
	renderer.cleanUp();
	std::cout << "Success!";
	return 0;
}