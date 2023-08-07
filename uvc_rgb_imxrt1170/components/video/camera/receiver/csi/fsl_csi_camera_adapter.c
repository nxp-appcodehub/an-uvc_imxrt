/*
 * Copyright 2017, 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_camera.h"
#include "fsl_camera_receiver.h"
#include "fsl_csi.h"
#include "fsl_csi_camera_adapter.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_Init(camera_receiver_handle_t *handle,
                                 const camera_config_t *config,
                                 camera_receiver_callback_t callback,
                                 void *userData);
#else
static status_t CSI_ADAPTER_Init(camera_receiver_handle_t *handle,
                                 const camera_config_t *config,
                                 camera_receiver_callback_t callback,
                                 void *userData,
                                 uint8_t * fragDmaBufferAddr[],
                                 uint32_t fragDmaBufferLine);
#endif

static status_t CSI_ADAPTER_Deinit(camera_receiver_handle_t *handle);

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_Start(camera_receiver_handle_t *handle);
#else
static status_t CSI_ADAPTER_Start(camera_receiver_handle_t *handle, uint32_t fragBufferAddr);
#endif

static status_t CSI_ADAPTER_Stop(camera_receiver_handle_t *handle);

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_SubmitEmptyBuffer(camera_receiver_handle_t *handle, uint32_t buffer);

static status_t CSI_ADAPTER_GetFullBuffer(camera_receiver_handle_t *handle, uint32_t *buffer);
#endif

#if !CSI_DRIVER_FRAG_MODE
static void CSI_ADAPTER_Callback(CSI_Type *base, csi_handle_t *handle, status_t status, void *userData);
#else
static void CSI_ADAPTER_Callback(CSI_Type *base, csi_frag_handle_t *handle, status_t status, void *userData);
#endif

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_InitExt(camera_receiver_handle_t *handle,
                                    const camera_config_t *config,
                                    const void *specialConfig,
                                    camera_receiver_callback_t callback,
                                    void *userData);
#else
static status_t CSI_ADAPTER_InitExt(camera_receiver_handle_t *handle,
                                    const camera_config_t *config,
                                    const void *specialConfig,
                                    camera_receiver_callback_t callback,
                                    void *userData,
									uint8_t * fragDmaBufferAddr[],
                                    uint32_t fragDmaBufferLine);
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
const camera_receiver_operations_t csi_ops = {.init              = CSI_ADAPTER_Init,
                                              .deinit            = CSI_ADAPTER_Deinit,
                                              .start             = CSI_ADAPTER_Start,
                                              .stop              = CSI_ADAPTER_Stop,
#if !CSI_DRIVER_FRAG_MODE
                                              .submitEmptyBuffer = CSI_ADAPTER_SubmitEmptyBuffer,
                                              .getFullBuffer     = CSI_ADAPTER_GetFullBuffer,
#endif
                                              .init_ext          = CSI_ADAPTER_InitExt};

/*******************************************************************************
 * Code
 ******************************************************************************/

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_Init(camera_receiver_handle_t *handle,
                                 const camera_config_t *config,
                                 camera_receiver_callback_t callback,
                                 void *userData)
#else
static status_t CSI_ADAPTER_Init(camera_receiver_handle_t *handle,
                                 const camera_config_t *config,
                                 camera_receiver_callback_t callback,
                                 void *userData,
                                 uint8_t * fragDmaBufferAddr[],
                                 uint32_t fragDmaBufferLine)
#endif
{
    csi_resource_t *resource        = (csi_resource_t *)(handle->resource);
    csi_private_data_t *privateData = (csi_private_data_t *)(handle->privateData);
    
    privateData->callback = callback;
    privateData->userData = userData;

#if !CSI_DRIVER_FRAG_MODE
    csi_config_t csiConfig;

    CSI_GetDefaultConfig(&csiConfig);

    csiConfig.bytesPerPixel = config->bytesPerPixel;

    if (kCAMERA_InterfaceGatedClock == config->interface)
    {
        csiConfig.workMode = kCSI_GatedClockMode;
    }
    else if (kCAMERA_InterfaceNonGatedClock == config->interface)
    {
        csiConfig.workMode = kCSI_NonGatedClockMode;
    }
    else if (kCAMERA_InterfaceCCIR656 == config->interface)
    {
        csiConfig.workMode = kCSI_CCIR656ProgressiveMode;
    }
    else
    {
        return kStatus_InvalidArgument;
    }

    csiConfig.linePitch_Bytes = config->frameBufferLinePitch_Bytes;
    csiConfig.dataBus         = resource->dataBus;
    csiConfig.useExtVsync     = true;
    csiConfig.height          = FSL_VIDEO_EXTRACT_HEIGHT(config->resolution);
    csiConfig.width           = FSL_VIDEO_EXTRACT_WIDTH(config->resolution);

    csiConfig.polarityFlags = 0U;
    if ((uint32_t)kCAMERA_HrefActiveHigh == (config->controlFlags & (uint32_t)kCAMERA_HrefActiveHigh))
    {
        csiConfig.polarityFlags |= (uint32_t)kCSI_HsyncActiveHigh;
    }
    if ((uint32_t)kCAMERA_DataLatchOnRisingEdge == (config->controlFlags & (uint32_t)kCAMERA_DataLatchOnRisingEdge))
    {
        csiConfig.polarityFlags |= (uint32_t)kCSI_DataLatchOnRisingEdge;
    }
    if ((uint32_t)kCAMERA_VsyncActiveHigh != (config->controlFlags & (uint32_t)kCAMERA_VsyncActiveHigh))
    {
        csiConfig.polarityFlags |= (uint32_t)kCSI_VsyncActiveLow;
    }

    (void)CSI_Init(resource->csiBase, &csiConfig);

    return CSI_TransferCreateHandle(resource->csiBase, &(privateData->csiHandle), CSI_ADAPTER_Callback,
                                    (void *)(handle));
#else
    CSI_FragModeInit(resource->csiBase);
    
    csi_frag_config_t csiFragConfig = {
        .width               = FSL_VIDEO_EXTRACT_WIDTH(config->resolution),
        .height              = FSL_VIDEO_EXTRACT_HEIGHT(config->resolution),
        .polarityFlags       = kCSI_HsyncActiveHigh | kCSI_DataLatchOnRisingEdge | kCSI_VsyncActiveLow,
        .workMode            = kCSI_GatedClockMode,
        .useExtVsync         = true,
        .dmaBufferLine       = fragDmaBufferLine,
        .isDmaBufferCachable = true,
    };

    if(fragDmaBufferAddr != NULL)
    {
        csiFragConfig.dmaBufferAddr[0]    = fragDmaBufferAddr[0];
        csiFragConfig.dmaBufferAddr[1]    = fragDmaBufferAddr[1];
		csiFragConfig.dmaBufferAddr[2]    = fragDmaBufferAddr[2];
        csiFragConfig.dmaBufferAddr[3]    = fragDmaBufferAddr[3];
    }
    else
    {
        csiFragConfig.dmaBufferAddr[0]    = NULL;
        csiFragConfig.dmaBufferAddr[1]    = NULL;
		csiFragConfig.dmaBufferAddr[2]    = NULL;
        csiFragConfig.dmaBufferAddr[3]    = NULL;
    }
    
    return CSI_FragModeCreateHandle(resource->csiBase, &(privateData->csiFragHandle), &csiFragConfig, CSI_ADAPTER_Callback,
                                    (void *)(handle));
#endif
}

static status_t CSI_ADAPTER_Deinit(camera_receiver_handle_t *handle)
{
    CSI_Deinit(((csi_resource_t *)handle->resource)->csiBase);

    return kStatus_Success;
}

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_Start(camera_receiver_handle_t *handle)
#else
static status_t CSI_ADAPTER_Start(camera_receiver_handle_t *handle, uint32_t fragBufferAddr)
#endif
{
#if !CSI_DRIVER_FRAG_MODE
    return CSI_TransferStart(((csi_resource_t *)handle->resource)->csiBase,
                             &(((csi_private_data_t *)(handle->privateData))->csiHandle));
#else
    csi_frag_capture_config_t csi_frag_capture_config = {
        .buffer = fragBufferAddr,
        .outputGrayScale = false,
        .window = NULL
    };
    return CSI_FragModeTransferCaptureImage(((csi_resource_t *)handle->resource)->csiBase,
                                          &(((csi_private_data_t *)(handle->privateData))->csiFragHandle),
                                          &csi_frag_capture_config);
#endif
}

static status_t CSI_ADAPTER_Stop(camera_receiver_handle_t *handle)
{
#if !CSI_DRIVER_FRAG_MODE
    return CSI_TransferStop(((csi_resource_t *)handle->resource)->csiBase,
                            &(((csi_private_data_t *)(handle->privateData))->csiHandle));
#else
    CSI_FragModeTransferAbortCaptureImage(((csi_resource_t *)handle->resource)->csiBase,
                                                 &(((csi_private_data_t *)(handle->privateData))->csiFragHandle));
    
    return kStatus_Success;
#endif
}

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_SubmitEmptyBuffer(camera_receiver_handle_t *handle, uint32_t buffer)
{
    return CSI_TransferSubmitEmptyBuffer(((csi_resource_t *)handle->resource)->csiBase,
                                         &(((csi_private_data_t *)(handle->privateData))->csiHandle), buffer);
}

static status_t CSI_ADAPTER_GetFullBuffer(camera_receiver_handle_t *handle, uint32_t *buffer)
{
    return CSI_TransferGetFullBuffer(((csi_resource_t *)handle->resource)->csiBase,
                                     &(((csi_private_data_t *)(handle->privateData))->csiHandle), buffer);
}
#endif

#if !CSI_DRIVER_FRAG_MODE
static void CSI_ADAPTER_Callback(CSI_Type *base, csi_handle_t *handle, status_t status, void *userData)
#else
static void CSI_ADAPTER_Callback(CSI_Type *base, csi_frag_handle_t *handle, status_t status, void *userData)
#endif
{
    camera_receiver_handle_t *cameraReceiverHandle = (camera_receiver_handle_t *)userData;
    csi_private_data_t *privateData                = (csi_private_data_t *)(cameraReceiverHandle->privateData);

    if (NULL != privateData->callback)
    {
        privateData->callback(cameraReceiverHandle, status, privateData->userData);
    }
}

#if !CSI_DRIVER_FRAG_MODE
static status_t CSI_ADAPTER_InitExt(camera_receiver_handle_t *handle,
                                    const camera_config_t *config,
                                    const void *specialConfig,
                                    camera_receiver_callback_t callback,
                                    void *userData)
{
    return CSI_ADAPTER_Init(handle, config, callback, userData);
}
#else
static status_t CSI_ADAPTER_InitExt(camera_receiver_handle_t *handle,
                                    const camera_config_t *config,
                                    const void *specialConfig,
                                    camera_receiver_callback_t callback,
                                    void *userData,
									uint8_t * fragDmaBufferAddr[],
                                    uint32_t fragDmaBufferLine)
{
    return CSI_ADAPTER_Init(handle, config, callback, userData, fragDmaBufferAddr, fragDmaBufferLine);
}
#endif
