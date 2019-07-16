#ifndef animatedobjectH
#define animatedobjectH

#define ANMT_NONE 0
#define ANMT_LOOP 1
#define ANMT_ONEWAY 2
#define ANMT_RANDOM 3

#include "Object.h"

class MAnimatedObject: public MObject
{
protected:
	bool AnimationEnabled;
	unsigned int AnimationsCount;//count by Y
	unsigned int FramesCount;//count by X
	unsigned int CurrentAnimation;//offset by Y
	unsigned int CurrentFrame;//offset by X
	unsigned int CurrentAnimationTypeNumber;
	vector<unsigned char> AnimationTypes;
	glm::vec2 UVUnit;
	stQuad UV;
public:
	MAnimatedObject();
	~MAnimatedObject();
	bool SetAnimations(unsigned int AnimationsCount, unsigned int FramesCount);
	bool SetCurrentAnimation(unsigned int inCurrentAnimation);
	bool SetCurrentFrame(unsigned int inCurrentFrame);
	bool SetAnimationType(unsigned int inAnimationNumber, unsigned char inAnimationType);
	void IncreaseAnimationFrame();
	unsigned char GetCurrentAnimation();
	void StartAnimation();
	void StopAnimation();
	virtual void OnAnimationEnd();
};

#endif
