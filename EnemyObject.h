#ifndef enemyobjectH
#define enemyobjectH

#include "PhysicObject.h"

class MEnemyObject: public MPhysicObject
{
private:
	float Velocity;
	b2Vec2 Position;
	unsigned int BulletLimit;
	unsigned int BulletCount;
	bool Destroyed;
	unsigned int DieAnimatioNumber;
	bool DieAnimationEnd;
	void Die();
	void OnBeginCollideWith(MPhysicObject* pObject);
public:
	MEnemyObject();
	~MEnemyObject();
	bool SetMoveParameters(float inVelocity);
	void Move();
	bool GetDestroyed();
	bool GetDieAnimationEnd();
	void SetDieAnimationNumber(unsigned int Number);
	void SetBulletLimit(unsigned int Number);
	
	int GetEntityType();
	void OnAnimationEnd();
};

#endif
