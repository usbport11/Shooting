#include "stdafx.h"
#include "Bullet.h"

MBullet::MBullet():MPhysicObject()
{
	PircingLimit = 0;
	PircingValue = 0;
	RicochetLimit = 0;
	RicochetValue = 0;
	LiveLimit = 0;
	LiveValue = 0;
	
	Direction = glm::vec2(0, 0);
	Velocity = glm::vec2(0, 0);
	Collided = false;
}

MBullet::~MBullet()
{
	Direction = glm::vec2(0, 0);
	Velocity = glm::vec2(0, 0);
	Collided = false;
}

bool MBullet::AddVelocity(b2World* inpWorld, float DirectionX, float DirectionY, float VelocityX, float VelocityY)
{
	if((!DirectionX && !DirectionY) || (!VelocityX && !VelocityY))
	{
		LogFile<<"Bullet: NULL direction or velocity"<<endl;
		return false;
	}
	Direction = glm::vec2(DirectionX, DirectionY);
	Velocity = glm::vec2(VelocityX, VelocityY);
	if(!AddPhysics(inpWorld, b2_dynamicBody, true, true, OT_BULLET, OT_BOUNDARY | OT_ENEMY)) return false;
	Body->SetGravityScale(0.0f);
	Body->SetLinearVelocity(b2Vec2(Direction.x * Velocity.x, Direction.y * Velocity.y));
	
	return true;
}

void MBullet::SetPircing(unsigned int inPircingLimit)
{
	PircingLimit = inPircingLimit;
}

void MBullet::SetRicochet(unsigned int inRicochetLimit)
{
	RicochetLimit = inRicochetLimit;
}

void MBullet::SetLive(unsigned int inLiveLimit)
{
	LiveLimit = inLiveLimit;
}

void MBullet::Fly()
{
	if(LiveLimit)
	{
		LiveValue ++;
		if(LiveLimit <= LiveValue) Collided = true;
		return;
	}
	b2Vec2 Position = Body->GetPosition();
	//relocate data in draw buffer
	UVQuad.p[0][0] = glm::vec2(Position.x - HalfSize.x - Edge, Position.y - HalfSize.y - Edge);
	UVQuad.p[1][0] = glm::vec2(Position.x - HalfSize.x - Edge, Position.y + HalfSize.y + Edge);
	UVQuad.p[2][0] = glm::vec2(Position.x + HalfSize.x + Edge, Position.y + HalfSize.y + Edge);
	UVQuad.p[3][0] = glm::vec2(Position.x + HalfSize.x + Edge, Position.y - HalfSize.y - Edge);
}

bool MBullet::GetCollided()
{
	return Collided;
}

int MBullet::GetEntityType()
{
	return OT_BULLET;
}

void MBullet::OnBeginCollideWith(MPhysicObject* pObject)
{
	switch(pObject->GetEntityType())
	{
		case OT_ENEMY:
			if(PircingLimit)
			{
				PircingValue ++;
				if(PircingLimit <= PircingValue) Collided = true;
			}
			else Collided = true;
			break;
		case OT_BOUNDARY:
			if(RicochetLimit)
			{
				RicochetValue ++;
				if(RicochetLimit <= RicochetValue) Collided = true;
				else Body->SetLinearVelocity(b2Vec2(-Direction.x * Velocity.x, Direction.y * Velocity.y));
			}
			else Collided = true;
			break;
	}
}
