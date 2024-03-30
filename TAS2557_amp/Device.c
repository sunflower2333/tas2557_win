/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, tas2557ampCreateDevice)
#endif

UINT8 g_MuteRegister = 0x07;
UINT8 g_DeviceMute = 3; // 3: mute, 0: unmute
UINT8 g_LastMuteValue = 3;
BOOLEAN
CheckMuteStatus(
    PDEVICE_CONTEXT pDevice
) {
    PAGED_CODE();
    SpbDeviceWriteRead(&pDevice->SpbContextA, &g_MuteRegister, &g_DeviceMute, sizeof(g_MuteRegister), sizeof(g_DeviceMute));
    return g_DeviceMute;
}

VOID
ShutdownAmp(PDEVICE_CONTEXT pDevice) {
    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Enter!");
    // delay Detected
    WA(0x2c, 0x00);
    WA(0x00, 0x00);
    WA(0x7f, 0x64);
    WA(0x07, 0x01);
    // delay Detected
    DELAY_MS(10)
    WA(0x00, 0x00);
    WA(0x7f, 0x00);
    WA(0x07, 0x03);
    WA(0x04, 0x60);
    // delay Detected
    DELAY_MS(2)
    WA(0x05, 0x00);
    WA(0x04, 0x00);
    WA(0x00, 0x01);
    WA(0x3d, 0x00);
    WA(0x3e, 0x00);
    WA(0x4d, 0x00);

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Exit!");
}

VOID
StartupAmp(PDEVICE_CONTEXT pDevice) {
    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Enter!");
    //TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! startup current mute reg: %d, last mute reg: %d", g_LastMuteValue, g_DeviceMute);
    // delay Detected
    WdfWaitLockAcquire(pDevice->StartLock, NULL);

    // There is no reg marked spkr is Muted(or i can not found)
    // these regs must be rewrite when power up I2S and ClassD Amp again.
    // (Although nothing changes in them)
// delay Detected
    // delay Detected
    //WA(0x2c, 0x00);
    
    /*WA(0x00, 0x00);
    WA(0x7f, 0x64);
    WA(0x07, 0x01);*/

    // delay Detected
    //WA(0x00, 0x00);
    //WA(0x7f, 0x00);
    //WA(0x07, 0x03);
    //WA(0x04, 0x60);

    // delay Detected
    //DELAY_MS(2);
    
    //WA(0x05, 0x00);
    //WA(0x04, 0x00);

    //WA(0x00, 0x01);
    //WA(0x3d, 0x00);
    //WA(0x3e, 0x00);
    //WA(0x4d, 0x00);
    
    // delay Detected
    //WA(0x00, 0x00);
    
    // delay Detected
    //DELAY_MS(1);
    //WA(0x00, 0x01);
    //WA(0x4d, 0x15);
    //WA(0x3d, 0x01);
    //WA(0x3e, 0x01);
    //WA(0x00, 0x00);
    //WA(0x05, 0xa0);
    WA(0x05, 0xa0);
    WA(0x04, 0xf8);
    
    // delay Detected
    WA(0x2c, 0x2b);
    //WA(0x07, 0x00);
    
    //DELAY_MS(2);
    //WA(0x00, 0x00);
    //WA(0x7f, 0x64);
    //WA(0x07, 0x00);
    //WA(0x00, 0x00);
    //WA(0x7f, 0x82);
    //WA(0x00, 0x02);



    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Finished Enable Amplifier.");
    WdfWaitLockRelease(pDevice->StartLock);
}

int CsAudioArg2 = 1;

VOID
CSAudioRegisterEndpoint(
    PDEVICE_CONTEXT pDevice
) {
    CsAudioArg arg;
    RtlZeroMemory(&arg, sizeof(CsAudioArg));
    arg.argSz = sizeof(CsAudioArg);
    arg.endpointType = CSAudioEndpointTypeSpeaker;
    arg.endpointRequest = CSAudioEndpointRegister;
    ExNotifyCallback(pDevice->CSAudioAPICallback, &arg, &CsAudioArg2);
}

VOID
CsAudioCallbackFunction(
    IN PDEVICE_CONTEXT pDevice,
    CsAudioArg* arg,
    PVOID Argument2
) {
    //TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "CsAudioCallbackFunction Enter.");
    if (!pDevice) {
        return;
    }

    if (Argument2 == &CsAudioArg2) {
        return;
    }

    pDevice->CSAudioManaged = TRUE;

    CsAudioArg localArg;
    RtlZeroMemory(&localArg, sizeof(CsAudioArg));
    RtlCopyMemory(&localArg, arg, min(arg->argSz, sizeof(CsAudioArg)));

    if (localArg.endpointType == CSAudioEndpointTypeDSP && localArg.endpointRequest == CSAudioEndpointRegister) {
        CSAudioRegisterEndpoint(pDevice);
    }
    else if (localArg.endpointType != CSAudioEndpointTypeSpeaker) {
        return;
    }

    if (localArg.endpointRequest == CSAudioEndpointStop) {
        ShutdownAmp(pDevice);
    }
    if (localArg.endpointRequest == CSAudioEndpointStart && !CheckMuteStatus(pDevice)) {
        StartupAmp(pDevice);
    }
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "CsAudioCallbackFunction Exit.");
}

NTSTATUS
StopAmp(PDEVICE_CONTEXT pDevice) {
    //
    // Mute Amplifier
    //
    NTSTATUS status = STATUS_SUCCESS;

    // TODO
    // mute
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "Muted Device! Status: %!STATUS!", status);
    ShutdownAmp(pDevice);
    
    // Stop Spb Controller
    //
    //WdfInterruptDisable(pDevice->Interrupt);
    //WdfIoTargetClose(pDevice->SpbContextA.SpbIoTarget);
    //WdfIoTargetClose(pDevice->SpbContextB.SpbIoTarget);
    

    return status;
}

NTSTATUS
OnD0Entry(
    _In_  WDFDEVICE               FxDevice,
    _In_  WDF_POWER_DEVICE_STATE  FxPreviousState
)
/*++

Routine Description:

This routine allocates objects needed by the driver.

Arguments:

FxDevice - a handle to the framework device object
FxPreviousState - previous power state

Return Value:

Status

--*/
{
    UNREFERENCED_PARAMETER(FxPreviousState);
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    //
    // Setup Frimware at the first time.
    //
    SetupfwAmp(pDevice);
    // Move to Book0 Page0
    WA(0x00, 0x00);
    WA(0x7f, 0x00);
    //StartupAmp(pDevice);

    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
OnD0Exit(
    _In_  WDFDEVICE               FxDevice,
    _In_  WDF_POWER_DEVICE_STATE  FxPreviousState
)
/*++

Routine Description:

This routine destroys objects needed by the driver.

Arguments:

FxDevice - a handle to the framework device object
FxPreviousState - previous power state

Return Value:

Status

--*/
{
    UNREFERENCED_PARAMETER(FxPreviousState);

    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");
    //if (pDevice->CSAudioAPICallbackObj) {
    //    ExUnregisterCallback(pDevice->CSAudioAPICallbackObj);
    //}

    if (pDevice->SpbContextA.SpbIoTarget != WDF_NO_HANDLE)
    {
        StopAmp(pDevice);
        WdfObjectDelete(pDevice->SpbContextA.SpbIoTarget);
        pDevice->SpbContextA.SpbIoTarget = WDF_NO_HANDLE;
    }


    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}

NTSTATUS
OnSelfManagedIoInit(
    _In_
    WDFDEVICE FxDevice
) {
    PDEVICE_CONTEXT pDevice = DeviceGetContext(FxDevice);
    NTSTATUS status = STATUS_SUCCESS;
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    // CS Audio Callback
    FuncEntry(TRACE_DEVICE);
    UNICODE_STRING CSAudioCallbackAPI;
    RtlInitUnicodeString(&CSAudioCallbackAPI, L"\\CallBack\\CsAudioCallbackAPI");


    OBJECT_ATTRIBUTES attributes;
    InitializeObjectAttributes(&attributes,
        &CSAudioCallbackAPI,
        OBJ_KERNEL_HANDLE | OBJ_OPENIF | OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
        NULL,
        NULL
    );

    status = ExCreateCallback(&pDevice->CSAudioAPICallback, &attributes, TRUE, TRUE);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    pDevice->CSAudioAPICallbackObj = ExRegisterCallback(pDevice->CSAudioAPICallback,
        CsAudioCallbackFunction,
        pDevice
    );

    if (!pDevice->CSAudioAPICallbackObj) {
        return STATUS_NO_CALLBACK_ACTIVE;
    }

    CSAudioRegisterEndpoint(pDevice);
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");


    return status;
}

NTSTATUS
tas2557ampCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Entering.");

    
    {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "Registering PnpCallbacks.");
        WDF_PNPPOWER_EVENT_CALLBACKS pnpCallbacks;
        WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpCallbacks);
        pnpCallbacks.EvtDevicePrepareHardware = OnPrepareHardware;
        pnpCallbacks.EvtDeviceReleaseHardware = OnReleaseHardware;
        pnpCallbacks.EvtDeviceSelfManagedIoInit = OnSelfManagedIoInit;
        pnpCallbacks.EvtDeviceD0Entry = OnD0Entry;
        pnpCallbacks.EvtDeviceD0Exit = OnD0Exit;
        WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpCallbacks);
    }


    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status)) {
        //
        // Get a pointer to the device context structure that we just associated
        // with the device object. We define this structure in the device.h
        // header file. DeviceGetContext is an inline function generated by
        // using the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in device.h.
        // This function will do the type checking and return the device context.
        // If you pass a wrong object handle it will return NULL and assert if
        // run under framework verifier mode.
        //

        {
            WDF_DEVICE_STATE deviceState;
            WDF_DEVICE_STATE_INIT(&deviceState);

            deviceState.NotDisableable = WdfFalse;
            WdfDeviceSetDeviceState(device, &deviceState);
        }

        deviceContext = DeviceGetContext(device);

        //
        // Initialize the context.
        //
        //deviceContext->PrivateDeviceData = 0;

        //
        // Create a device interface so that applications can find and talk
        // to us.
        //
        status = WdfDeviceCreateDeviceInterface(
            device,
            &GUID_DEVINTERFACE_tas2557amp,
            NULL // ReferenceString
            );
        //
        // Allocate a waitlock to guard access to the default buffers
        //
        status = WdfWaitLockCreate(
            WDF_NO_OBJECT_ATTRIBUTES,
            &deviceContext->StartLock);

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = tas2557ampQueueInitialize(device);
        }
    }


    TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! Leaving.");
    return status;
}
