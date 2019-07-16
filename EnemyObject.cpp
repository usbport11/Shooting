#include "stdafx.h"
#include "EnemyObject.h"

MEnemyObject::MEnemyObject():MPhysicObject()
{
	Velocity = 0.0f;
	Destroyed = false;
	DieAnimationEnd = false;
	BulletLimit = 0;
	BulletCount = 0;
}

MEnemyObject::~MEnemyObject()
{
	Velocity = 0.0f;
}

bool MEnemyObject::SetMoveParameters(float inVelocity)
{
	Body->SetGravityScale(0.0f);
	Velocity = inVelocity;
	Body->SetLinearVelocity(b2Vec2(Velocity, 0));
	
	return true;
}

void MEnemyObject::Move()
{
	Position = Body->GetPosition();
	//relocate data in draw buffer
	UVQuad.p[0][0] = glm::vec2(Position.x - HalfSize.x - Edge, Position.y - HalfSize.y - Edge);
	UVQuad.p[1][0] = glm::vec2(Position.x - HalfSize.x - Edge, Position.y + HalfSize.y + Edge);
	UVQuad.p[2][0] = glm::vec2(Position.x + HalfSize.x + Edge, Position.y + HalfSize.y + Edge);
	UVQuad.p[3][0] = glm::vec2(Position.x + HalfSize.x + Edge, Position.y - HalfSize.y - Edge);
}

bool MEnemyObject::GetDestroyed()
{
	return Destroyed;
}

bool MEnemyObject::GetDieAnimationEnd()
{
	return DieAnimationEnd;
}

void MEnemyObject::SetDieAnimationNumber(unsigned int Number)
{
	DieAnimatioNumber = Number;
}

void MEnemyObject::SetBulletLimit(unsigned int Number)
{
	BulletLimit = Number;
}

void MEnemyObject::Die()
{
	//stop move
	Body->SetLinearVelocity(b2Vec2(0, 0));
	//set die animation
	SetCurrentAnimation(DieAnimatioNumber);
	SetCurrentFrame(0);
	Destroyed = true;
}

void MEnemyObject::OnBeginCollideWith(MPhysicObject* pObject)
{
	switch(pObject->GetEntityType())
	{
		case OT_BULLET:
			if(!Destroyed)
			{
				if(BulletLimit)
				{
					BulletCount ++;
					if(BulletLimit <= BulletCount) Die();
				}
				else Die();
			}
			break;
		case OT_BOUNDARY:
			DirectionRight = !DirectionRight;
			FlipTextureByX();
			Velocity = -Velocity;
			Body->SetLinearVelocity(b2Vec2(Velocity, 0));
			break;
	}
}

int MEnemyObject::GetEntityType()
{
	return OT_ENEMY;
}

void MEnemyObject::OnAnimationEnd()
{
	DieAnimationEnd = true;
}
