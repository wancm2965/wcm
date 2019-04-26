#pragma once

typedef struct
{
	DWORD   dwUnknown1;
	ULONG   uKeMaximumIncrement;
	ULONG   uPageSize;
	ULONG   uMmNumberOfPhysicalPages;
	ULONG   uMmLowestPhysicalPage;
	ULONG   uMmHighestPhysicalPage;
	ULONG   uAllocationGranularity;
	PVOID   pLowestUserAddress;
	PVOID   pMmHighestUserAddress;
	ULONG   uKeActiveProcessors;
	BYTE    bKeNumberProcessors;
	BYTE    bUnknown2;
	WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER   liIdleTime;
	DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG         uCurrentTimeZoneId;
	DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

class CPUUsage
{
public:
	CPUUsage(void);
	~CPUUsage(void);

public:
	UINT GetUsage(void);
protected:
	PROCNTQSI						NtQuerySystemInformation;
	SYSTEM_PERFORMANCE_INFORMATION	SysPerfInfo;
	SYSTEM_TIME_INFORMATION			SysTimeInfo;
	SYSTEM_BASIC_INFORMATION		SysBaseInfo;
	double							dbIdleTime;
	double							dbSystemTime;
	LONG							status;
	LARGE_INTEGER					liOldIdleTime;
	LARGE_INTEGER					liOldSystemTime;
};
