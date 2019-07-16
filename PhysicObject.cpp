#include "stdafx.h"
#include "PhysicObject.h"

MPhysicObject::MPhysicObject():MAnimatedObject()
{
	pWorld = NULL;
	Body = NULL;
	BodyFixture = NULL;
	Edge = 0.0f;
}

MPhysicObject::~MPhysicObject()
{
	pWorld = NULL;
	Body = NULL;
	BodyFixture = NULL;
}

bool MPhysicObject::AddPhysics(b2World* inpWorld, b2BodyType inBodyType, bool inSensorFixture, bool inBullet, uint16 inFilterCategory, uint16 inFilterMask, float inEdge)
{
	if(!inpWorld)
	{
		LogFile<<"PhysicObject: NULL world"<<endl;
		return false;
	}
	glm::vec2 QuadSize = GetQuadSize(GetVertexQuad(UVQuad));
	if(QuadSize.x <= 0.01f || QuadSize.y <= 0.01f)
	{
		LogFile<<"PhysicObject: Too small quad size (not initialized?)"<<endl;
		return false;
	}
	pWorld = inpWorld;
	BodyType = inBodyType;
	SensorFixture = inSensorFixture;
	Bullet = inBullet;
	FilterCategory = inFilterCategory;
	FilterMask = inFilterMask;
	Edge = inEdge;

	HalfSize = b2Vec2((QuadSize.x - Edge * 2) * 0.5f, (QuadSize.y - Edge * 2) * 0.5f);
	BodyDef.type = BodyType;
	BodyDef.fixedRotation = true;
	FixtureDef.isSensor = SensorFixture;
	BodyDef.position.Set(UVQuad.p[0][0].x + HalfSize.x, UVQuad.p[0][0].y + HalfSize.y);
	PolygonShape.SetAsBox(HalfSize.x, HalfSize.y);
	FixtureDef.shape = &PolygonShape;
	FixtureDef.density = 1.0f;
	//FixtureDef.friction = 3.0f;//?
	FixtureDef.filter.categoryBits = FilterCategory;
	FixtureDef.filter.maskBits = FilterMask;
	Body = pWorld->CreateBody(&BodyDef);
	BodyFixture = Body->CreateFixture(&FixtureDef);
	Body->SetBullet(Bullet);
	Body->SetUserData(this);
	
	return true;
}

bool MPhysicObject::UpdatePhysics()
{
	if(!Body || !pWorld) return false;
	Body->SetTransform(b2Vec2(GetQuadCenter(GetVertexQuad(UVQuad)).x, GetQuadCenter(GetVertexQuad(UVQuad)).y), 0.0f);
	return true;
}

bool MPhysicObject::RemovePhysics()
{
	//stop physics
	PhysicEnable(false);
	//body fixture destroy
	if(BodyFixture) Body->DestroyFixture(BodyFixture);
	//destroy body
	if(Body) pWorld->DestroyBody(Body);
	Body = NULL;
	BodyFixture = NULL;
	
	return true;
}

void MPhysicObject::PhysicEnable(bool Enable)
{
	if(Body) Body->SetActive(Enable);
}

bool MPhysicObject::OnFixtureConnectBegin(int UserData){}
bool MPhysicObject::OnFixtureConnectEnd(int UserData){}
void MPhysicObject::OnBeginCollide(){}
void MPhysicObject::OnEndCollide(){}
int MPhysicObject::GetEntityType(){return OT_BOUNDARY;}
void MPhysicObject::OnBeginCollideWith(MPhysicObject* pObject){}

