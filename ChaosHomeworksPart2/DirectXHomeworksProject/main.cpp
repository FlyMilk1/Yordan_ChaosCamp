#include "DXRenderer.h"
#include <string>

int main() {
	DXRenderer renderer;
	FLOAT color[] = {1,0,0,1};
	renderer.render(color);
	std::cout << "Success!";
}