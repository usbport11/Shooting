#ifndef physicobjectH
#define physicobjectH

#include "AnimatedObject.h"

enum enObjectType
{
	OT_BOUNDARY = 1,
	OT_MOVABLE = 2,
	OT_COLLECTABLE = 4,
	OT_ACTIVATABLE = 8,
	OT_BULLET = 16,
	OT_ENEMY = 32,
};

class MPhysicObject: public MAnimatedObject
{
private:
	bool SensorFixture;
	bool Bullet;
	b2BodyType BodyType;
	uint16 FilterCategory;
	uint16 FilterMask;
protected:
	b2World* pWorld;
	b2Body* Body;
	b2PolygonShape PolygonShape;
	b2BodyDef BodyDef;
	b2FixtureDef FixtureDef;
	b2Fixture* BodyFixture;
	b2Vec2 HalfSize;
	float Edge;
public:
	int FootContacts;
	MPhysicObject();
	~MPhysicObject();
	bool AddPhysics(b2World* inpWorld, b2BodyType inBodyType, bool inSensorFixture, bool inBullet, uint16 inFilterCategory, uint16 inFilterMask, float inEdge = 0.0f);
	bool UpdatePhysics();
	bool RemovePhysics();
	void PhysicEnable(bool Enable);
	
	virtual bool OnFixtureConnectBegin(int UserData);
	virtual bool OnFixtureConnectEnd(int UserData);
	virtual void OnBeginCollide();
	virtual void OnEndCollide();
	virtual int GetEntityType();
	virtual void OnBeginCollideWith(MPhysicObject* pObject);
};

#endif
