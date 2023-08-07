/*
 * Copyright (c) 2017, NXP
 * All rights reserved.
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

#include "board.h"
#include "pin_mux.h"

#include "fsl_gpio.h"
#include "fsl_csi.h"

#include "fsl_camera.h"
#include "fsl_camera_device.h"
#include "fsl_camera_receiver.h"
#include "fsl_csi_camera_adapter.h"
#include "fsl_mt9m114.h"
#include "camera_device.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void BOARD_PullCameraResetPin(bool pullUp);
static void BOARD_PullCameraPowerDownPin(bool pullUp);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static mt9m114_resource_t mt9m114Resource = {
    .twoWireIfI2C = APP_CAMERA_I2C,
    .pullResetPin = BOARD_PullCameraResetPin,
    .pullPowerDownPin = BOARD_PullCameraPowerDownPin,
    .inputClockFreq_Hz = 24000000,
};

camera_device_handle_t cameraDevice = {
    .resource = &mt9m114Resource,
    .ops = &mt9m114_ops,
};

static csi_resource_t csiResource = {
    .csiBase = CSI,
};

static csi_private_data_t csiPrivateData;

camera_receiver_handle_t cameraReceiver = {
    .resource = &csiResource,
    .ops = &csi_ops,
    .privateData = &csiPrivateData,
};

AT_NONCACHEABLE_SECTION_ALIGN(static uint16_t s_cameraFrameBuffer[CAMERA_FRAME_BUFFER_COUNT][CAMERA_VERTICAL_POINTS * CAMERA_HORIZONTAL_POINTS + 32u], FRAME_BUFFER_ALIGN);

/*******************************************************************************
 * Code
 ******************************************************************************/

extern void CSI_DriverIRQHandler(void);

void CSI_IRQHandler(void)
{
    CSI_DriverIRQHandler();
}

static void BOARD_PullCameraResetPin(bool pullUp)
{
    /* Reset pin is connected to DCDC_3V3. */
    return;
}

static void BOARD_PullCameraPowerDownPin(bool pullUp)
{
    if (pullUp)
    {
        GPIO_PinWrite(GPIO1, 4, 1);
    }
    else
    {
        GPIO_PinWrite(GPIO1, 4, 0);
    }
}

/*!
 * @brief Initialize the LPI2C1.
 */
void InitLPI2C1(void)
{
    /* Configure LPI2C. */
    lpi2c_master_config_t i2cMasterConfig;

    LPI2C_MasterGetDefaultConfig(&i2cMasterConfig);
    i2cMasterConfig.baudRate_Hz = 100000;
    i2cMasterConfig.debugEnable = true;
    i2cMasterConfig.ignoreAck = true;

    CLOCK_SetMux(kCLOCK_Lpi2cMux, 1);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 0);

    LPI2C_MasterInit(APP_CAMERA_I2C, &i2cMasterConfig, CLOCK_GetOscFreq());
}

/*!
 * @brief Initialize the camera interface.
 */
void InitCamera(void)
{
    /* Configure CSI MCLK. */
    CLOCK_SetMux(kCLOCK_CsiMux, 0);
    CLOCK_SetDiv(kCLOCK_CsiDiv, 0);

    /* Set the pins for CSI reset and power down. */
    const gpio_pin_config_t pinConfig = {
        kGPIO_DigitalOutput, 1,
    };
    GPIO_PinInit(GPIO1, 4, &pinConfig);

    /* Configure camera device and receiver.*/
    const camera_config_t cameraConfig = {
        .pixelFormat = kVIDEO_PixelFormatRGB565,
        .bytesPerPixel = CAMERA_BYTES_PER_PIXEL,
        .resolution = FSL_VIDEO_RESOLUTION(CAMERA_HORIZONTAL_POINTS, CAMERA_VERTICAL_POINTS),
        .frameBufferLinePitch_Bytes = CAMERA_HORIZONTAL_POINTS * CAMERA_BYTES_PER_PIXEL,
        .interface = kCAMERA_InterfaceGatedClock,
        .controlFlags = CAMERA_CONTROL_FLAGS,
        .framePerSec = 30,
    };
    CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);
    CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, NULL, NULL);

    memset(s_cameraFrameBuffer, 0, sizeof(s_cameraFrameBuffer));

    /* Submit the empty frame buffers to buffer queue. */
    for (uint32_t i = 0; i < CAMERA_FRAME_BUFFER_COUNT; i++)
    {
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(s_cameraFrameBuffer[i] + 32u));
    }

}
