#include "DXRenderer.h"
#include <string>

int main() {
	DXRenderer renderer;
	FLOAT color[] = {1.0f,0.5f,0.0f,1.0f};
	renderer.prepareForRendering();
	for (int i = 0; i < 1'000; ++i) {
		renderer.renderFrame(color, FALSE);
	}
	renderer.cleanUp();
	std::cout << "Success!";
	return 0;
}