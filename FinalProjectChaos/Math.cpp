#include "Math.h"
float clampf(const float& input, const float& min, const float& max){
	if(input > max){
		return max;
	}
	else if(input < min){
		return min;
	}
	else{
		return input;
	}
}