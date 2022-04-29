#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"
// Class for generating a unit cube
class Tree
{
public:
	Tree();
	~Tree();
	void Create(string treeBarkTexture, string leafTexture);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_trunkTexture;
	CTexture m_leafTexture;
};

