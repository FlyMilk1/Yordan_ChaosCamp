#pragma once
#include "vec3.h"
#include <vector>
class AnimationSegment {
public:
	AnimationSegment(const vec3& pos, const float& pan, const float& tilt, const int& frame);
	void calculateFrames(const AnimationSegment& previousSegment);
	float getPan()const;
	float getTilt()const;
	vec3 getPosition()const;
	int getFrame()const;
	std::vector<AnimationSegment> getInterpolation()const;
private:
	std::vector<AnimationSegment> interpolationFromLast;
	float pan;
	float tilt;
	vec3 position;
	int frame;
};
