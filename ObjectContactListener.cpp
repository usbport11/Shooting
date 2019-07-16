#include "stdafx.h"
#include "ObjectContactListener.h"

void MObjectContactListener::BeginContact(b2Contact* pContact)
{
	b2Fixture* fixtureA = pContact->GetFixtureA();
	b2Fixture* fixtureB = pContact->GetFixtureB();
	MPhysicObject* pObjectA = (MPhysicObject*)fixtureA->GetBody()->GetUserData();
	MPhysicObject* pObjectB = (MPhysicObject*)fixtureB->GetBody()->GetUserData();
	if(pObjectA) pObjectA->OnFixtureConnectBegin((int)fixtureA->GetUserData());
	if(pObjectB) pObjectB->OnFixtureConnectBegin((int)fixtureB->GetUserData());
	if(pObjectA && pObjectB) 
	{
		pObjectA->OnBeginCollideWith(pObjectB);
		pObjectB->OnBeginCollideWith(pObjectA);
		return;
	}
	/*
	MPhysicObject* pObject;
	b2Fixture* fixtureA = pContact->GetFixtureA();
	b2Fixture* fixtureB = pContact->GetFixtureB();
	//foot check
	pObject = (MPhysicObject*)fixtureA->GetBody()->GetUserData();
	if(pObject) pObject->OnFixtureConnectBegin((int)fixtureA->GetUserData());
	pObject = (MPhysicObject*)fixtureB->GetBody()->GetUserData();
	if(pObject) pObject->OnFixtureConnectBegin((int)fixtureB->GetUserData());
	//extend check
	bool SensorA = fixtureA->IsSensor();
	bool SensorB = fixtureB->IsSensor();
	if(!(SensorA ^ SensorB)) return;
	if(SensorA) pObject = (MPhysicObject*)fixtureA->GetBody()->GetUserData();
	else pObject = (MPhysicObject*)fixtureB->GetBody()->GetUserData();
	pObject->OnBeginCollide();
	*/
}
  
void MObjectContactListener::EndContact(b2Contact* pContact)
{
	MPhysicObject* pObject;
	b2Fixture* fixtureA = pContact->GetFixtureA();
	b2Fixture* fixtureB = pContact->GetFixtureB();
	//foot check
	pObject = (MPhysicObject*)fixtureA->GetBody()->GetUserData();
	if(pObject) pObject->OnFixtureConnectEnd((int)fixtureA->GetUserData());
	pObject = (MPhysicObject*)fixtureB->GetBody()->GetUserData();
	if(pObject) pObject->OnFixtureConnectEnd((int)fixtureB->GetUserData());
	/*
	bool SensorA = fixtureA->IsSensor();
	bool SensorB = fixtureB->IsSensor();
	if(!(SensorA ^ SensorB)) return;
	if(SensorA) pObject = (MPhysicObject*)fixtureA->GetBody()->GetUserData();
	else pObject = (MPhysicObject*)fixtureB->GetBody()->GetUserData();
	pObject->OnEndCollide();
	*/
}

bool MObjectContactListener::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	return true;
}
