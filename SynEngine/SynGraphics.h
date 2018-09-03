#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "common.h"
#include "synd3d.h"
#include "syncamera.h"
#include "synmodel.h"
#include "syncolorshader.h"
#include "synlight.h"
#include "synskydome.h"
#include "synskydomeshader.h"
#include "synposition.h"
#include "syntimer.h"
#include "synfpscounter.h"
#include "syncpuusage.h"
#include "syninput.h"
#include "synterrain.h"
#include "synterrainshader.h"
#include "SynGUI.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 7000.0f;
const float SCREEN_NEAR = 0.1f;

class SynGraphics
{
public:
	SynGraphics();
	SynGraphics(const SynGraphics&);
	~SynGraphics();

	bool Initialize(int, int, HWND, HINSTANCE);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float frameTime);
	bool Render();

private:
	SynD3D* m_D3D;
	SynCamera* m_Camera;
	SynModel* m_Model;
	SynColorShader* m_ColorShader;
	SynLight* m_Light;
	SynSkyDome* m_SkyDome;
	SynSkyDomeShader* m_SkyDomeShader;
	SynPosition* m_Position;
	SynTimer* m_Timer;
	SynFpsCounter* m_FpsCounter;
	SynCpuUsage* m_CpuUsage;
	SynInput* m_Input;
	SynTerrain* m_Terrain;
	SynTerrainShader* m_TerrainShader;
	SynGUI* m_GUI;
	int* m_meshCount;
	int* m_totalIndexCount;

	bool isCursorVisible = false;

	DirectX::XMFLOAT3* m_translationVector3D;
};

#endif