#include "stdafx.h"
#include "Game2.h"

map<UINT_PTR, MGame2*> TimerMap;

void CALLBACK MGame2::TimerFunction(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	MGame2* pGame2 = TimerMap[idEvent];
	if(idEvent == pGame2->tmEnemyTimer.Id)
	{
		if(pGame2->EnemyObjects.size() < pGame2->EnemyLimitCount)
		{
			pGame2->NeedAddEnemy = true;
			return;
		}
	}
	if(idEvent == pGame2->tmFrameTimer.Id)
	{
		pGame2->MovableObject->IncreaseAnimationFrame();
		for(unsigned int i=0; i<pGame2->EnemyObjects.size(); i++)
		{
			pGame2->EnemyObjects[i]->IncreaseAnimationFrame();
			pGame2->ObjectBuffer.UpdateObject(pGame2->EnemyObjects[i]);
		}
		return;
	}
}

MGame2::MGame2():MWindow()
{
	Pause = true;
    Key = new bool [256];
    memset(Key, 0, 256);
	
	pDrawFunc = NULL;

	txHeroAnimations = NULL;
	txWall = NULL;
	txBullet = NULL;
	txEnemyAnimations1 = NULL;
	txEnemyAnimations2 = NULL;
	txEnemyAnimations3 = NULL;

	MovableObject = NULL;
	EnemyObject = NULL;
	
	World = NULL;

	CameraRound = 1000;
	CoordinateScale = 0.01; //1 metre - 100 pixels
	
	NeedAddEnemy = false;
}

MGame2::~MGame2()
{
	pDrawFunc = NULL;
}

bool MGame2::Initialize()
{	
    LogFile<<"Game2: Initialize"<<endl;
    
    if(!WindowInitialized)
    {
    	LogFile<<"Game2: Window was not initialized"<<endl;
    	return 0;
	}
	
	//randomize
    LogFile<<"Game2: randomize rand by time"<<endl;
    srand(time(NULL));
    
    //set timers data
    EnemyLimitCount = 15;
	if(!tmEnemyTimer.Set(m_hWnd, TM_ENEMY, 1500, TimerFunction)) return false;
	TimerMap[tmEnemyTimer.Id] = this;
	tmEnemyTimer.Enabled = false;
	if(!tmFrameTimer.Set(m_hWnd, TM_FRAME, 250, TimerFunction)) return false;
	TimerMap[tmFrameTimer.Id] = this;
	tmFrameTimer.Enabled = false;
    
    //prepare view
	Projection = glm::ortho(0.0f, WindowWidth * CoordinateScale, 0.0f, WindowHeight * CoordinateScale);//, -1.0f, 1.0f);
	CameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
	CameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	Model = glm::mat4(1.0f);
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    MVPdefault = MVP = Projection * View * Model;
    
    //prepare vertex array - for all programs
    glGenVertexArrays(1, &VertexArrayId);
	glBindVertexArray(VertexArrayId);
	GLenum Error = glGetError();
	if(Error != GL_NO_ERROR)
	{
		LogFile<<"Game2: "<<(char*)gluErrorString(Error)<<" "<<Error<<endl;
		return false;
	}
	
	LogFile<<"Game2: Load shaders"<<endl;
	ProgramId = LoadShaders((char*)"shaders/main2.vertexshader.glsl", (char*)"shaders/main2.fragmentshader.glsl");
	if(!ProgramId) return false;
	MVPId = glGetUniformLocation(ProgramId, "MVP");
	if(MVPId == -1)
	{
		LogFile<<"Game2: Can't get MVP uniform"<<endl;
		return false;
	}
	ColorId = glGetUniformLocation(ProgramId, "sameColor");
	if(ColorId == -1)
	{
		LogFile<<"Game2: Can't get Color uniform"<<endl;
		return false;
	}
	Sampler2DId = glGetUniformLocation(ProgramId, "mainSampler");
	if(Sampler2DId == -1)
	{
		LogFile<<"Game2: Can't get texture uniform"<<endl;
		return false;
	}
	UseTextureId = glGetUniformLocation(ProgramId, "useSampler");
	if(UseTextureId == -1)
	{
		LogFile<<"Game2: Can't get usetexture uniform"<<endl;
		return false;
	}
	UseOffsetVertexId = glGetUniformLocation(ProgramId, "useOffsetVertex");
	if(UseOffsetVertexId == -1)
	{
		LogFile<<"Game2: Can't get useoffset vertex uniform"<<endl;
		return false;
	}
	UseOffsetUVId = glGetUniformLocation(ProgramId, "useOffsetUV");
	if(UseOffsetUVId == -1)
	{
		LogFile<<"Game2: Can't get useoffset uv uniform"<<endl;
		return false;
	}
	
	if(!(txHeroAnimations = TextureLoader.LoadTexture("textures/tex16.png", 1, 1, 0, txHeroAnimations_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txWall = TextureLoader.LoadTexture("textures/tex05.png", 1, 1, 0, txWall_cnt, GL_NEAREST, GL_REPEAT))) return false;
	if(!(txBullet = TextureLoader.LoadTexture("textures/tex04.png", 1, 1, 0, txBullet_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txEnemyAnimations1 = TextureLoader.LoadTexture("textures/tex18.png", 1, 1, 0, txEnemyAnimations1_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txEnemyAnimations2 = TextureLoader.LoadTexture("textures/tex19.png", 1, 1, 0, txEnemyAnimations2_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txEnemyAnimations3 = TextureLoader.LoadTexture("textures/tex20.png", 1, 1, 0, txEnemyAnimations3_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	
	//box2d world values
	Gravity = b2Vec2(0, -20.0f);
	timeStep = 1.0f / 60.0f;
	velocityIterations = 6;
	positionIterations = 2;
	World = new b2World(Gravity);
    World->SetContactListener(&OCL);
    
    //buffer object
    if(!ObjectBuffer.Initialize(GL_STREAM_DRAW)) return false;
    
    //hero
    MovableObject = new MMovableObject;
    if(!ObjectBuffer.AddObject(MovableObject, 2, 2, 0.32f, 0.32f, 0, 0, 1, 1, txHeroAnimations->Id)) return false;
    	if(!MovableObject->SetAnimations(3, 4)) return false;
    	if(!MovableObject->SetAnimationType(1, ANMT_LOOP)) return false;
		if(!MovableObject->SetCurrentAnimation(1)) return false;
		MovableObject->StartAnimation();
	if(!MovableObject->AddPhysics(World, b2_dynamicBody, false, true, OT_MOVABLE, OT_BOUNDARY, 0.02f)) return false;
	if(!MovableObject->SetMoveParameters(0.2f, 0.9f, 2.0f)) return false;
	if(!MovableObject->SetMoveKeys(VK_LEFT, VK_RIGHT, VK_UP)) return false;
	
	//level walls
	Walls.push_back(new MPhysicObject);
	Walls.push_back(new MPhysicObject);
	Walls.push_back(new MPhysicObject);
	Walls.push_back(new MPhysicObject);
	if(!ObjectBuffer.AddObject(Walls[0], 0, 0, 8.0f, 0.5f, 0, 0, 25.0f, 1.5625f, txWall->Id)) return false;//3.1235
	if(!Walls[0]->AddPhysics(World, b2_staticBody, false, false, OT_BOUNDARY, OT_MOVABLE | OT_BULLET | OT_ENEMY)) return false;
	if(!ObjectBuffer.AddObject(Walls[1], 0, 5.5f, 8.0f, 0.5f, 0, 0, 25.0f, 1.5625f, txWall->Id)) return false;
	if(!Walls[1]->AddPhysics(World, b2_staticBody, false, false, OT_BOUNDARY, OT_MOVABLE | OT_BULLET | OT_ENEMY)) return false;
	if(!ObjectBuffer.AddObject(Walls[2], 0, 0, 0.5f, 6.0f, 0, 0, 1.5625f, 18.75f, txWall->Id)) return false;
	if(!Walls[2]->AddPhysics(World, b2_staticBody, false, false, OT_BOUNDARY, OT_MOVABLE | OT_BULLET | OT_ENEMY)) return false;
	if(!ObjectBuffer.AddObject(Walls[3], 7.5f, 0, 0.5f, 6.0f, 0, 0, 1.5625f, 18.75f, txWall->Id)) return false;
	if(!Walls[3]->AddPhysics(World, b2_staticBody, false, false, OT_BOUNDARY, OT_MOVABLE | OT_BULLET | OT_ENEMY)) return false;
	
	ObjectBuffer.DisposeAll();
	
	//base color
	SetCurrentColor(1, 1, 1, 1);
	
	//stop game processing 
	Start();
	tmEnemyTimer.Start();
	tmFrameTimer.Start();
	//set logo screen
	pDrawFunc = &MGame2::DrawGame;
    
    return true;
}

void MGame2::Start()
{
    Pause = false;
}

void MGame2::Stop()
{
    Pause = true;
}

void MGame2::SetCurrentColor(float R, float G, float B, float A)
{
	CurrentColor[3] = CurrentColor[2] = CurrentColor[1] = CurrentColor[0] = glm::vec4(R, G, B, A);
}

void MGame2::DrawGame()
{
	//math operations
	if(!Pause)
	{	
		World->Step(timeStep, velocityIterations, positionIterations);
		MovableObject->Move();
		ObjectBuffer.UpdateObject(MovableObject);
		for(unsigned int i=0; i<Bullets.size(); i++)
		{
			if(Bullets[i]->GetCollided())
			{
				ObjectBuffer.RemoveObject(Bullets[i]);
				Bullets[i]->RemovePhysics();
				delete Bullets[i];
				Bullets.erase(Bullets.begin() + i);
			}
			else
			{
				Bullets[i]->Fly();
				ObjectBuffer.UpdateObject(Bullets[i]);
			}
		}
		for(unsigned int i=0; i<EnemyObjects.size(); i++)
		{
			if(EnemyObjects[i]->GetDestroyed())
			{
				EnemyObjects[i]->PhysicEnable(false);
				if(EnemyObjects[i]->GetDieAnimationEnd())
				{
					ObjectBuffer.RemoveObject(EnemyObjects[i]);
					delete EnemyObjects[i];
					EnemyObjects.erase(EnemyObjects.begin() + i);
				}
			}
			else
			{
				EnemyObjects[i]->Move();
				ObjectBuffer.UpdateObject(EnemyObjects[i]);
			}
		}
		if(NeedAddEnemy)
		{
			NeedAddEnemy = false;
			EnemyObjects.push_back(new MEnemyObject);
			switch(rand() % 3)
			{
				case 0:
					ObjectBuffer.AddObject(EnemyObjects.back(), rand() % 5 + 1 + rand() % 10 * 0.1, 0.5f, 0.32f, 0.32f, 0, 0, 1, 1, txEnemyAnimations1->Id);
					break;
				case 1:
					ObjectBuffer.AddObject(EnemyObjects.back(), rand() % 5 + 1 + rand() % 10 * 0.1, rand() % 2 + 1 - (rand() % 2) * 0.1f - 0.2f, 0.32f, 0.32f, 0, 0, 1, 1, txEnemyAnimations2->Id);
					break;
				case 2:
					ObjectBuffer.AddObject(EnemyObjects.back(), rand() % 5 + 1 + rand() % 10 * 0.1, 0.5f, 0.64f, 0.64f, 0, 0, 1, 1, txEnemyAnimations3->Id);
					EnemyObjects.back()->SetBulletLimit(5);
					break;
			}
			EnemyObjects.back()->SetAnimations(2, 4);
			EnemyObjects.back()->SetAnimationType(1, ANMT_LOOP);
			EnemyObjects.back()->SetAnimationType(0, ANMT_ONEWAY);
			EnemyObjects.back()->SetCurrentAnimation(1);
			EnemyObjects.back()->StartAnimation();
			EnemyObjects.back()->AddPhysics(World, b2_dynamicBody, true, true, OT_ENEMY, OT_BOUNDARY | OT_BULLET);
			EnemyObjects.back()->SetMoveParameters((rand() % 5) * 0.1f + 0.5f);
			EnemyObjects.back()->SetDieAnimationNumber(0);
			ObjectBuffer.DisposeAll();
		}
		
		//cout<<"Bullets count: "<<Bullets.size()<<endl;
		
		//CameraPosition.x = CameraDirection.x = round((GetQuadCenter(MovableObject->GetVertex()).x - HalfWindowSize[0] * CoordinateScale) * CameraRound) / CameraRound;
		//CameraPosition.y = CameraDirection.y = round((GetQuadCenter(MovableObject->GetVertex()).y - HalfWindowSize[1] * CoordinateScale) * CameraRound) / CameraRound;
		View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0, 1, 0));
    	MVP = Projection * View * Model;
	}
	
	//prepare shader, matrix, color, uniforms
	glUseProgram(ProgramId);
	glUniformMatrix4fv(MVPId, 1, GL_FALSE, &MVP[0][0]);
	
	glUniform4fv(ColorId, 1, &CurrentColor[0][0]);//set white color
	glUniform1i(UseOffsetVertexId, 0);//disable vertex offset
	glUniform1i(UseOffsetUVId, 0);//disable uv offset
	glUniform1i(UseTextureId, 1);//enable texture
	
	glEnable(GL_BLEND);
	
	//draw exit, hero, liquid, text
	glUniform1i(UseOffsetVertexId, 0);//disable vertex offset
	glUniform1i(UseOffsetUVId, 0);//disable uv offset
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
	ObjectBuffer.DrawAll();
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
	
	glDisable(GL_BLEND);
}

void MGame2::OnDraw()
{
	if(pDrawFunc) ((*this).*(pDrawFunc))();
}

void MGame2::OnActivate(WPARAM wParam)
{
	switch(LOWORD(wParam))
	{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if(pDrawFunc == &MGame2::DrawGame) Start();
			LogFile<<"Game2: window activated!"<<endl;
			break;
		case WA_INACTIVE:
			if(pDrawFunc == &MGame2::DrawGame) Stop();
			LogFile<<"Game2: window deactivated!"<<endl;
			break;
	}
}

void MGame2::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 1;
	if(Key[VK_ESCAPE])
	{
		SendMessage(m_hWnd, WM_DESTROY, 0, 0);
		return;
	}
	if(pDrawFunc == &MGame2::DrawGame)
	{
		if(!Pause)
		{
			//key space pressed, key not holded and object on ground
			if(wParam == VK_UP && !(lParam & 0x40000000)) MovableObject->StartJump();
			if(wParam == VK_SPACE && !(lParam & 0x40000000))
			{
				Bullets.push_back(new MBullet);
				ObjectBuffer.AddObject(Bullets.back(), GetQuadCenter(MovableObject->GetVertex()).x, GetQuadCenter(MovableObject->GetVertex()).y, 0.08f, 0.08f, 0, 0, 1, 1, txBullet->Id);
				if(!Bullets.back()->AddVelocity(World, 2 * MovableObject->GetDirectionRight() - 1, 0.0f, 5.8f, 0.0f)) cout<<"BAD!"<<endl;
				ObjectBuffer.DisposeAll();
			}
		}
	}
}

void MGame2::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 0;
	if(pDrawFunc == &MGame2::DrawGame)
	{
	}
}

void MGame2::OnClose()
{
	Stop();
	tmEnemyTimer.Start();
	tmFrameTimer.Start();
	LogFile<<"Game2: Stopped."<<endl;
	
	if(MovableObject)
	{
		MovableObject->RemovePhysics();
		delete MovableObject;
	}
	if(EnemyObject)
	{
		EnemyObject->RemovePhysics();
		delete EnemyObject;
	}
	
	for(unsigned int i=0; i<Walls.size(); i++)
	{
		if(Walls[i])
		{
			Walls[i]->RemovePhysics();
			delete Walls[i];
		}
		
	}
	Walls.clear();
	
	for(unsigned int i=0; i<EnemyObjects.size(); i++)
	{
		if(EnemyObjects[i])
		{
			EnemyObjects[i]->RemovePhysics();
			delete EnemyObjects[i];
		}
		
	}
	EnemyObjects.clear();
	
	for(unsigned int i=0; i<Bullets.size(); i++)
	{
		if(Bullets[i])
		{
			Bullets[i]->RemovePhysics();
			delete Bullets[i];
		}
	}
	Bullets.clear();
	
	if(World) delete World;
	
	ObjectBuffer.Close();
	
	TextureLoader.DeleteTexture(txHeroAnimations, txHeroAnimations_cnt);
	TextureLoader.DeleteTexture(txWall, txWall_cnt);
	TextureLoader.DeleteTexture(txBullet, txBullet_cnt);
	TextureLoader.DeleteTexture(txEnemyAnimations1, txEnemyAnimations1_cnt);
	TextureLoader.DeleteTexture(txEnemyAnimations2, txEnemyAnimations2_cnt);
	TextureLoader.DeleteTexture(txEnemyAnimations3, txEnemyAnimations3_cnt);
	TextureLoader.Close();
	
	glDeleteProgram(ProgramId);
	glDeleteVertexArrays(1, &VertexArrayId);
	LogFile<<"Game2: Shaders free"<<endl;
	
	if(Key) delete [] Key;
	LogFile<<"Game2: Keys free"<<endl;
}

void MGame2::OnSize()
{
	//change projection matix and default MVP
	Projection = glm::ortho(0.0f, WindowWidth * CoordinateScale, 0.0f, WindowHeight * CoordinateScale);
    MVPdefault = Model * Projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
	//center view using translate matrix
	//CameraPosition = glm::vec3(HalfWindowSize[0] * CoordinateScale, 0.0f, 1.0f);
	//CameraDirection = glm::vec3(HalfWindowSize[0] * CoordinateScale, 0.0f, 0.0f);
	
	//change viewport
	glViewport(0, 0, WindowWidth, WindowHeight);//IMPORTANT!
}

