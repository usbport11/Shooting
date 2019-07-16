#include "stdafx.h"
#include "AnimatedObject.h"

MAnimatedObject::MAnimatedObject():MObject()
{
	AnimationsCount = 0;
	FramesCount = 0;
	CurrentAnimation = 0;
	CurrentFrame = 0;
	CurrentAnimationTypeNumber = 0;
	AnimationEnabled = false;
}

MAnimatedObject::~MAnimatedObject()
{
	AnimationsCount = 0;
	FramesCount = 0;
	CurrentAnimation = 0;
	CurrentFrame = 0;
	CurrentAnimationTypeNumber = 0;
	AnimationEnabled = false;
	AnimationTypes.clear();
}

bool MAnimatedObject::SetAnimations(unsigned int inAnimationsCount, unsigned int inFramesCount)
{
	if(!inAnimationsCount || inAnimationsCount > 24) 
	{
		LogFile<<"Wrong animations count parameter"<<endl;
		return false;
	}
	if(!inFramesCount || inFramesCount > 24)
	{
		LogFile<<"Wrong frames count parameter"<<endl;
		return false;
	}
	AnimationsCount = inAnimationsCount;
	FramesCount = inFramesCount;
	UVUnit = glm::vec2((float)1/FramesCount, (float)1/AnimationsCount);
	SetQuad(UV, CurrentFrame * UVUnit[0], CurrentAnimation * UVUnit[1], UVUnit[0], UVUnit[1]);
	SetUV(UV);
	AnimationTypes.insert(AnimationTypes.begin(), inAnimationsCount, ANMT_NONE);
	return SetCurrentAnimation(0);
}

bool MAnimatedObject::SetCurrentAnimation(unsigned int inCurrentAnimation)
{
	if(inCurrentAnimation >= AnimationsCount)
	{
		LogFile<<"Wrong current animation parameter"<<endl;
		return false;
	}
	CurrentAnimation = inCurrentAnimation;
	return SetCurrentFrame(0);
}

bool MAnimatedObject::SetCurrentFrame(unsigned int inCurrentFrame)
{
	if(inCurrentFrame >= FramesCount)
	{
		LogFile<<"Wrong current frame parameter"<<endl;
		return false;
	}
	CurrentFrame = inCurrentFrame;
	SetQuad(UV, CurrentFrame * UVUnit[0], CurrentAnimation * UVUnit[1], UVUnit[0], UVUnit[1]);
	SetUV(UV);
	if(!DirectionRight) FlipTextureByX();
	return true;
}

bool MAnimatedObject::SetAnimationType(unsigned int inAnimationNumber, unsigned char inAnimationType)
{
	cout<<AnimationTypes.size()<<" "<<inAnimationNumber<<endl;
	if(inAnimationNumber >= AnimationTypes.size())
	{
		LogFile<<"Wrong animation number parameter"<<endl;
		return false;
	}
	if(inAnimationType > ANMT_RANDOM || !inAnimationType)
	{
		LogFile<<"Wrong current type parameter"<<endl;
		return false;
	}
	AnimationTypes[inAnimationNumber] = inAnimationType;
	return true;
}

void MAnimatedObject::IncreaseAnimationFrame()
{
	if(!AnimationEnabled || !AnimationsCount || !AnimationTypes[CurrentAnimation]) return;
	CurrentFrame ++;
	switch(AnimationTypes[CurrentAnimation])
	{
		case ANMT_LOOP:
			if(CurrentFrame >= FramesCount) CurrentFrame = 0;
			break;
		case ANMT_ONEWAY:
			if(CurrentFrame >= FramesCount)
			{
				OnAnimationEnd();
				CurrentFrame = FramesCount - 1;
			}
			break;
	}
	SetQuad(UV, CurrentFrame * UVUnit[0], CurrentAnimation * UVUnit[1], UVUnit[0], UVUnit[1]);
	SetUV(UV);
	if(!DirectionRight) FlipTextureByX();//will change with delay by time (250 msec)
}

unsigned char MAnimatedObject::GetCurrentAnimation()
{
	return CurrentAnimation;
}

void MAnimatedObject::StartAnimation()
{
	AnimationEnabled = true;
}

void MAnimatedObject::StopAnimation()
{
	AnimationEnabled = false;
}

void MAnimatedObject::OnAnimationEnd(){}
