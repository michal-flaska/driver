#pragma once
#include <ntddk.h>

// IOCTL codes for communication
#define IOCTL_BASE 0x800
#define IOCTL_READ_MEMORY  CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_PROCESS  CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x3, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Structures for data exchange
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

// Memory operations
NTSTATUS ReadProcessMemory(ULONG ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);
NTSTATUS WriteProcessMemory(ULONG ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);
NTSTATUS GetProcessIdByName(PWCH ProcessName, PULONG ProcessId);