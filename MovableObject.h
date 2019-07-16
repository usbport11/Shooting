#ifndef movableobjectH
#define movableobjectH

#include "PhysicObject.h"

#define FOOT_LISTENER_ID 3

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

class MMovableObject: public MPhysicObject
{
private:
	int FootContacts;
	b2Fixture* FootFixture;

	float JumpImpulse;
	int JumpTimeOut;
	float VelocityImpulse;
	float MaxVelocity;
	
	b2Vec2 Velocity;
	b2Vec2 Position;
	
	int KeyLeft;
	int KeyRight;
	int KeyJump;
public:
	MMovableObject();
	~MMovableObject();
	bool SetMoveKeys(int inKeyLeft, int inKeyRight, int inKeyJump);
	bool SetMoveParameters(float inVelocityImpulse, float inJumpImpulse, float inMaxVelocity);
	void Move();
	void StartJump();
	bool RemovePhysics();
	
	bool OnFixtureConnectBegin(int UserData);
	bool OnFixtureConnectEnd(int UserData);
	
	int GetEntityType();
	void OnBeginCollideWith(MPhysicObject* pObject);
};

#endif
