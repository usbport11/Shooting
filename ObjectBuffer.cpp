#include "stdafx.h"
#include "Object.h"
#include "ObjectBuffer.h"

MObjectBuffer::MObjectBuffer()
{
	Id = -1;
	ObjectsCount = 0;
	Type = GL_STATIC_DRAW;
	ObjectsCount = 0;
}

MObjectBuffer::~MObjectBuffer()
{
	Id = -1;
	ObjectsCount = 0;
}

bool MObjectBuffer::Initialize(GLenum inType)
{
	if(inType != GL_STATIC_DRAW && inType != GL_STREAM_DRAW)
	{
		LogFile<<"ObjectBuffer: Given buffer type is not supported"<<endl;
		return false;
	}
	Type = inType;
	
	glGenBuffers(1, &Id);
	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
	{
		LogFile<<"ObjectBuffer: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
		return false;
	}
	
	return true;
}

bool MObjectBuffer::PreCheck(MObject* Object)
{
	return !(Id < 0 || !Object || Object->GetBufferOffset() < 0 || !ObjectsCount);
}

bool MObjectBuffer::AddObject(MObject* Object, float pX0, float pY0, float pW, float pH, float tX0, float tY0, float tW, float tH, unsigned int inTextureId)
{
	if(Id < 0)
	{
		LogFile<<"ObjectBuffer: Not initialized"<<endl;
		return false;
	}
	if(!inTextureId)
	{
		LogFile<<"ObjectBuffer: Null texture"<<endl;
		return false;
	}
	if(pW < 0.001 && pH > -0.001 || tW < 0.001 && tH > -0.001)
	{
		LogFile<<"Buffer object: object too small?"<<endl;
		return false;
	}
	
	//create UVQuad and add to buffer
	stQuad Vertex;
	stQuad UV;
	stUVQuad UVQuad;
	SetQuad(Vertex, pX0, pY0, pW, pH);
	SetQuad(UV, tX0, tY0, tW, tH);
	SetUVQuad(UVQuad, Vertex, UV);
	UVQuads.push_back(UVQuad);
	
	//set links and data
	//MObject* Object = new MObject;
	if(!Object)
	{
		LogFile<<"ObjectBuffer: NULL object"<<endl;
		return false;
	}
	if(!Object->SetBufferOffset(ObjectsCount))
	{
		LogFile<<"ObjectBuffer: Can't set object offset"<<endl;
		return false;
	}
	if(!Object->Set(UVQuad))
	{
		LogFile<<"ObjectBuffer: Can't set object quad"<<endl;
		return false;
	}
	if(!Object->SetTexture(inTextureId)) return false;
	//Object->SetRemoveMark(false);//?
	
	//add object to pointer array
	Objects.push_back(Object);
	
	ObjectsCount ++;
	
	return true;
}

bool MObjectBuffer::UpdateObject(MObject* Object)
{
	if(!PreCheck(Object)) return false;
	
	UVQuads[Object->GetBufferOffset()] = Object->Get();
	
	glBindBuffer(GL_ARRAY_BUFFER, Id);
	glBufferSubData(GL_ARRAY_BUFFER, Object->GetBufferOffset() * sizeof(stUVQuad), sizeof(stUVQuad), &UVQuads[Object->GetBufferOffset()].p[0][0]);
	
	return true;
}

bool MObjectBuffer::UpdateAll()
{
	for(unsigned int i=0; i<ObjectsCount; i++)
	{
		UVQuads[i] = Objects[i]->Get();
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, Id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, UVQuads.size() * sizeof(stUVQuad), &UVQuads[0].p[0][0]);
	
	return true;
}

bool MObjectBuffer::DisposeAll()
{
	for(unsigned int i=0; i<ObjectsCount; i++)
	{
		//change offsets of objects
		Objects[i]->SetBufferOffset(i); //IMPORTANT!
		UVQuads[i] = Objects[i]->Get();
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, Id);
	glBufferData(GL_ARRAY_BUFFER, UVQuads.size() * sizeof(stUVQuad), NULL, Type);
	glBufferData(GL_ARRAY_BUFFER, UVQuads.size() * sizeof(stUVQuad), &UVQuads[0].p[0][0], Type);
	
	return true;
}

bool MObjectBuffer::RemoveObject(MObject* Object)
{
	if(!PreCheck(Object)) return false;

	//remove object from pointer array
	Objects.erase(Objects.begin() + Object->GetBufferOffset());
	//remove from quad array
	UVQuads.erase(UVQuads.begin() + Object->GetBufferOffset());
	//set wrong offset
	Object->SetBufferOffset(-1);
	//decrese elements count
	ObjectsCount --;
	
	//update all buffer
	if(!DisposeAll()) return false;
	
	return true;
}

void MObjectBuffer::DrawObject(MObject* Object)
{
	//if(!PreCheck(Object)) return;
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Object->GetTextureId());
	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, Id);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	//uv
	glBindBuffer(GL_ARRAY_BUFFER, Id);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);
	//draw
	glDrawArrays(GL_QUADS, 4 * Object->GetBufferOffset(), 4);
}

void MObjectBuffer::DrawAll()
{
	for(unsigned int i=0; i<ObjectsCount; i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Objects[i]->GetTextureId());
		//vertex
		glBindBuffer(GL_ARRAY_BUFFER, Id);
		glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		//uv
		glBindBuffer(GL_ARRAY_BUFFER, Id);
		glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)8);
		//draw
		glDrawArrays(GL_QUADS, 4 * Objects[i]->GetBufferOffset(), 4);
	}
}

void MObjectBuffer::Close()
{
	Objects.clear();
	UVQuads.clear();
	glDeleteBuffers(1, &Id);
}
