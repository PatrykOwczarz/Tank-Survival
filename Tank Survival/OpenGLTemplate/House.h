#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"
// Class for generating a unit cube
class House
{
public:
	House();
	~House();
	void Create(string wallTexture, string roofTexture);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_wallTexture;
	CTexture m_roofTexture;
};

