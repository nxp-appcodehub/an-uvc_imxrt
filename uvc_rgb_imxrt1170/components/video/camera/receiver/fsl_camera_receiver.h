/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CAMERA_RECEIVER_H_
#define _FSL_CAMERA_RECEIVER_H_

#include "fsl_common.h"
#include "fsl_camera.h"
#include "fsl_csi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _camera_receiver_operations camera_receiver_operations_t;

/*! @brief Camera receiver handle. */
typedef struct _camera_receiver_handle
{
    void *resource;
    const camera_receiver_operations_t *ops;
    void *privateData;
} camera_receiver_handle_t;

/*!
 * @brief Camera receiver callback when new frame received.
 *
 * Callback function executes when new frame received successfully, the status
 * kStatus_Success is passed to upper layer.
 */
typedef void (*camera_receiver_callback_t)(camera_receiver_handle_t *handle, status_t status, void *userData);

/*! @brief Camera receiver operations. */
struct _camera_receiver_operations
{
#if !CSI_DRIVER_FRAG_MODE
    status_t (*init)(camera_receiver_handle_t *handle,
                     const camera_config_t *config,
                     camera_receiver_callback_t callback,
                     void *userData);                                                 /*!< Init the receiver. */
#else
    status_t (*init)(camera_receiver_handle_t *handle,
                     const camera_config_t *config,
                     camera_receiver_callback_t callback,
                     void *userData,
                     uint8_t * fragDmaBufferAddr[],
                     uint32_t DEMO_CSI_DMA_BUFFER_LINE);                              /*!< Init the receiver. */
#endif
    status_t (*deinit)(camera_receiver_handle_t *handle);                             /*!< Deinit the receiver. */
#if !CSI_DRIVER_FRAG_MODE
    status_t (*start)(camera_receiver_handle_t *handle);                              /*!< Start the receiver. */
#else
    status_t (*start)(camera_receiver_handle_t *handle, uint32_t fragBufferAddr);     /*!< Start the receiver. */
#endif
    status_t (*stop)(camera_receiver_handle_t *handle);                               /*!< Stop the receiver. */

#if !CSI_DRIVER_FRAG_MODE
    status_t (*submitEmptyBuffer)(camera_receiver_handle_t *handle, uint32_t buffer); /*!< Submit the empty
                                                                                        frame buffer to receiver
                                                                                        buffer queue. */
    status_t (*getFullBuffer)(camera_receiver_handle_t *handle, uint32_t *buffer);    /*!< Get the full-filled
                                                                                           frame buffer from
                                                                                           the receiver buffer queue. */
#endif

#if !CSI_DRIVER_FRAG_MODE
    status_t (*init_ext)(camera_receiver_handle_t *handle,
                         const camera_config_t *config,
                         const void *specialConfig,
                         camera_receiver_callback_t callback,
                         void *userData); /*!< Init the receiver with specific configuration. */
#else
    status_t (*init_ext)(camera_receiver_handle_t *handle,
                         const camera_config_t *config,
                         const void *specialConfig,
                         camera_receiver_callback_t callback,
                         void *userData,
						 uint8_t * fragDmaBufferAddr[],
                         uint32_t fragDmaBufferLine); /*!< Init the receiver with specific configuration. */
#endif
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the camera receiver with user defined configuration.
 *
 * @param handle Camera receiver handle.
 * @param config Pointer to the user-defined configuration structure.
 * @param callback Callback when new frame received.
 * @param userData Parameter of the callback.
 * @return Returns @ref kStatus_Success if initialize success, otherwise returns
 * error code.
 */
#if !CSI_DRIVER_FRAG_MODE
static inline status_t CAMERA_RECEIVER_Init(camera_receiver_handle_t *handle,
                                            const camera_config_t *config,
                                            camera_receiver_callback_t callback,
                                            void *userData)
{
    return handle->ops->init(handle, config, callback, userData);
}
#else
static inline status_t CAMERA_RECEIVER_Init(camera_receiver_handle_t *handle,
                                            const camera_config_t *config,
                                            camera_receiver_callback_t callback,
                                            void *userData,
                                            uint8_t * fragDmaBufferAddr[],
                                            uint32_t fragDmaBufferLine)
{
    return handle->ops->init(handle, config, callback, userData, fragDmaBufferAddr, fragDmaBufferLine);
}
#endif

/*!
 * @brief Initializes the camera receiver with user defined configuration.
 *
 * This is an externed API of @ref CAMERA_RECEIVER_Init, it allow upper layer to
 * pass in receiver specific configuration. In this case, the
 * @ref CAMERA_RECEIVER_Init uses default specific configuration.
 *
 * @param handle Camera receiver handle.
 * @param config Pointer to the user-defined configuration structure.
 * @param specialConfig Pointer to the receiver specific configuration.
 * @param callback Callback when new frame received.
 * @param userData Parameter of the callback.
 * @return Returns @ref kStatus_Success if initialize success, otherwise returns
 * error code.
 */
#if !CSI_DRIVER_FRAG_MODE
static inline status_t CAMERA_RECEIVER_InitExt(camera_receiver_handle_t *handle,
                                               const camera_config_t *config,
                                               const void *specialConfig,
                                               camera_receiver_callback_t callback,
                                               void *userData)
{
    return handle->ops->init_ext(handle, config, specialConfig, callback, userData);
}
#else
static inline status_t CAMERA_RECEIVER_InitExt(camera_receiver_handle_t *handle,
                                               const camera_config_t *config,
                                               const void *specialConfig,
                                               camera_receiver_callback_t callback,
                                               void *userData,
                                               uint8_t * fragDmaBufferAddr[],
                                               uint32_t fragDmaBufferLine)
{
    return handle->ops->init_ext(handle, config, specialConfig, callback, userData, fragDmaBufferAddr, fragDmaBufferLine);
}
#endif

/*!
 * @brief Deinitialize the camera receiver.
 *
 * @param handle Camera receiver handle.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_RECEIVER_Deinit(camera_receiver_handle_t *handle)
{
    return handle->ops->deinit(handle);
}

/*!
 * @brief Start the camera receiver to save input frame to frame buffer queue.
 *
 * @param handle Camera receiver handle.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
#if !CSI_DRIVER_FRAG_MODE
static inline status_t CAMERA_RECEIVER_Start(camera_receiver_handle_t *handle)
{
    return handle->ops->start(handle);
}
#else
static inline status_t CAMERA_RECEIVER_Start(camera_receiver_handle_t *handle, uint32_t fragBufferAddr)
{
    return handle->ops->start(handle, fragBufferAddr);
}
#endif

/*!
 * @brief Stop the camera receiver from saving input frame to frame buffer queue.
 *
 * @param handle Camera receiver handle.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_RECEIVER_Stop(camera_receiver_handle_t *handle)
{
    return handle->ops->stop(handle);
}

#if !CSI_DRIVER_FRAG_MODE
/*!
 * @brief Submit the empty frame buffer to the camera receiver buffer queue.
 *
 * @param handle Camera receiver handle.
 * @param buffer The empty frame buffer to submit.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_RECEIVER_SubmitEmptyBuffer(camera_receiver_handle_t *handle, uint32_t buffer)
{
    return handle->ops->submitEmptyBuffer(handle, buffer);
}

/*!
 * @brief Get the full frame buffer from the camera receiver buffer queue.
 *
 * @param handle Camera receiver handle.
 * @param buffer The empty frame buffer address.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_RECEIVER_GetFullBuffer(camera_receiver_handle_t *handle, uint32_t *buffer)
{
    return handle->ops->getFullBuffer(handle, buffer);
}
#endif

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_CAMERA_RECEIVER_H_ */
