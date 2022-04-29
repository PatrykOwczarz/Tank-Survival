#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	m_pathWidth = 20.f;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	
	m_controlPoints.push_back(glm::vec3(170, -15, -25));
	m_controlPoints.push_back(glm::vec3(130, -5, 160));
	m_controlPoints.push_back(glm::vec3(50, 0, 200));
	m_controlPoints.push_back(glm::vec3(0, 0, 110));
	m_controlPoints.push_back(glm::vec3(-50, 0, 110));
	m_controlPoints.push_back(glm::vec3(-40, 0, 175));
	m_controlPoints.push_back(glm::vec3(-100, 0, 175));
	m_controlPoints.push_back(glm::vec3(-100, 0, 75));
	m_controlPoints.push_back(glm::vec3(-90, 0, -50));
	m_controlPoints.push_back(glm::vec3(-200, 0, -95));
	m_controlPoints.push_back(glm::vec3(-200, 0, -135));
	m_controlPoints.push_back(glm::vec3(-60, 0, -135));
	m_controlPoints.push_back(glm::vec3(-40, 0, -200));
	m_controlPoints.push_back(glm::vec3(0, 0, -200));
	m_controlPoints.push_back(glm::vec3(70, 0, -170));
	m_controlPoints.push_back(glm::vec3(120, -5, -140));
	

	/* These are the control points for the circle path in the path starting guide.
	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(71, 5, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, -71));
	m_controlPoints.push_back(glm::vec3(0, 5, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));
	*/

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size()-1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();

	// Call UniformlySampleControlPoints with the number of samples required
	int numSamples = 1000;
	UniformlySampleControlPoints(numSamples);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < m_centrelinePoints.size() + 1; i++) {
		//float t = (float)i / 100.0f;
		//glm::vec3 v = Interpolate(m_centrelinePoints[i % m_centrelinePoints.size()], m_centrelinePoints[(i + 1) % m_centrelinePoints.size()], m_centrelinePoints[(i + 2) % m_centrelinePoints.size()], m_centrelinePoints[(i + 3) % m_centrelinePoints.size()], t);
		glm::vec3 v = m_centrelinePoints[i % m_centrelinePoints.size()];
		vbo.AddData(&v, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

}


void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) {
		glm::vec3 p = m_centrelinePoints[i % m_centrelinePoints.size()];
		glm::vec3 pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];
		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.f, 1.f, 0.f)));

		glm::vec3 l = p - (m_pathWidth/2) * N;
		glm::vec3 r = p + (m_pathWidth/2) * N;

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}

	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Left offset vao
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	int tempSize = m_leftOffsetPoints.size();
	for (unsigned int i = 0; i < tempSize + 1; i++) {
		float t = (float)i / 100.0f;
		//glm::vec3 v = Interpolate(m_leftOffsetPoints[i % tempSize], m_leftOffsetPoints[(i + 1) % tempSize], m_leftOffsetPoints[(i + 2) % tempSize], m_leftOffsetPoints[(i + 3) % tempSize], t);
		glm::vec3 v = m_leftOffsetPoints[i % m_leftOffsetPoints.size()];
		vbo.AddData(&v, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

	
	// Right offset vao
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	// Create a VBO
	CVertexBufferObject vbo2;
	vbo2.Create();
	vbo2.Bind();
	tempSize = m_rightOffsetPoints.size();
	for (unsigned int i = 0; i < tempSize; i++) {
		float t = (float)i / 100.0f;
		//glm::vec3 v = Interpolate(m_rightOffsetPoints[i % tempSize], m_rightOffsetPoints[(i + 1) % tempSize], m_rightOffsetPoints[(i + 2) % tempSize], m_rightOffsetPoints[(i + 3) % tempSize], t);
		glm::vec3 v = m_rightOffsetPoints[i % m_rightOffsetPoints.size()];
		vbo2.AddData(&v, sizeof(glm::vec3));
		vbo2.AddData(&texCoord, sizeof(glm::vec2));
		vbo2.AddData(&normal, sizeof(glm::vec3));
	}
	// Upload the VBO to the GPU
	vbo2.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

	// Note it is possible to only use one VAO / VBO with all the points instead.


}


void CCatmullRom::CreateTrack(string filePath)
{
	m_textureTrack.Load("resources\\textures\\road-texture-edit.jpg");
	m_textureTrack.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_textureTrack.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_textureTrack.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_textureTrack.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);
	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	vector<glm::vec3> vertices;
	vector<glm::vec2> texCoordsTest;

	for (unsigned int i = 0; i < m_centrelinePoints.size() + 1 ; i++) {
		vertices.push_back(m_leftOffsetPoints[i % m_leftOffsetPoints.size()]);
		vertices.push_back(m_rightOffsetPoints[i % m_rightOffsetPoints.size()]);
		texCoordsTest.push_back(glm::vec2(0.f, (float)i));
		texCoordsTest.push_back(glm::vec2(4.f, (float)i));
		//vertices.push_back(m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()]);
		//vertices.push_back(m_rightOffsetPoints[(i + 1) % m_rightOffsetPoints.size()]);
	}

	glm::vec2 texCoords[4] =
	{
		glm::vec2(0.f,0.f),
		glm::vec2(1.f,0.f),
		glm::vec2(0.f,1.f),
		glm::vec2(1.f,1.f),
	};

	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vbo.AddData(&vertices.at(i), sizeof(glm::vec3));
		vbo.AddData(&texCoordsTest[i], sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	m_vertexCount = vertices.size();
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));


}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	m_texture.Bind();
	glPointSize(3.f);
	glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
	glPointSize(0.5f);
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	m_texture.Bind();
	//glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());
	glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	m_texture.Bind();
	//glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
	glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(m_vaoTrack);
	m_textureTrack.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

vector<glm::vec3> CCatmullRom::GetCentrelinePoints() 
{
	return m_centrelinePoints;
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);

/*
* for (unsigned int i = 0; i < (m_vertexCount / 4) ; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, ((i + 1 ) * 4) % m_vertexCount);
	}
*/