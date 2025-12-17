#pragma once
#include <ntddk.h>
#include <wdm.h>
#include <ntdef.h>
#include <ntstrsafe.h>

// Undocumented structures
typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    PVOID Reserved2;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved7[6];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemProcessInformation = 5
} SYSTEM_INFORMATION_CLASS;

// External declarations
NTKERNELAPI NTSTATUS NTAPI ZwQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

NTKERNELAPI NTSTATUS NTAPI PsLookupProcessByProcessId(
    HANDLE ProcessId,
    PEPROCESS* Process
);

NTKERNELAPI VOID NTAPI KeStackAttachProcess(
    PEPROCESS Process,
    PKAPC_STATE ApcState
);

NTKERNELAPI VOID NTAPI KeUnstackDetachProcess(
    PKAPC_STATE ApcState
);

// IOCTL codes
#define IOCTL_BASE 0x800
#define IOCTL_READ_MEMORY  CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_PROCESS  CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x3, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Structures
typedef struct _READ_WRITE_REQUEST {
    ULONG ProcessId;
    PVOID Address;
    PVOID Buffer;
    SIZE_T Size;
} READ_WRITE_REQUEST, * PREAD_WRITE_REQUEST;

typedef struct _PROCESS_REQUEST {
    WCHAR ProcessName[260];
    ULONG ProcessId;
} PROCESS_REQUEST, * PPROCESS_REQUEST;

// Function declarations
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS ReadProcessMemory(ULONG ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);
NTSTATUS WriteProcessMemory(ULONG ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);
NTSTATUS GetProcessIdByName(PWCH ProcessName, PULONG ProcessId);