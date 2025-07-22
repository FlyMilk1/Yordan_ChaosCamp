#include "AnimationSegment.h"

AnimationSegment::AnimationSegment(const vec3& pos, const float& pan, const float& tilt, const int& frame):position(pos), pan(pan), tilt(tilt), frame(frame){}

void AnimationSegment::calculateFrames(const AnimationSegment& previousSegment)
{
	//Linear interpolation
	int frames = frame - previousSegment.getFrame();
	vec3 posPerFrame = (position - previousSegment.getPosition()) / frames;
	float tiltPerFrame = (tilt - previousSegment.getTilt()) / frames;
	float panPerFrame = (pan - previousSegment.getPan()) / frames;
	for (int frameIdx = 0; frameIdx < frames; frameIdx++) {
		interpolationFromLast.emplace_back(posPerFrame, panPerFrame, tiltPerFrame, 0);
	}
	
}

float AnimationSegment::getPan() const
{
	return pan;
}

float AnimationSegment::getTilt() const
{
	return tilt;
}
vec3 AnimationSegment::getPosition() const
{
	return position;
}
int AnimationSegment::getFrame() const
{
	return frame;
}