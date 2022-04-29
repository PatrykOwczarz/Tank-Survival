/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "House.h"
#include "tree.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "HeightMapTerrain.h"
#include "FrameBufferObject.h"

// Constructor
Game::Game()
{
	srand(time(0));
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pPlanarTerrain2 = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_House = NULL;
	m_tree = NULL;
	m_pHeightmapTerrain = NULL;

	m_pTankMesh = NULL;
	m_pBulletMesh = NULL;
	m_pSandbagsMesh = NULL;
	m_pConeMesh = NULL;

	m_pFBO = NULL;
	m_pPlane = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	m_cameraSpeed = 0.05f;
	m_cameraRotation = 0.0f;
	m_topDownCamZoom = 1.0f;

	m_firstPersonCam = false;
	m_thirdPersonCam = true;
	m_topDownCam = false;

	m_playerPosition = glm::vec3(0.0f, 10.0f, 0.0f);
	m_playerStrafe = 0.f;

	m_bulletPosition = glm::vec3(0.f, 0.f, 0.f);
	m_hasSetStart = false;
	m_isDestroyed = false;
	m_hasShot = false;
	m_timeSinceShot = 0;

	m_playerHealth = 3;
	m_playerScore = 0;
	m_isDead = false;

	m_spotlightOn = false;
	m_fogOn = false;

	m_isBlurry = false;

	m_timePassed = 0.0f;

	m_lapTracker = 0;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pPlanarTerrain2;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pSphere;
	delete m_House;
	delete m_tree;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_pHeightmapTerrain;

	delete m_pTankMesh;
	delete m_pSandbagsMesh;
	delete m_pBulletMesh;
	delete m_pConeMesh;

	delete m_pFBO;
	delete m_pPlane;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pPlanarTerrain2 = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	m_House = new House;
	m_tree = new Tree;	
	m_pHeightmapTerrain = new CHeightMapTerrain;

	m_pTankMesh = new COpenAssetImportMesh;
	m_pSandbagsMesh = new COpenAssetImportMesh;
	m_pConeMesh = new COpenAssetImportMesh;
	m_pBulletMesh = new COpenAssetImportMesh;

	m_pPlane = new CPlane;
	m_pFBO = new CFrameBufferObject;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("blurShader.vert");
	sShaderFileNames.push_back("blurShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create a shader program for blur effect
	CShaderProgram* pBlurProgram = new CShaderProgram;
	pBlurProgram->CreateProgram();
	pBlurProgram->AddShaderToProgram(&shShaders[4]);
	pBlurProgram->AddShaderToProgram(&shShaders[5]);
	pBlurProgram->LinkProgram();
	m_pShaderPrograms->push_back(pBlurProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from 
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "GrassBright.bmp", 150.0f, 250.0f, 10.0f); 
	m_pPlanarTerrain2->Create("resources\\textures\\", "dirtpile01.jpg", 150.0f, 250.0f, 10.0f); 

	m_pPlane->Create("resources\\textures\\", "GrassBright.bmp", 40.0f, 30.0f, 1.0f);

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013

	// Load meshes into the game
	m_pTankMesh->Load("resources\\models\\Tank\\Tank.obj");
	m_pSandbagsMesh->Load("resources\\models\\Sandbags\\Sandbags.obj");
	m_pBulletMesh->Load("resources\\models\\Bullet\\Bullet.obj");
	m_pConeMesh->Load("resources\\models\\Cone\\Cone.obj");
	

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);

	m_House->Create("resources\\textures\\brick-wall.jpg", "resources\\textures\\rooftiles.jpg");
	m_tree->Create("resources\\textures\\tree-bark.jpg", "resources\\textures\\grassfloor01.jpg");

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	//m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");			// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

	// Create heightmap terrain test
	m_pHeightmapTerrain->Create((char*)"resources\\textures\\test.bmp", (char*)"resources\\textures\\GrassBright.bmp", glm::vec3(0, 0, 0), 500.0f, 500.0f, 20.5f);

	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateTrack("resources\\textures\\Tile41a.jpg");

	m_conePosition1 = GenerateRandomPositionOnTrack();
	m_conePosition2 = GenerateRandomPositionOnTrack();
	m_conePosition3 = GenerateRandomPositionOnTrack();
	m_conePosition4 = GenerateRandomPositionOnTrack();

	m_barrelPosition1 = GenerateRandomPositionOnTrack();
	m_barrelPosition2 = GenerateRandomPositionOnTrack();

	m_pFBO->Create(width, height);

}

// Render method runs repeatedly in a loop
void Game::RenderScene(int pass) 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	glm::vec4 spotLightPosition1 = glm::vec4(m_spotlightPos, 1.f); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("spotLight1.position", viewMatrix * spotLightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("spotLight1.La", glm::vec3(1.f));		// Ambient colour of light
	pMainProgram->SetUniform("spotLight1.Ld", glm::vec3(1.f));		// Diffuse colour of light
	pMainProgram->SetUniform("spotLight1.Ls", glm::vec3(1.f));		// Specular colour of light
	pMainProgram->SetUniform("spotLight1.direction", glm::normalize(viewNormalMatrix * m_spotlightDir));
	pMainProgram->SetUniform("spotLight1.exponent", 10.0f);
	pMainProgram->SetUniform("spotLight1.cutoff", 30.0f);

	if (m_fogOn) 
	{
		pMainProgram->SetUniform("isFoggy", true);
	}
	else if (!m_fogOn)
	{
		pMainProgram->SetUniform("isFoggy", false);
	}

	// Day mode
	if (!m_spotlightOn) 
	{
		// Set light and materials in main shader program
		glm::vec4 lightPosition1 = glm::vec4(0, 100, 300, 1); // Position of light source *in world coordinates*
		pMainProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
		pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
		pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light

		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.2f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.2f));	// Specular material reflectance
		pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

		pMainProgram->SetUniform("spotLight1.isOn", false);
		
	}
	// Night time
	else if (m_spotlightOn) 
	{
		// Set light and materials in main shader program
		glm::vec4 lightPosition1 = glm::vec4(0, 100, 300, 1); // Position of light source *in world coordinates*
		pMainProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("light1.La", glm::vec3(0.0f));		// Ambient colour of light
		pMainProgram->SetUniform("light1.Ld", glm::vec3(0.0f));		// Diffuse colour of light
		pMainProgram->SetUniform("light1.Ls", glm::vec3(0.0f));		// Specular colour of light

		pMainProgram->SetUniform("material1.Ma", glm::vec3(0.1f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.2f));	// Specular material reflectance
		pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

		pMainProgram->SetUniform("spotLight1.isOn", true);
	}

		

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(150.0f, 0.015f, 0.0f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render(true);
	modelViewMatrixStack.Pop();

	glDisable(GL_CULL_FACE);
	// Render the planar terrain
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(150.0f, 0.01f, 0.0f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain2->Render(true);
		modelViewMatrixStack.Pop();
	glEnable(GL_CULL_FACE);

	// Render the new terrain
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHeightmapTerrain->Render();
	modelViewMatrixStack.Pop();

	/* implementation from the FBO example file. Uses the FBO to create a plane which appears at a location in the game world.
	if (pass == 1) {
		// Render the plane for the TV
		// Back face actually places the horse the right way round
		glDisable(GL_CULL_FACE);
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 50.0f, 0.0f));
		modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(90.f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(180.f));
		modelViewMatrixStack.Scale(-1.0);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the plane colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		m_pFBO->BindTexture(0);
		m_pPlane->Render(false);
		modelViewMatrixStack.Pop();
		glEnable(GL_CULL_FACE);
	}
	*/


	// When pass == 1, render the FBO which is used for the radial blur effect.
	// The strength is controlled by the speed of the tank and the lap tracker counter.
	if (pass == 1) {
		glDisable(GL_CULL_FACE);
		CreateAndRenderFBOQuad();
		glEnable(GL_CULL_FACE);
	}


	/* For use later.
	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.7f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.6f));	// Specular material reflectance	
	*/

	//Render the tank
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_playerPosition);
		modelViewMatrixStack *= m_playerOrientation;
		//modelViewMatrixStack.Scale(1.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pTankMesh->Render();
	modelViewMatrixStack.Pop();

	// Render a bullet
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_bulletPosition);
		modelViewMatrixStack *= m_playerOrientation;
		modelViewMatrixStack.Scale(2.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBulletMesh->Render();
	modelViewMatrixStack.Pop();

	// Render a cone
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_conePosition1);
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pConeMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_conePosition2);
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pConeMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_conePosition3);
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pConeMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_conePosition4);
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pConeMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the barrel 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_barrelPosition1);
		modelViewMatrixStack.Scale(3.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_barrelPosition2);
		modelViewMatrixStack.Scale(3.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	//Render the sandbags;
	glm::vec3 sandbagsPosition = glm::vec3(-75.0f, 0.0f, 150.0f); 
	sandbagsPosition.y = m_pHeightmapTerrain->ReturnGroundHeight(sandbagsPosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(sandbagsPosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(45.f));
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSandbagsMesh->Render();
	modelViewMatrixStack.Pop();

	sandbagsPosition = glm::vec3(-10.0f, 0.0f, 150.0f); 
	sandbagsPosition.y = m_pHeightmapTerrain->ReturnGroundHeight(sandbagsPosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(sandbagsPosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.f));
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSandbagsMesh->Render();
	modelViewMatrixStack.Pop();

	sandbagsPosition = glm::vec3(-130.0f, 0.0f, 60.0f);
	sandbagsPosition.y = m_pHeightmapTerrain->ReturnGroundHeight(sandbagsPosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(sandbagsPosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(0.f));
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSandbagsMesh->Render();
	modelViewMatrixStack.Pop();

	sandbagsPosition = glm::vec3(-100.0f, 0.0f, -100.0f);
	sandbagsPosition.y = m_pHeightmapTerrain->ReturnGroundHeight(sandbagsPosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(sandbagsPosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(-90.f));
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSandbagsMesh->Render();
	modelViewMatrixStack.Pop();

	sandbagsPosition = glm::vec3(120.0f, 0.2f, -100.0f);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(sandbagsPosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.f));
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSandbagsMesh->Render();
	modelViewMatrixStack.Pop();

	sandbagsPosition = glm::vec3(150.0f, 0.2f, -100.0f); 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(sandbagsPosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(90.f));
		modelViewMatrixStack.Scale(3.f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSandbagsMesh->Render();
	modelViewMatrixStack.Pop();


	// Render the horses
	glm::vec3 horsePosition = glm::vec3(20.0f, 0.0f, 0.0f); 
	horsePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(horsePosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(horsePosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(0.f));
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();

	horsePosition = glm::vec3(0.0f, 0.0f, 20.0f);
	horsePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(horsePosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(horsePosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(270.f));
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();

	horsePosition = glm::vec3(-10.0f, 0.0f, -10.0f);
	horsePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(horsePosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(horsePosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(135.f));
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();


	// Render the sphere
	/*
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 2.0f, 150.0f));
		modelViewMatrixStack.Scale(2.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		m_pSphere->Render();
	modelViewMatrixStack.Pop();
	*/

	// Render the houses
	glm::vec3 housePosition = glm::vec3(-150.f, 0.f, 0.f); 
	housePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(housePosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(housePosition);
		modelViewMatrixStack.Scale(20.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_House->Render();
	modelViewMatrixStack.Pop();

	housePosition = glm::vec3(-180.f, 0.f, 180.f); 
	housePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(housePosition);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(housePosition);
		modelViewMatrixStack.Scale(15.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_House->Render();
	modelViewMatrixStack.Pop();

	housePosition = glm::vec3(170.f, 0.2f, 0.f); 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(housePosition);
		modelViewMatrixStack.Scale(15.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_House->Render();
	modelViewMatrixStack.Pop();

	// Render the Trees
	for (int i = -8; i < 9; i++) 
	{
		glm::vec3 treePosition = glm::vec3(30.f * i, 0.f, -230.f);
		treePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(treePosition);
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(treePosition);
			modelViewMatrixStack.Scale(5.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_tree->Render();
		modelViewMatrixStack.Pop();
	}

	for (int i = -8; i < 9; i++)
	{
		glm::vec3 treePosition = glm::vec3(30.f * i, 0.f, 230.f); 
		treePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(treePosition);
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(treePosition);
			modelViewMatrixStack.Scale(5.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_tree->Render();
		modelViewMatrixStack.Pop();
	}

	for (int i = -7; i < 8; i++)
	{
		glm::vec3 treePosition = glm::vec3(240.f, 0.f, 30.f * i);
		treePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(treePosition);
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(treePosition);
			modelViewMatrixStack.Scale(5.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_tree->Render();
		modelViewMatrixStack.Pop();
	}

	for (int i = -7; i < 8; i++)
	{
		glm::vec3 treePosition = glm::vec3(-240.f, 0.1f, 30.f * i); 
		treePosition.y = m_pHeightmapTerrain->ReturnGroundHeight(treePosition);
		modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(treePosition);
			modelViewMatrixStack.Scale(5.0f);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_tree->Render();
		modelViewMatrixStack.Pop();
	}

	glm::vec3 treePosition = glm::vec3(120.f, 0.1f, 80.f);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(treePosition);
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_tree->Render();
	modelViewMatrixStack.Pop();

	treePosition = glm::vec3(150.f, 0.1f, 50.f);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(treePosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(40.f));
		modelViewMatrixStack.Scale(4.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_tree->Render();
	modelViewMatrixStack.Pop();

	treePosition = glm::vec3(180.f, 0.1f, 100.f);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(treePosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(60.f));
		modelViewMatrixStack.Scale(4.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_tree->Render();
	modelViewMatrixStack.Pop();

	treePosition = glm::vec3(70.f, 0.1f, 100.f);
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(treePosition);
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(30.f));
		modelViewMatrixStack.Scale(4.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_tree->Render();
	modelViewMatrixStack.Pop();


	// Render tha Centreline
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("bUseTexture", false); // turn off texturing
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// Render your object here
		m_pCatmullRom->RenderCentreline();
	modelViewMatrixStack.Pop();

	// Render offset curve
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("bUseTexture", false); // turn off texturing
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// Render your object here
		m_pCatmullRom->RenderOffsetCurves();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// Render your object here
		m_pCatmullRom->RenderTrack();
	modelViewMatrixStack.Pop();	


}

void Game::Render() 
{
	m_pFBO->Bind();
	RenderScene(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderScene(1);
	
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRateAndHud();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	// updates the player controls
	PlayerController();

	m_currentDistance += m_dt * m_cameraSpeed;
	glm::vec3 p;
	glm::vec3 pNext;
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 1.f, pNext);
	glm::vec3 T = glm::normalize(pNext - p);
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));
	
	glm::vec3 up = glm::rotate(glm::vec3(0, 1, 0), m_cameraRotation, T);

	// clamp player position so that it stays within the track width.
	m_playerStrafe = glm::clamp(m_playerStrafe, -4.f, 4.f);

	// set player position and orientation based on the TNB frame.
	m_playerPosition = p + (0.7f * B) + (m_playerStrafe * N);
	m_playerOrientation = glm::mat4(glm::mat3(T, B, N));

	// sets the position and direction of the spotlight to follow the player.
	m_spotlightPos = m_playerPosition + (B * 3.f);
	m_spotlightDir = (5.f*T) - B ;

	DetectAndProcessCollision();

	// clamp the zoom amount for the top down camera mode
	m_topDownCamZoom = glm::clamp(m_topDownCamZoom, 0.3f, 1.5f);

	// sets different camera configurations based on what mode of camera is toggled.
	if (m_thirdPersonCam) {
		m_pCamera->Set(p + (B * 5.f) + (T * -10.f), p + T * 10.f, up);
	}
	else if (m_topDownCam) {
		m_pCamera->Set(m_playerPosition + glm::vec3(0.f, 300.f * m_topDownCamZoom, 0.f), m_playerPosition + (T * 2.f), T);
	}
	else if (m_firstPersonCam) 
	{
		m_pCamera->Set(m_playerPosition + (B * 3.f), m_playerPosition + (T * 3.1f) + (B * 1.3f) + T, up);
	}
	// free roam camera if none of the bools are true.
	else if (!m_thirdPersonCam && !m_topDownCam && !m_firstPersonCam) {
		m_pCamera->Update(m_dt);
	}

	// each lap the player does, the score is incremented and the obstacle positions are randomly generated again.
	if (m_lapTracker < m_pCatmullRom->CurrentLap(m_currentDistance)) 
	{
		m_lapTracker = m_pCatmullRom->CurrentLap(m_currentDistance);
		m_playerScore += 1;
		m_cameraSpeed += 0.01;
		m_conePosition1 = GenerateRandomPositionOnTrack();
		m_conePosition2 = GenerateRandomPositionOnTrack();
		m_conePosition3 = GenerateRandomPositionOnTrack();
		m_conePosition4 = GenerateRandomPositionOnTrack();
		m_barrelPosition1 = GenerateRandomPositionOnTrack();
		m_barrelPosition2 = GenerateRandomPositionOnTrack();
	}

	if (m_lapTracker == 1) 
	{
		m_spotlightOn = true;
	}
	else if (m_lapTracker == 2)
	{
		m_spotlightOn = false;
		m_fogOn = true;
	}
	else if (m_lapTracker == 3)
	{
		m_spotlightOn = true;
		m_fogOn = true;
	}

	// if the time since the bullet has been shot it gets automatically destroyed and reset.
	if (m_timeSinceShot == 3) 
	{
		m_isDestroyed = true;
		m_timeSinceShot = 0;
	}

	// if the bullet has been shot and destroyed, the position is reset and the booleans reset.
	if (m_hasShot && m_isDestroyed)
	{
		m_bulletPosition = glm::vec3(0.f, 0.f, 0.f);
		m_hasShot = false;
		m_isDestroyed = false;
		m_hasSetStart = false;	
	}

	// if the bullet has been shot but not destroyed, the bullet position is updated based on the player position.
	if (m_hasShot && !m_isDestroyed) 
	{
		// this if statement defines the start position of the bullet at the moment it is shot.
		if(!m_hasSetStart)
		{
			m_bulletPosition = m_playerPosition + (T * 3.1f) + (B * 1.3f);
			m_bulletDirection = T;
		}
		m_hasSetStart = true;
		m_bulletPosition += m_bulletDirection;
	}

	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	//m_pCamera->Update(m_dt);

	m_pAudio->Update();
}

// Used the following guide on how to create and use a 2D quad with an FBO: https://learnopengl.com/Advanced-OpenGL/Framebuffers
// reference to this code: https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/5.1.framebuffers/framebuffers.cpp
void Game::CreateAndRenderFBOQuad() 
{
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// creates and uses the blur shader program.
	CShaderProgram* blurProgram = (*m_pShaderPrograms)[2];

	blurProgram->UseProgram();
	// adjusts the strength of the radial blur based on the speed of the tank and lap counter.
	blurProgram->SetUniform("speedStrength", (5 * m_cameraSpeed * m_lapTracker));
	glBindVertexArray(quadVAO);
	m_pFBO->BindTexture(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

// A function that detects collisions and processes them accordingly
void Game::DetectAndProcessCollision()
{
	// Player and cone 1
	if (glm::length(m_playerPosition - m_conePosition1) < 3) 
	{
		DamagePlayer(1);
		m_conePosition1 = GenerateRandomPositionOnTrack();
	}

	// Player and cone 2
	if (glm::length(m_playerPosition - m_conePosition2) < 3)
	{
		DamagePlayer(1);
		m_conePosition2 = GenerateRandomPositionOnTrack();
	}

	// Player and cone 3
	if (glm::length(m_playerPosition - m_conePosition3) < 3)
	{
		DamagePlayer(1);
		m_conePosition3 = GenerateRandomPositionOnTrack();
	}

	// Player and cone 4
	if (glm::length(m_playerPosition - m_conePosition4) < 3)
	{
		DamagePlayer(1);
		m_conePosition4 = GenerateRandomPositionOnTrack();
	}

	// Player and barrel 1
	if (glm::length(m_playerPosition - m_barrelPosition1) < 3.5f)
	{
		DamagePlayer(1);
		m_barrelPosition1 = GenerateRandomPositionOnTrack();
	}

	// Player and barrel 2
	if (glm::length(m_playerPosition - m_barrelPosition2) < 3.5f)
	{
		DamagePlayer(1);
		m_barrelPosition2 = GenerateRandomPositionOnTrack();
	}

	// Bullet and barrel 1
	if (glm::length(m_bulletPosition - m_barrelPosition1) < 3.f)
	{
		m_isDestroyed = true;
		m_playerScore += 1;
		m_barrelPosition1 = GenerateRandomPositionOnTrack();
	}

	// Bullet and barrel 2
	if (glm::length(m_bulletPosition - m_barrelPosition2) < 3.f)
	{
		m_isDestroyed = true;
		m_playerScore += 1;
		m_barrelPosition2 = GenerateRandomPositionOnTrack();
	}
}

// A function that deals damage to the player and checks if they are dead or not.
void Game::DamagePlayer(int damage) 
{
	if (m_playerHealth > 0)
	{
		m_playerHealth -= damage;
	}
	if(m_playerHealth <= 0)
	{
		m_isDead = true;
	}
}

// A function that generates a random position along the track with a random offset in the left and right direction on the track.
// Makes use of srand() and rand() which from my research does not seem to be the most efficient solution so would be looked into at a later date.
// srand() is called in the constructor.
glm::vec3 Game::GenerateRandomPositionOnTrack()
{	
	int size = (int)m_pCatmullRom->GetCentrelinePoints().size();
	int randPosition = std::rand() % size;

	glm::vec3 p = m_pCatmullRom->GetCentrelinePoints()[randPosition];
	glm::vec3 pNext = m_pCatmullRom->GetCentrelinePoints()[(randPosition + 1) % m_pCatmullRom->GetCentrelinePoints().size()];
	glm::vec3 T = glm::normalize(pNext - p);
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	int offset = std::rand() % 9 + -4;
	return p + ((float)offset * N);
	
}

void Game::PlayerController()
{
	// Pressing E shoots the cannon. The cannon has a 3 second delay or until it hits an object before it can be shot again.
	if (GetKeyState('E') & 0x80) {
		m_hasShot = true;
		m_isDestroyed = false;
	}
	// A and D are used to strafe the player left and right along the track
	if (GetKeyState('A') & 0x80) {
		m_playerStrafe -= 0.01f * m_dt;
	}
	if (GetKeyState('D') & 0x80) {
		m_playerStrafe += 0.01f * m_dt;
	}
	// up and down arrow keys zoom in and zoom out the top down view camera respectively.
	if (GetKeyState(VK_UP) & 0x80) {
		m_topDownCamZoom -= 0.01f;
	}
	if (GetKeyState(VK_DOWN) & 0x80) {
		m_topDownCamZoom += 0.01f;
	}
}

void Game::DisplayFrameRateAndHud()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Show the Hud when the game is running
	if(!m_isDead)
	{
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		// renders the time survived text based on time passed onto the screen
		m_pFtFont->Render(width / 2 - 80, height - 20, 20, "Time survived: %d s", m_timePassed);
		// renders the player health onto the screen
		m_pFtFont->Render(width - width + 20, height - height + 20, 30, "Health: %d", m_playerHealth);
		// renders the player score onto the screen
		m_pFtFont->Render(width - 80, height - 20, 20, "Score: %d", m_playerScore);
		// renders current laps onto the screen
		m_pFtFont->Render(width - 200, height - 20, 20, "Laps: %d", (int)m_pCatmullRom->CurrentLap(m_currentDistance));

		// renders a text that appears onto the screen only if the player can shoot.
		if (!m_hasShot) 
		{
			m_pFtFont->Render(width - 200, height - height + 20, 20, "Cannon Loaded!");
		}

		// Collision text rendered onto screen for testing purposes
		/*
		m_pFtFont->Render(width - 200, height - 70, 20, "Coll cone 1: %d", (int)glm::length(m_playerPosition - m_conePosition1));
		m_pFtFont->Render(width - 200, height - 90, 20, "Coll cone 2: %d", (int)glm::length(m_playerPosition - m_conePosition2));
		m_pFtFont->Render(width - 200, height - 110, 20, "Coll cone 3: %d", (int)glm::length(m_playerPosition - m_conePosition3));
		m_pFtFont->Render(width - 200, height - 130, 20, "Coll cone 4: %d", (int)glm::length(m_playerPosition - m_conePosition4));
		m_pFtFont->Render(width - 200, height - 150, 20, "Coll barr 1: %d", (int)glm::length(m_playerPosition - m_barrelPosition1));
		m_pFtFont->Render(width - 200, height - 170, 20, "Coll bull 1: %d", (int)glm::length(m_bulletPosition - m_barrelPosition1));
		*/

	}
	// Moves the information into the middle of the screen after the player is defeated
	else if (m_isDead)
	{
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		// shows the time survived text based on time passed.
		m_pFtFont->Render(width / 2 - 160, (height / 2) + 30, 40, "Time survived: %d s", m_timePassed);
		m_pFtFont->Render(width / 2 - 80, (height / 2) - 30, 40, "Score: %d", m_playerScore);
	}

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// I created my timer code and code that controls how long the bullet will fly for here because it was more convinient, 
		// If I had more time I would refactor this out and make the same code in a different function which would not depend on the FPS function.
		if (!m_isDead) 
		{
			m_timePassed += 1;
		}
		if (m_hasShot)
		{
			m_timeSinceShot += 1;
		}

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
		
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	if(!m_isDead)
	{
		Update();
	}
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		// U,J are used for testing purposes, they control the speed of the tank
		case 'U':
			m_cameraSpeed += 0.01f;
			break;
		case 'J':
			if (m_cameraSpeed > 0.f) 
			{
				m_cameraSpeed -= 0.01f;
			}
			else
			{
				m_cameraSpeed = 0.f;
			}
			break;
		// pressing 1 changes the camera to third person mode
		case '1':
			m_firstPersonCam = false;
			m_thirdPersonCam = true;
			m_topDownCam = false;
			break;
		// pressing 2 changes the camera to first person mode
		case '2':
			m_firstPersonCam = true;
			m_thirdPersonCam = false;
			m_topDownCam = false;
			break;
		// pressing 3 toggles the camera to top down mode
		case '3':
			m_firstPersonCam = false;
			m_thirdPersonCam = false;
			m_topDownCam = true;
			break;
			
		// Test case for spotlights
		case '9':
			m_spotlightOn = !m_spotlightOn;
			break;
		// Test case for fog
		case '8':
			m_fogOn = !m_fogOn;
			break;

		// pressing 0 changes the camera to free fly mode
		case '0':
			m_firstPersonCam = false;
			m_thirdPersonCam = false;
			m_topDownCam = false;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
