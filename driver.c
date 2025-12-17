#include "driver.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrint("[driver] Loading...\n");

	// Set up function pointers
	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	DbgPrint("[driver] Loaded successfully\n");
	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	DbgPrint("[driver] Unloading...\n");
}

NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
	ULONG inSize = stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outSize = stack->Parameters.DeviceIoControl.OutputBufferLength;

	switch (controlCode) {
	case IOCTL_READ_MEMORY: {
		if (inSize < sizeof(READ_WRITE_REQUEST)) {
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		PREAD_WRITE_REQUEST req = (PREAD_WRITE_REQUEST)buffer;
		status = ReadProcessMemory(req->ProcessId, req->Address, req->Buffer, req->Size);
		break;
	}

	case IOCTL_WRITE_MEMORY: {
		if (inSize < sizeof(READ_WRITE_REQUEST)) {
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		PREAD_WRITE_REQUEST req = (PREAD_WRITE_REQUEST)buffer;
		status = WriteProcessMemory(req->ProcessId, req->Address, req->Buffer, req->Size);
		break;
	}

	case IOCTL_GET_PROCESS: {
		if (inSize < sizeof(PROCESS_REQUEST) || outSize < sizeof(PROCESS_REQUEST)) {
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		PPROCESS_REQUEST req = (PPROCESS_REQUEST)buffer;
		status = GetProcessIdByName(req->ProcessName, &req->ProcessId);
		break;
	}

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = (NT_SUCCESS(status)) ? outSize : 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}
