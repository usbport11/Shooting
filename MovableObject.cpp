#include "stdafx.h"
#include "MovableObject.h"

MMovableObject::MMovableObject():MPhysicObject()
{
	FootFixture = NULL;
	MaxVelocity = 0;
	VelocityImpulse = 0;
	JumpImpulse = 0;
	FootContacts = 0;
	JumpTimeOut = 0;
	KeyLeft = VK_LEFT;
	KeyRight = VK_RIGHT;
	KeyJump = VK_UP;
}

MMovableObject::~MMovableObject()
{
	FootFixture = NULL;
	MaxVelocity = 0;
	VelocityImpulse = 0;
	JumpImpulse = 0;
}

bool MMovableObject::SetMoveParameters(float inVelocityImpulse, float inJumpImpulse, float inMaxVelocity)
{
	if(!pWorld)
	{
		LogFile<<"MMovableObject: NULL world"<<endl;
		return false;
	}
	if(!Body)
	{
		LogFile<<"MMovableObject: NULL body"<<endl;
		return false;
	}
	if(!inVelocityImpulse)
	{
		LogFile<<"MMovableObject: Velocity value is NULL"<<endl;
		return false;
	}
	if(inMaxVelocity <= 0 || inMaxVelocity < inVelocityImpulse)
	{
		LogFile<<"PhysicObject: Velocity values are wrong"<<endl;
		return false;
	}
	if(BodyDef.type == b2_staticBody)
	{
		LogFile<<"MMovableObject: Can't move static object"<<endl;
		return false;
	}
	if((HalfSize.x > -0.01 && HalfSize.x < 0.01) || (HalfSize.y > -0.01 && HalfSize.y < 0.01))
	{
		LogFile<<"MMovableObject: Wrong half size"<<endl;
		return false;
	}
	
	MaxVelocity = inMaxVelocity;
	VelocityImpulse = inVelocityImpulse;
	JumpImpulse = inJumpImpulse;
	
	PolygonShape.SetAsBox(HalfSize.x - HalfSize.x * 0.1f, 0.1f, b2Vec2(0.0f, -0.5f), 0);
	FixtureDef.isSensor = true;
	FootFixture = Body->CreateFixture(&FixtureDef);
	FootFixture->SetUserData((void*)FOOT_LISTENER_ID);
	
	return true;
}

void MMovableObject::Move()
{
	//get velocity
    Velocity = Body->GetLinearVelocity();
    
	//cap velocity
    if(fabs(Velocity.x) > MaxVelocity)
	{
		Velocity.x = sgn(Velocity.x) * MaxVelocity;
		Body->SetLinearVelocity(Velocity);
	}
	
	//stand still check
	if(!(GetAsyncKeyState(KeyLeft) & 0x8000) && !(GetAsyncKeyState(KeyRight) & 0x8000))
	{
		Velocity.x *= 0.9;
		Body->SetLinearVelocity(Velocity);
	}
	
	//add impulse
    if((GetAsyncKeyState(KeyLeft) & 0x8000) && Velocity.x > -MaxVelocity) 
	{
		Body->ApplyLinearImpulse(b2Vec2(-VelocityImpulse, 0), Body->GetWorldCenter(), true);
		if(DirectionRight)
		{
			DirectionRight = false;
			FlipTextureByX();
		}
	}
    if((GetAsyncKeyState(KeyRight) & 0x8000) && Velocity.x < MaxVelocity)
	{
		Body->ApplyLinearImpulse(b2Vec2(VelocityImpulse, 0), Body->GetWorldCenter(), true);
		if(!DirectionRight)
		{
			DirectionRight = true;
			FlipTextureByX();
		}
	}
	
	Position = Body->GetPosition();
	
	//relocate data in draw buffer
	UVQuad.p[0][0] = glm::vec2(Position.x - HalfSize.x - Edge, Position.y - HalfSize.y - Edge);
	UVQuad.p[1][0] = glm::vec2(Position.x - HalfSize.x - Edge, Position.y + HalfSize.y + Edge);
	UVQuad.p[2][0] = glm::vec2(Position.x + HalfSize.x + Edge, Position.y + HalfSize.y + Edge);
	UVQuad.p[3][0] = glm::vec2(Position.x + HalfSize.x + Edge, Position.y - HalfSize.y - Edge);
	
	JumpTimeOut --;
	if(JumpTimeOut < 0) JumpTimeOut = 0;
}

void MMovableObject::StartJump()
{
	cout<<"Trying jump!"<<endl;
	if(FootContacts < 1) 
	{
		cout<<"Foot contacts < 1"<<endl;
		return;
	}
	if(JumpTimeOut > 0)
	{
		cout<<"Jump timeout > 0"<<endl;
		return;
	}
	Body->ApplyLinearImpulse(b2Vec2(0, JumpImpulse), Body->GetPosition(), true);
	JumpTimeOut = 30;
}

bool MMovableObject::RemovePhysics()
{
	//destroy foot fixture
	if(FootFixture) Body->DestroyFixture(FootFixture);
	//call basic destroy
	MPhysicObject::RemovePhysics();
	
	return true;
}

bool MMovableObject::OnFixtureConnectBegin(int UserData)
{
	if(UserData == FOOT_LISTENER_ID)
	{
		FootContacts ++;
		return true;
	}
	return false;
}

bool MMovableObject::OnFixtureConnectEnd(int UserData)
{
	if(UserData == FOOT_LISTENER_ID)
	{
		FootContacts --;
		return true;
	}
	return false;
}

bool MMovableObject::SetMoveKeys(int inKeyLeft, int inKeyRight, int inKeyJump)
{
	KeyLeft = inKeyLeft;
	KeyRight = inKeyRight;
	KeyJump = inKeyJump;
	return true;
}

int MMovableObject::GetEntityType()
{
	return OT_MOVABLE;
}

void MMovableObject::OnBeginCollideWith(MPhysicObject* pObject)
{
}
