#ifndef game2H
#define game2H

#define TM_ENEMY 3001
#define TM_FRAME 3002

#include "Window.h"
#include "TextureLoader.h"
#include "ObjectBuffer.h"
#include "MovableObject.h"
#include "EnemyObject.h"
#include "Bullet.h"
#include "ObjectContactListener.h"

class MGame2;

typedef void (MGame2::*VoidFunc) ();

class MGame2: public MWindow
{
private:
	VoidFunc pDrawFunc;
	
	//statuses
	bool Pause;
	bool* Key;
	
	//classes
	MObjectBuffer ObjectBuffer;
	MMovableObject* MovableObject;
	MObjectContactListener OCL;
	MTextureLoader TextureLoader;
	MEnemyObject* EnemyObject;
	
	//textures
	stTexture* txHeroAnimations;
	unsigned int txHeroAnimations_cnt;
	stTexture* txWall;
	unsigned int txWall_cnt;
	stTexture* txBullet;
	unsigned int txBullet_cnt;
	stTexture* txEnemyAnimations1;
	unsigned int txEnemyAnimations1_cnt;
	stTexture* txEnemyAnimations2;
	unsigned int txEnemyAnimations2_cnt;
	stTexture* txEnemyAnimations3;
	unsigned int txEnemyAnimations3_cnt;
	
	//physic
	b2Vec2 Gravity;
	float32 timeStep;
	int velocityIterations;
	int positionIterations;
	b2World* World;
	vector<MPhysicObject*> Walls; 
	vector<MBullet*> Bullets;
	vector<MEnemyObject*> EnemyObjects;
	
	//shaders data
	GLuint VertexArrayId;
	GLuint ProgramId;
	GLuint MVPId;
	GLuint ColorId;
	GLuint Sampler2DId;
	GLuint UseTextureId;
	GLuint UseOffsetVertexId;
	GLuint UseOffsetUVId;
	
	//matrixes and view
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	glm::mat4 MVP;
	glm::mat4 MVPdefault;
	glm::vec3 CameraPosition;
	glm::vec3 CameraDirection;
	float CameraRound;
	float CoordinateScale; //diffrence between resolution and cordinate system (needed for box2d better reaction)
	
	//help data
	glm::vec4 CurrentColor[4];
	void SetCurrentColor(float R, float G, float B, float A);
	
	//draw functions
	void DrawGame();
	
	//overload virtual functions
	void OnDraw();
	void OnActivate(WPARAM wParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnSize();
	
	//local
	void Start();
	void Stop();
	
	//timer
	bool NeedAddEnemy;
	unsigned int EnemyLimitCount;
	stTimer tmEnemyTimer;
	stTimer tmFrameTimer;
	static void CALLBACK TimerFunction(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	
public:
	MGame2();
	~MGame2();
	bool Initialize();
	void OnClose();
};

#endif
