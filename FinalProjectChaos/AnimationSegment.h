#pragma once
#include "vec3.h"
#include <vector>
class AnimationSegment {
public:
	AnimationSegment(const vec3& pos={0,0,0}, const float& pan=0, const float& tilt=0, const int& frame=0);
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
