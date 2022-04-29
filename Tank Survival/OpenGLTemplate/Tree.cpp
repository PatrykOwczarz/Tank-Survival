#include "Tree.h"
#include <iostream>
Tree::Tree()
{}
Tree::~Tree()
{
	Release();
}
void Tree::Create(string treeBarkTexture, string leafTexture)
{
	m_trunkTexture.Load(treeBarkTexture);
	m_trunkTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_trunkTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_trunkTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_trunkTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_leafTexture.Load(leafTexture);
	m_leafTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_leafTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_leafTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_leafTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();
	// Write the code to add interleaved vertex attributes to the VBO

	glm::vec3 trunkVertices[24] =
	{
		//front
		glm::vec3(-0.5f,0.f,0.5f),
		glm::vec3(0.5f,0.f,0.5f),
		glm::vec3(-0.5f,4.f,0.5f),
		glm::vec3(0.5f,4.f,0.5f),

		//right
		glm::vec3(0.5f,0.f,0.5f),
		glm::vec3(0.5f,0.f,-0.5f),
		glm::vec3(0.5f,4.f,0.5f),
		glm::vec3(0.5f,4.f,-0.5f),

		//left
		glm::vec3(-0.5f,0.f,-0.5f),
		glm::vec3(-0.5f,0.f,0.5f),
		glm::vec3(-0.5f,4.f,-0.5f),
		glm::vec3(-0.5f,4.f,0.5f),

		//back
		glm::vec3(0.5f,0.f,-0.5f),
		glm::vec3(-0.5f,0.f,-0.5f),
		glm::vec3(0.5f,4.f,-0.5f),
		glm::vec3(-0.5f,4.f,-0.5f),

		//top
		glm::vec3(-0.5f,4.f,0.5f),
		glm::vec3(0.5f,4.f,0.5f),
		glm::vec3(-0.5f,4.f,-0.5f),
		glm::vec3(0.5f,4.f,-0.5f),

		//bottom
		glm::vec3(0.5f,0.f,0.5f),
		glm::vec3(-0.5f,0.f,0.5f),
		glm::vec3(0.5f,0.f,-0.5f),
		glm::vec3(-0.5f,0.f,-0.5f),

	};

	glm::vec3 pyramidVertices[16] =
	{
		//bottom
		glm::vec3(-1.5f,3.f,-1.5f),
		glm::vec3(1.5f,3.f,-1.5f),
		glm::vec3(-1.5f,3.f,1.5f),
		glm::vec3(1.5f,3.f,1.5f),

		//top front
		glm::vec3(-1.5f,3.f,1.5f),
		glm::vec3(1.5f,3.f,1.5f),
		glm::vec3(0.f,8.f,0.f),

		//top right
		glm::vec3(1.5f,3.f,1.5f),
		glm::vec3(1.5f,3.f,-1.5f),
		glm::vec3(0.f,8.f,0.f),

		//top left
		glm::vec3(-1.5f,3.f,-1.5f),
		glm::vec3(-1.5f,3.f,1.5f),
		glm::vec3(0.f,8.f,0.f),

		//top back
		glm::vec3(1.5f,3.f,-1.5f),
		glm::vec3(-1.5f,3.f,-1.5f),
		glm::vec3(0.f,8.f,0.f),
	};

	glm::vec3 pyramid2Vertices[16] =
	{
		//bottom
		glm::vec3(-1.5f,5.f,-1.5f),
		glm::vec3(1.5f,5.f,-1.5f),
		glm::vec3(-1.5f,5.f,1.5f),
		glm::vec3(1.5f,5.f,1.5f),

		//top front
		glm::vec3(-1.5f,5.f,1.5f),
		glm::vec3(1.5f,5.f,1.5f),
		glm::vec3(0.f,8.f,0.f),

		//top right
		glm::vec3(1.5f,5.f,1.5f),
		glm::vec3(1.5f,5.f,-1.5f),
		glm::vec3(0.f,8.f,0.f),

		//top left
		glm::vec3(-1.5f,5.f,-1.5f),
		glm::vec3(-1.5f,5.f,1.5f),
		glm::vec3(0.f,8.f,0.f),

		//top back
		glm::vec3(1.5f,5.f,-1.5f),
		glm::vec3(-1.5f,5.f,-1.5f),
		glm::vec3(0.f,8.f,0.f),

		// Alternative design for the tree where the tree is 
		/*
		//bottom
		glm::vec3(-2.f,5.f,0.f),
		glm::vec3(0.f,5.f,-2.f),
		glm::vec3(0.f,5.f,2.f),
		glm::vec3(2.f,5.f,0.f),

		//top front
		glm::vec3(0.f,5.f,2.f),
		glm::vec3(2.f,5.f,0.f),
		glm::vec3(0.f,8.f,0.f),

		//top right
		glm::vec3(2.f,5.f,0.f),
		glm::vec3(0.f,5.f,-2.f),
		glm::vec3(0.f,8.f,0.f),

		//top left
		glm::vec3(-2.f,5.f,0.f),
		glm::vec3(0.f,5.f,2.f),
		glm::vec3(0.f,8.f,0.f),

		//top back
		glm::vec3(0.f,5.f,-2.f),
		glm::vec3(-2.f,5.f,0.f),
		glm::vec3(0.f,8.f,0.f),
		*/
	};

	glm::vec2 cubeTexCoords[4] =
	{
		glm::vec2(0.f,0.f),
		glm::vec2(0.f,1.f),
		glm::vec2(3.f,0.f),
		glm::vec2(3.f,1.f),
	};

	glm::vec2 basedRoofTexCoord[4] =
	{
		glm::vec2(0.f,0.f),
		glm::vec2(0.f,5.f),
		glm::vec2(5.f,0.f),
		glm::vec2(5.f,5.f),
	};

	glm::vec2 pyramidTexCoords[3] =
	{
		glm::vec2(0.f,0.f),
		glm::vec2(0.f,5.f),
		glm::vec2(2.5f,5.f),
	};

	glm::vec3 trunkNormalList[6] =
	{
		//front
		glm::vec3(glm::cross(trunkVertices[1] - trunkVertices[0], trunkVertices[2] - trunkVertices[0])),
		//right
		glm::vec3(glm::cross(trunkVertices[5] - trunkVertices[4], trunkVertices[6] - trunkVertices[4])),
		//left
		glm::vec3(glm::cross(trunkVertices[9] - trunkVertices[8], trunkVertices[10] - trunkVertices[8])),
		//back
		glm::vec3(glm::cross(trunkVertices[13] - trunkVertices[12], trunkVertices[14] - trunkVertices[12])),
		//top
		glm::vec3(glm::cross(trunkVertices[17] - trunkVertices[16], trunkVertices[18] - trunkVertices[16])),
		//bottom
		glm::vec3(glm::cross(trunkVertices[21] - trunkVertices[20], trunkVertices[22] - trunkVertices[20])),
	};

	glm::vec3 cubeNormals[24] =
	{
		//front
		trunkNormalList[0],
		trunkNormalList[0],
		trunkNormalList[0],
		trunkNormalList[0],

		//right
		trunkNormalList[1],
		trunkNormalList[1],
		trunkNormalList[1],
		trunkNormalList[1],

		//left
		trunkNormalList[2],
		trunkNormalList[2],
		trunkNormalList[2],
		trunkNormalList[2],

		//back
		trunkNormalList[3],
		trunkNormalList[3],
		trunkNormalList[3],
		trunkNormalList[3],

		//top
		trunkNormalList[4],
		trunkNormalList[4],
		trunkNormalList[4],
		trunkNormalList[4],

		//bottom
		trunkNormalList[5],
		trunkNormalList[5],
		trunkNormalList[5],
		trunkNormalList[5],
	};

	glm::vec3 pyramidNormalList[4] =
	{
		//top front
		glm::vec3(glm::cross(pyramidVertices[5] - pyramidVertices[4], pyramidVertices[6] - pyramidVertices[4])),
		//top right
		glm::vec3(glm::cross(pyramidVertices[8] - pyramidVertices[7], pyramidVertices[9] - pyramidVertices[7])),
		//top left
		glm::vec3(glm::cross(pyramidVertices[11] - pyramidVertices[10], pyramidVertices[12] - pyramidVertices[10])),
		//top back
		glm::vec3(glm::cross(pyramidVertices[14] - pyramidVertices[13], pyramidVertices[15] - pyramidVertices[13])),
	};

	glm::vec3 pyramidNormals[16] =
	{
		//bottom
		glm::vec3(0.f,-1.f,0.f),
		glm::vec3(0.f,-1.f,0.f),
		glm::vec3(0.f,-1.f,0.f),
		glm::vec3(0.f,-1.f,0.f),

		//top front
		pyramidNormalList[0],
		pyramidNormalList[0],
		pyramidNormalList[0],

		//top right
		pyramidNormalList[1],
		pyramidNormalList[1],
		pyramidNormalList[1],

		//top left
		pyramidNormalList[2],
		pyramidNormalList[2],
		pyramidNormalList[2],

		//top back
		pyramidNormalList[3],
		pyramidNormalList[3],
		pyramidNormalList[3],
	};

	glm::vec3 pyramid2NormalList[4] =
	{
		//top front
		glm::vec3(glm::cross(pyramid2Vertices[5] - pyramid2Vertices[4], pyramid2Vertices[6] - pyramid2Vertices[4])),
		//top right
		glm::vec3(glm::cross(pyramid2Vertices[8] - pyramid2Vertices[7], pyramid2Vertices[9] - pyramid2Vertices[7])),
		//top left
		glm::vec3(glm::cross(pyramid2Vertices[11] - pyramid2Vertices[10], pyramid2Vertices[12] - pyramid2Vertices[10])),
		//top back
		glm::vec3(glm::cross(pyramid2Vertices[14] - pyramid2Vertices[13], pyramid2Vertices[15] - pyramid2Vertices[13])),
	};

	glm::vec3 pyramid2Normals[16] =
	{
		//bottom
		glm::vec3(0.f,-1.f,0.f),
		glm::vec3(0.f,-1.f,0.f),
		glm::vec3(0.f,-1.f,0.f),
		glm::vec3(0.f,-1.f,0.f),

		//top front
		pyramid2NormalList[0],
		pyramid2NormalList[0],
		pyramid2NormalList[0],

		//top right
		pyramid2NormalList[1],
		pyramid2NormalList[1],
		pyramid2NormalList[1],

		//top left
		pyramid2NormalList[2],
		pyramid2NormalList[2],
		pyramid2NormalList[2],

		//top back
		pyramid2NormalList[3],
		pyramid2NormalList[3],
		pyramid2NormalList[3],
	};

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 24; i++) {
		m_vbo.AddData(&trunkVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&cubeTexCoords[i % 4], sizeof(glm::vec2));
		m_vbo.AddData(&cubeNormals[i], sizeof(glm::vec3));
	}

	for (unsigned int i = 0; i < 4; i++) {
		m_vbo.AddData(&pyramidVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&basedRoofTexCoord[i], sizeof(glm::vec2));
		m_vbo.AddData(&pyramidNormals[i], sizeof(glm::vec3));
	}

	for (unsigned int i = 0; i < 12; i++) {
		m_vbo.AddData(&pyramidVertices[i + 4], sizeof(glm::vec3));
		m_vbo.AddData(&pyramidTexCoords[i % 3], sizeof(glm::vec2));
		m_vbo.AddData(&pyramidNormals[i + 4], sizeof(glm::vec3));
	}

	for (unsigned int i = 0; i < 4; i++) {
		m_vbo.AddData(&pyramid2Vertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&basedRoofTexCoord[i], sizeof(glm::vec2));
		m_vbo.AddData(&pyramid2Normals[i], sizeof(glm::vec3));
	}

	for (unsigned int i = 0; i < 12; i++) {
		m_vbo.AddData(&pyramid2Vertices[i + 4], sizeof(glm::vec3));
		m_vbo.AddData(&pyramidTexCoords[i % 3], sizeof(glm::vec2));
		m_vbo.AddData(&pyramid2Normals[i + 4], sizeof(glm::vec3));
	}

	for (unsigned int i = 0; i < 6; i++) {
		string output = "Cube normal " + std::to_string(i) + ": [" + std::to_string(trunkNormalList[i].x) + "," + std::to_string(trunkNormalList[i].y) + "," + std::to_string(trunkNormalList[i].z) + "]";
		std::cout << output;
	}

	for (unsigned int i = 0; i < 4; i++) {
		string output = "Pyramid normal " + std::to_string(i) + ": [" + std::to_string(pyramidNormalList[i].x) + "," + std::to_string(pyramidNormalList[i].y) + "," + std::to_string(pyramidNormalList[i].z) + "]";
		std::cout << output;
	}

	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}
void Tree::Render()
{
	glBindVertexArray(m_vao);
	m_trunkTexture.Bind();

	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

	m_leafTexture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 28, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 31, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 34, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 37, 3);

	glDrawArrays(GL_TRIANGLE_STRIP, 40, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 44, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 47, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 50, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 53, 3);

}
void Tree::Release()
{
	m_trunkTexture.Release();
	m_leafTexture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}