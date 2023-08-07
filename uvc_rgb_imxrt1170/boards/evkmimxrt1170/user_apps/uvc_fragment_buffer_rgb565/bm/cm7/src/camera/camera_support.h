/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CAMERA_SUPPORT_H_
#define _CAMERA_SUPPORT_H_

#include "fsl_camera.h"
#include "fsl_camera_receiver.h"
#include "fsl_camera_device.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define DEMO_CAMERA_HEIGHT        480
#define DEMO_CAMERA_WIDTH         640
#define DEMO_CAMERA_FRAME_RATE    30
#define DEMO_CAMERA_CONTROL_FLAGS (kCAMERA_HrefActiveHigh | kCAMERA_DataLatchOnRisingEdge)
#define DEMO_CAMERA_BUFFER_ALIGN  64
#define DEMO_CAMERA_MIPI_CSI_LANE 2
#define DEMO_CAMERA_BUFFER_BPP    4
#define DEMO_CAMERA_FRAME_BYTES   (DEMO_CAMERA_HEIGHT * DEMO_CAMERA_WIDTH * DEMO_CAMERA_BUFFER_BPP)

#define DEMO_CAMERA_BUFFER_LINE   8
#define DEMO_CAMERA_BUFFER_BYTES  (DEMO_CAMERA_BUFFER_LINE * DEMO_CAMERA_WIDTH * DEMO_CAMERA_BUFFER_BPP)

#define UVC_FRAME_HEIGHT        480
#define UVC_FRAME_WIDTH         640
#define UVC_FRAME_BUFFER_ALIGN  64
#define UVC_FRAME_BUFFER_BPP    2
#define UVC_FRAME_BYTES         (UVC_FRAME_HEIGHT * UVC_FRAME_WIDTH * UVC_FRAME_BUFFER_BPP)

#define UVC_BUFFER_BYTES        (DEMO_CAMERA_BUFFER_LINE * UVC_FRAME_WIDTH * UVC_FRAME_BUFFER_BPP)
#define UVC_BUFFER_COUNT        4

extern camera_device_handle_t cameraDevice;
extern camera_receiver_handle_t cameraReceiver;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* This function should be called before camera pins initialization */
void BOARD_EarlyInitCamera(void);

void BOARD_InitCameraResource(void);

void BOARD_InitMipiCsi(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _CAMERA_SUPPORT_H_ */
