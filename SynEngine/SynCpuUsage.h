#ifndef _CPUUSAGE_H_
#define _CPUUSAGE_H_

#pragma comment(lib, "pdh.lib")

#include "common.h"
#include <pdh.h>

class SynCpuUsage
{
public:
	SynCpuUsage();
	SynCpuUsage(const SynCpuUsage&);
	~SynCpuUsage();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

#endif