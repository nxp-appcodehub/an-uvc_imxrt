/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_csi_camera_adapter.h"
#include "camera_support.h"
#include "fsl_pxp.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "video_camera.h"

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void DEMO_InitCamera(void);
static void DEMO_InitPxp(void);

extern void USB_DeviceApplicationInit(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t s_cameraBuffer[CSI_FRAG_BUFFER_COUNT][DEMO_CAMERA_BUFFER_BYTES], DEMO_CAMERA_BUFFER_ALIGN);
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t s_uvcBuffer[UVC_BUFFER_COUNT][UVC_BUFFER_BYTES + 64], UVC_FRAME_BUFFER_ALIGN);

static volatile uint32_t csiCurrentLine = 0u;
volatile uint32_t pxpCurrentLine = 0u;
volatile uint32_t uvcFbAvailable = 0u;
volatile uint32_t uvcFbAddr[UVC_BUFFER_COUNT];
static volatile uint32_t uvcFbFillIdx = 0u;

extern usb_video_camera_struct_t g_UsbDeviceVideoCamera;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief The main function.
 */
void main(void)
{
    bool pxpOnging = false;

    BOARD_ConfigMPU();
    BOARD_BootClockRUN();
    BOARD_EarlyInitCamera();
    BOARD_InitLpuartPins();
    BOARD_InitMipiCameraPins();
    BOARD_InitDebugConsole();

    memset(s_cameraBuffer, 0, sizeof(s_cameraBuffer));
    memset(s_uvcBuffer, 0, sizeof(s_uvcBuffer));

    DEMO_InitPxp();
    DEMO_InitCamera();

    USB_DeviceApplicationInit();
    USB_DeviceRun(g_UsbDeviceVideoCamera.deviceHandle);

    pxp_ps_buffer_config_t psBufferConfig = {
        .pixelFormat = kPXP_PsPixelFormatRGB888, /* Note: This is 32-bit per pixel */
        .swapByte    = false,
        .bufferAddrU = 0U,
        .bufferAddrV = 0U,
        .pitchBytes  = DEMO_CAMERA_WIDTH * DEMO_CAMERA_BUFFER_BPP,
    };

    /* Output config. */
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat    = kPXP_OutputPixelFormatRGB565,
        .interlacedMode = kPXP_OutputProgressive,
        .buffer1Addr    = 0U,
        .pitchBytes     = UVC_FRAME_WIDTH * UVC_FRAME_BUFFER_BPP,
        .width  = UVC_FRAME_WIDTH,
        .height = DEMO_CAMERA_BUFFER_LINE,
    };

    PXP_SetProcessSurfaceBackGroundColor(PXP, 0);
    PXP_SetProcessSurfaceScaler(PXP, DEMO_CAMERA_WIDTH, DEMO_CAMERA_BUFFER_LINE, UVC_FRAME_WIDTH, DEMO_CAMERA_BUFFER_LINE);

    for(uint32_t i = 0; i < UVC_BUFFER_COUNT; i++)
    {
        uvcFbAddr[i] = (uint32_t)s_uvcBuffer[i] + 64;
    }

    CAMERA_RECEIVER_Start(&cameraReceiver, NULL);

    while (1)
    {
        if (pxpCurrentLine < csiCurrentLine && uvcFbAvailable < UVC_BUFFER_COUNT)
        {
            if (!pxpOnging)
            {
                /* Convert the camera input picture to RGB format. */
                register uint32_t cameraBufIdx = (pxpCurrentLine / DEMO_CAMERA_BUFFER_LINE) % CSI_FRAG_BUFFER_COUNT;
                psBufferConfig.bufferAddr = (uint32_t)s_cameraBuffer[cameraBufIdx];
                PXP_SetProcessSurfaceBufferConfig(PXP, &psBufferConfig);

                outputBufferConfig.buffer0Addr = uvcFbAddr[uvcFbFillIdx];
                PXP_SetOutputBufferConfig(PXP, &outputBufferConfig);

                PXP_Start(PXP);
                pxpOnging = true;
                uvcFbFillIdx = (uvcFbFillIdx == UVC_BUFFER_COUNT - 1) ? 0 : uvcFbFillIdx + 1;
            }
            else
            {
                /* Wait for PXP process complete. */
                if (kPXP_CompleteFlag & PXP_GetStatusFlags(PXP))
                {
                    PXP_ClearStatusFlags(PXP, kPXP_CompleteFlag);

                    pxpCurrentLine += DEMO_CAMERA_BUFFER_LINE;
                    pxpOnging = false;
                    uvcFbAvailable++;
                }
            }
        }
    }
}

static void DEMO_InitPxp(void)
{
    /*
     * Configure the PXP for rotate and scale.
     */
    PXP_Init(PXP);

    PXP_SetProcessSurfaceBackGroundColor(PXP, 0U);

    PXP_SetProcessSurfacePosition(PXP, 0U, 0U, UVC_FRAME_WIDTH - 1U, UVC_FRAME_HEIGHT - 1U);

    /* Disable AS. */
    PXP_SetAlphaSurfacePosition(PXP, 0xFFFFU, 0xFFFFU, 0U, 0U);

    PXP_EnableCsc1(PXP, false);
}

void camera_callback(camera_receiver_handle_t *handle, status_t status, void *userData)
{
    csi_private_data_t *privateData;

    if (status == kStatus_CSI_FrameStart)
    {
        csiCurrentLine = 0u;
        pxpCurrentLine = 0u;

        uvcFbAvailable = 0;
        uvcFbFillIdx = 0;
    }

    if (status == kStatus_CSI_FragBufferFull)
    {
        privateData = (csi_private_data_t *)(handle->privateData);
        csiCurrentLine = privateData->csiFragHandle.dmaCurLine;
    }
}

static void DEMO_InitCamera(void)
{
    camera_config_t cameraConfig;

    memset(&cameraConfig, 0, sizeof(cameraConfig));

    BOARD_InitCameraResource();

    /* CSI input data bus is 24-bit, and save as XRGB8888.. */
    cameraConfig.pixelFormat                = kVIDEO_PixelFormatXRGB8888;
    cameraConfig.bytesPerPixel              = DEMO_CAMERA_BUFFER_BPP;
    cameraConfig.resolution                 = FSL_VIDEO_RESOLUTION(DEMO_CAMERA_WIDTH, DEMO_CAMERA_HEIGHT);
    cameraConfig.frameBufferLinePitch_Bytes = DEMO_CAMERA_WIDTH * DEMO_CAMERA_BUFFER_BPP;
    cameraConfig.interface                  = kCAMERA_InterfaceGatedClock;
    cameraConfig.controlFlags               = DEMO_CAMERA_CONTROL_FLAGS;
    cameraConfig.framePerSec                = DEMO_CAMERA_FRAME_RATE;

    uint8_t * fragDmaBufferAddr[] = {
        s_cameraBuffer[0],
        s_cameraBuffer[1],
        s_cameraBuffer[2],
        s_cameraBuffer[3],
    };
    CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, camera_callback, NULL, fragDmaBufferAddr, DEMO_CAMERA_BUFFER_LINE);

    BOARD_InitMipiCsi();

    cameraConfig.pixelFormat   = kVIDEO_PixelFormatRGB565;
    cameraConfig.bytesPerPixel = 2;
    cameraConfig.resolution    = FSL_VIDEO_RESOLUTION(DEMO_CAMERA_WIDTH, DEMO_CAMERA_HEIGHT);
    cameraConfig.interface     = kCAMERA_InterfaceMIPI;
    cameraConfig.controlFlags  = DEMO_CAMERA_CONTROL_FLAGS;
    cameraConfig.framePerSec   = DEMO_CAMERA_FRAME_RATE;
    cameraConfig.csiLanes      = DEMO_CAMERA_MIPI_CSI_LANE;
    CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);

    CAMERA_DEVICE_Start(&cameraDevice);
}

