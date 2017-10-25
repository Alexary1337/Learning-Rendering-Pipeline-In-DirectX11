#include "synfpscounter.h"

SynFpsCounter::SynFpsCounter()
{

}

SynFpsCounter::SynFpsCounter(const SynFpsCounter& other)
{

}

SynFpsCounter::~SynFpsCounter()
{

}

void SynFpsCounter::Initialize()
{

	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}

void SynFpsCounter::Frame()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

int SynFpsCounter::GetFps()
{
	return m_fps;
}