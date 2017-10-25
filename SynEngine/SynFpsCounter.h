#ifndef _FPSCOUNTER_H_
#define _FPSCOUNTER_H_

#pragma comment(lib, "winmm.lib")

#include "common.h"
#include <windows.h>
#include <mmsystem.h>

class SynFpsCounter
{
public:
	SynFpsCounter();
	SynFpsCounter(const SynFpsCounter&);
	~SynFpsCounter();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_fps, m_count;
	unsigned long m_startTime;
};

#endif
