#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class House;
class Tree;
class CHeightMapTerrain;
class CFrameBufferObject;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void RenderScene(int pass);
	void Render();

	void CreateAndRenderFBOQuad();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CPlane* m_pPlanarTerrain2;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	COpenAssetImportMesh *m_pTankMesh;
	COpenAssetImportMesh *m_pSandbagsMesh;
	COpenAssetImportMesh *m_pConeMesh;
	COpenAssetImportMesh *m_pBulletMesh;
	CSphere *m_pSphere;
	House * m_House;
	Tree* m_tree;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CHeightMapTerrain* m_pHeightmapTerrain;

	CCatmullRom* m_pCatmullRom;

	CFrameBufferObject* m_pFBO;

	CPlane* m_pPlane;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRateAndHud();
	glm::vec3 GenerateRandomPositionOnTrack();
	void DetectAndProcessCollision();
	void PlayerController();
	void DamagePlayer(int damage);
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	int m_timePassed;
	float m_currentDistance;
	float m_cameraSpeed;
	float m_cameraRotation;
	float m_topDownCamZoom;

	glm::vec3 m_playerPosition;
	glm::mat4 m_playerOrientation;
	float m_playerStrafe;

	glm::vec3 m_spotlightPos;
	glm::vec3 m_spotlightDir;
	bool m_spotlightOn;
	bool m_fogOn;

	glm::vec3 m_bulletPosition;
	glm::vec3 m_bulletDirection;
	bool m_hasSetStart;
	bool m_hasShot;
	bool m_isDestroyed;
	int m_timeSinceShot;

	float m_pathWidth;

	glm::vec3 m_conePosition1;
	glm::vec3 m_conePosition2;
	glm::vec3 m_conePosition3;
	glm::vec3 m_conePosition4;

	glm::vec3 m_barrelPosition1;
	glm::vec3 m_barrelPosition2;

	bool m_firstPersonCam;
	bool m_thirdPersonCam;
	bool m_topDownCam;

	int m_playerHealth;
	int m_playerScore;
	bool m_isDead;

	bool m_isBlurry;

	int m_lapTracker;


};
