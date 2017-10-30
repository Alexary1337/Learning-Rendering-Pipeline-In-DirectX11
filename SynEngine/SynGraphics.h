#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "common.h"
#include "synd3d.h"
#include "syncamera.h"
#include "synmodel.h"
#include "syncolorshader.h"
#include "synlight.h"
#include "syntext.h"
#include "synskydome.h"
#include "synskydomeshader.h"
#include "synposition.h"
#include "syntimer.h"
#include "synfpscounter.h"
#include "syncpuusage.h"
#include "syninput.h"
#include "synbitmap.h"

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
	bool TurnWF();

private:
	bool HandleInput(float frameTime);
	bool Render();

private:
	SynD3D* m_D3D;
	SynCamera* m_Camera;
	SynModel* m_Model;
	SynColorShader* m_ColorShader;
	SynLight* m_Light;
	SynText* m_Text;
	SynSkyDome* m_SkyDome;
	SynSkyDomeShader* m_SkyDomeShader;
	SynPosition* m_Position;
	SynTimer* m_Timer;
	SynFpsCounter* m_FpsCounter;
	SynCpuUsage* m_CpuUsage;
	SynInput* m_Input;
	int* m_meshCount;
	int* m_totalIndexCount;
};

#endif