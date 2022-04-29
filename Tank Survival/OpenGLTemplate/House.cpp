#include "House.h"
#include <iostream>
House::House()
{}
House::~House()
{
	Release();
}
void House::Create(string wallTexture, string roofTexture)
{
	m_wallTexture.Load(wallTexture);
	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_wallTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_roofTexture.Load(roofTexture);
	m_roofTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_roofTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_roofTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_roofTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();
	// Write the code to add interleaved vertex attributes to the VBO

	glm::vec3 cubeVertices[24] =
	{
		//front
		glm::vec3(-1.f,0.f,1.f),
		glm::vec3(1.f,0.f,1.f),
		glm::vec3(-1.f,2.f,1.f),
		glm::vec3(1.f,2.f,1.f),

		//right
		glm::vec3(1.f,0.f,1.f),
		glm::vec3(1.f,0.f,-1.f),
		glm::vec3(1.f,2.f,1.f),
		glm::vec3(1.f,2.f,-1.f),

		//left
		glm::vec3(-1.f,0.f,-1.f),
		glm::vec3(-1.f,0.f,1.f),
		glm::vec3(-1.f,2.f,-1.f),
		glm::vec3(-1.f,2.f,1.f),

		//back
		glm::vec3(1.f,0.f,-1.f),
		glm::vec3(-1.f,0.f,-1.f),
		glm::vec3(1.f,2.f,-1.f),
		glm::vec3(-1.f,2.f,-1.f),

		//top
		glm::vec3(-1.f,2.f,1.f),
		glm::vec3(1.f,2.f,1.f),
		glm::vec3(-1.f,2.f,-1.f),
		glm::vec3(1.f,2.f,-1.f),

		//bottom
		glm::vec3(1.f,0.f,1.f),
		glm::vec3(-1.f,0.f,1.f),
		glm::vec3(1.f,0.f,-1.f),
		glm::vec3(-1.f,0.f,-1.f),

	};

	glm::vec3 pyramidVertices[16] =
	{
		//bottom
		glm::vec3(-1.5f,2.f,-1.5f),
		glm::vec3(1.5f,2.f,-1.5f),
		glm::vec3(-1.5f,2.f,1.5f),
		glm::vec3(1.5f,2.f,1.5f),

		//top front
		glm::vec3(-1.5f,2.f,1.5f),
		glm::vec3(1.5f,2.f,1.5f),
		glm::vec3(0.f,4.f,0.f),

		//top right
		glm::vec3(1.5f,2.f,1.5f),
		glm::vec3(1.5f,2.f,-1.5f),
		glm::vec3(0.f,4.f,0.f),

		//top left
		glm::vec3(-1.5f,2.f,-1.5f),
		glm::vec3(-1.5f,2.f,1.5f),
		glm::vec3(0.f,4.f,0.f),

		//top back
		glm::vec3(1.5f,2.f,-1.5f),
		glm::vec3(-1.5f,2.f,-1.5f),
		glm::vec3(0.f,4.f,0.f),
	};

	glm::vec2 cubeTexCoords[4] =
	{
		glm::vec2(0.f,0.f),
		glm::vec2(0.f,1.f),
		glm::vec2(1.f,0.f),
		glm::vec2(1.f,1.f),
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

	glm::vec3 cubeNormalList[6] =
	{
		//front
		glm::vec3(glm::cross(cubeVertices[1] - cubeVertices[0], cubeVertices[2] - cubeVertices[0])),
		//right
		glm::vec3(glm::cross(cubeVertices[5] - cubeVertices[4], cubeVertices[6] - cubeVertices[4])),
		//left
		glm::vec3(glm::cross(cubeVertices[9] - cubeVertices[8], cubeVertices[10] - cubeVertices[8])),
		//back
		glm::vec3(glm::cross(cubeVertices[13] - cubeVertices[12], cubeVertices[14] - cubeVertices[12])),
		//top
		glm::vec3(glm::cross(cubeVertices[17] - cubeVertices[16], cubeVertices[18] - cubeVertices[16])),
		//bottom
		glm::vec3(glm::cross(cubeVertices[21] - cubeVertices[20], cubeVertices[22] - cubeVertices[20])),
	};

	glm::vec3 cubeNormals[24] =
	{
		//front
		cubeNormalList[0],
		cubeNormalList[0],
		cubeNormalList[0],
		cubeNormalList[0],

		//right
		cubeNormalList[1],
		cubeNormalList[1],
		cubeNormalList[1],
		cubeNormalList[1],

		//left
		cubeNormalList[2],
		cubeNormalList[2],
		cubeNormalList[2],
		cubeNormalList[2],

		//back
		cubeNormalList[3],
		cubeNormalList[3],
		cubeNormalList[3],
		cubeNormalList[3],

		//top
		cubeNormalList[4],
		cubeNormalList[4],
		cubeNormalList[4],
		cubeNormalList[4],

		//bottom
		cubeNormalList[5],
		cubeNormalList[5],
		cubeNormalList[5],
		cubeNormalList[5],
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

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 24; i++) {
		m_vbo.AddData(&cubeVertices[i], sizeof(glm::vec3));
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

	for (unsigned int i = 0; i < 6; i++) {
		string output = "Cube normal " + std::to_string(i) + ": [" + std::to_string(cubeNormalList[i].x) + "," + std::to_string(cubeNormalList[i].y) + "," + std::to_string(cubeNormalList[i].z) + "]";
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
void House::Render()
{
	glBindVertexArray(m_vao);
	m_wallTexture.Bind();

	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

	m_roofTexture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 28, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 31, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 34, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 37, 3);

}
void House::Release()
{
	m_wallTexture.Release();
	m_roofTexture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}