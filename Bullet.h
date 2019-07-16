#ifndef bulletH
#define bulletH

#include "PhysicObject.h"

class MBullet: public MPhysicObject
{
private:
	unsigned int PircingLimit; //0 - not use
	unsigned int PircingValue;
	unsigned int RicochetLimit; //0 - not use
	unsigned int RicochetValue;
	unsigned int LiveLimit; //0 - not use
	unsigned int LiveValue;
	
	bool Collided;
	glm::vec2 Direction;
	glm::vec2 Velocity;
	void OnBeginCollideWith(MPhysicObject* pObject);
public:
	MBullet();
	~MBullet();
	bool AddVelocity(b2World* inpWorld, float DirectionX, float DirectionY, float VelocityX, float VelocityY);
	void SetPircing(unsigned int inPircingLimit);
	void SetRicochet(unsigned int inRicochetLimit);
	void SetLive(unsigned int inLiveLimit);
	void Fly();
	bool GetCollided();
	
	int GetEntityType();
};

#endif
