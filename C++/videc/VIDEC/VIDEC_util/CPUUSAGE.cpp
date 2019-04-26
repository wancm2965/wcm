//
//CPUUSAGE.cpp
#include "CPUUSAGE.h"

#ifdef _WIN32

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

	
CPUUsage::CPUUsage()
{
    memset(&liOldIdleTime,0,sizeof(liOldIdleTime));
    memset(&liOldSystemTime,0,sizeof(liOldSystemTime));

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
                                          GetModuleHandle("ntdll"),
                                         "NtQuerySystemInformation"
                                         );
}

CPUUsage::~CPUUsage()
{
}

UINT CPUUsage::GetUsage(void)
{
	UINT	uiUsage=0;

    if (!NtQuerySystemInformation)
        return 0;

    // get number of processors in the system
    status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
    if (status != NO_ERROR)
        return 0;
    
    // get new system time
	status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
    if (status!=NO_ERROR)
        return 0;

    // get new CPU's idle time
    status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
    if (status != NO_ERROR)
        return 0;

    // if it's a first call - skip it
   if (liOldIdleTime.QuadPart != 0)
   {
        // CurrentValue = NewValue - OldValue
        dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
        dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

		if (dbSystemTime > 0)
		{
			// CurrentCpuIdle = IdleTime / SystemTime
			dbIdleTime = dbIdleTime / dbSystemTime;
		}
		else
		{
			dbIdleTime = 0;
		}

        // CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
        dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;

        uiUsage=(UINT)dbIdleTime;
   }

    // store new CPU's idle and system time
    liOldIdleTime = SysPerfInfo.liIdleTime;
    liOldSystemTime = SysTimeInfo.liKeSystemTime;
		
	return uiUsage;
}

#endif