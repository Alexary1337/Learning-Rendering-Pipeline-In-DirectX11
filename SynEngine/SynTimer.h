#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>

class SynTimer
{
public:
	SynTimer();
	SynTimer(const SynTimer&);
	~SynTimer();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};

#endif