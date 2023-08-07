/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"

#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "fsl_debug_console.h"

#include "fsl_camera_device.h"
#include "fsl_camera_receiver.h"
#include "camera_device.h"

#include "video_camera.h"

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void USB_DeviceApplicationInit(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint32_t FrameBufferPointer1, FrameBufferPointer2;
volatile bool frameAvailable = false;

extern camera_device_handle_t cameraDevice;
extern camera_receiver_handle_t cameraReceiver;
extern usb_video_camera_struct_t g_UsbDeviceVideoCamera;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief The main function.
 */
void main(void)
{
    /* Configure MPU */
    BOARD_ConfigMPU();

    /* Configure clock */
    BOARD_BootClockRUN();
    SEMC->SDRAMCR3 |= SEMC_SDRAMCR3_REN_MASK;

    /* Configure pin mux. */
    BOARD_InitDEBUG_UARTPins();
    BOARD_InitSDRAMPins();
    BOARD_InitLPI2C1Pins();
    BOARD_InitCSIPins();

    /* Configure debug console. */
    BOARD_InitDebugConsole();

    InitLPI2C1();
    InitCamera();
    USB_DeviceApplicationInit();

    PRINTF("This is a USB video demo application.\r\n");
    
    CAMERA_DEVICE_Start(&cameraDevice);
    CAMERA_RECEIVER_Start(&cameraReceiver);

    /* Wait to get the full frame buffer to show. */
    while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &FrameBufferPointer1))
    {
    }

    /* Wait to get the full frame buffer to show. */
    while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &FrameBufferPointer2))
    {
    }

    USB_DeviceRun(g_UsbDeviceVideoCamera.deviceHandle);

    while (1U)
    {
#if USB_DEVICE_CONFIG_USE_TASK
        USB_DeviceTaskFn(g_UsbDeviceVideoCamera.deviceHandle);
#endif

        if(!frameAvailable)
        {
            CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, FrameBufferPointer2);
            FrameBufferPointer2 = FrameBufferPointer1;
            /* Wait to get the full frame buffer. */
            while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &FrameBufferPointer1))
            {
            }
            frameAvailable = true;
        }
    }
}