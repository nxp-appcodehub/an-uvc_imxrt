/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
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

#ifndef __USB_DEVICE_DESCRIPTOR_H__
#define __USB_DEVICE_DESCRIPTOR_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define USB_DEVICE_SPECIFIC_BCD_VERSION (0x0200U)
#define USB_DEVICE_DEMO_BCD_VERSION (0x0101U)

#define USB_DEVICE_CLASS (0xEFU)
#define USB_DEVICE_SUBCLASS (0x02U)
#define USB_DEVICE_PROTOCOL (0x01U)

#define USB_DEVICE_MAX_POWER (0x32U)

#if defined(USB_DEVICE_VIDEO_CLASS_VERSION_1_5) && (USB_DEVICE_VIDEO_CLASS_VERSION_1_5 > 0U)

#define USB_DEVICE_VIDEO_SPECIFIC_BCD_VERSION (0x0150U)
#define USB_DEVICE_VIDEO_CAMERA_PROTOCOL USB_DEVICE_VIDEO_PC_PROTOCOL_15
#define USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL (sizeof(g_UsbDeviceConfigurationDescriptor))

#elif defined(USB_DEVICE_VIDEO_CLASS_VERSION_1_1) && (USB_DEVICE_VIDEO_CLASS_VERSION_1_1 > 0U)

#define USB_DEVICE_VIDEO_SPECIFIC_BCD_VERSION (0x0110U)
#define USB_DEVICE_VIDEO_CAMERA_PROTOCOL USB_DEVICE_VIDEO_PC_PROTOCOL_UNDEFINED
#define USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL (sizeof(g_UsbDeviceConfigurationDescriptor))

#else

#define USB_DEVICE_VIDEO_SPECIFIC_BCD_VERSION (0x0100U)
#define USB_DEVICE_VIDEO_CAMERA_PROTOCOL USB_DEVICE_VIDEO_PC_PROTOCOL_UNDEFINED
#define USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL (sizeof(g_UsbDeviceConfigurationDescriptor))

#endif

#define USB_DESCRIPTOR_LENGTH_INTERFACE_ASSOCIATION (0x08U)
#define USB_DESCRIPTOR_LENGTH_STRING0 (sizeof(g_UsbDeviceString0))
#define USB_DESCRIPTOR_LENGTH_STRING1 (sizeof(g_UsbDeviceString1))
#define USB_DESCRIPTOR_LENGTH_STRING2 (sizeof(g_UsbDeviceString2))
#define USB_DESCRIPTOR_LENGTH_STRING3 (sizeof(g_UsbDeviceString3))

#define USB_DEVICE_CONFIGURATION_COUNT (1U)
#define USB_DEVICE_STRING_COUNT (4U)
#define USB_DEVICE_LANGUAGE_COUNT (1U)

#define USB_VIDEO_CAMERA_CONFIGURE_INDEX (1U)

#define USB_VIDEO_CAMERA_CONTROL_INTERFACE_COUNT (1U)
#define USB_VIDEO_CAMERA_CONTROL_INTERFACE_INDEX (0U)
#define USB_VIDEO_CAMERA_CONTROL_ENDPOINT_COUNT (1U)
#define USB_VIDEO_CAMERA_CONTROL_ENDPOINT (1U)

#define HS_INTERRUPT_IN_PACKET_SIZE (8U)
#define FS_INTERRUPT_IN_PACKET_SIZE (8U)
#define HS_INTERRUPT_IN_INTERVAL (0x01U)
#define FS_INTERRUPT_IN_INTERVAL (0x08U)

#define USB_VIDEO_CAMERA_VC_INTERFACE_HEADER_LENGTH (0x0DU)
#define USB_VIDEO_CAMERA_VC_OUTPUT_TERMINAL_LENGTH (0x09U)

#if defined(USB_DEVICE_VIDEO_CLASS_VERSION_1_5) && (USB_DEVICE_VIDEO_CLASS_VERSION_1_5 > 0U)

#define USB_VIDEO_CAMERA_VC_PROCESSING_UNIT_LENGTH (0x0DU)
#define USB_VIDEO_CAMERA_VC_INPUT_TERMINAL_LENGTH (0x12U)

#elif defined(USB_DEVICE_VIDEO_CLASS_VERSION_1_1) && (USB_DEVICE_VIDEO_CLASS_VERSION_1_1 > 0U)

#define USB_VIDEO_CAMERA_VC_PROCESSING_UNIT_LENGTH (0x0CU)
#define USB_VIDEO_CAMERA_VC_INPUT_TERMINAL_LENGTH (0x12U)

#else

#define USB_VIDEO_CAMERA_VC_PROCESSING_UNIT_LENGTH (0x0BU)
#define USB_VIDEO_CAMERA_VC_INPUT_TERMINAL_LENGTH (0x12U)

#endif

#define USB_VIDEO_CAMERA_VC_INTERFACE_TOTAL_LENGTH                                             \
    (USB_VIDEO_CAMERA_VC_INTERFACE_HEADER_LENGTH + USB_VIDEO_CAMERA_VC_INPUT_TERMINAL_LENGTH + \
     USB_VIDEO_CAMERA_VC_OUTPUT_TERMINAL_LENGTH + USB_VIDEO_CAMERA_VC_PROCESSING_UNIT_LENGTH)

#define USB_VIDEO_CAMERA_CLOCK_FREQUENCY (6000000U) /* 6MHz */

#define USB_VIDEO_CAMERA_STREAM_INTERFACE_COUNT (1U)
#define USB_VIDEO_CAMERA_STREAM_INTERFACE_INDEX (1U)
#define USB_VIDEO_CAMERA_STREAM_ENDPOINT_COUNT (1U)
#define USB_VIDEO_CAMERA_STREAM_ENDPOINT_IN (2U)
#define HS_STREAM_IN_PACKET_ADDITIONAL_TRANSACTION (2U)  /* MAX Value is 2U*/
#define HS_STREAM_IN_PACKET_SIZE (1024U)
#define FS_STREAM_IN_PACKET_SIZE (512U)
#define HS_STREAM_IN_INTERVAL (0x01U)
#define FS_STREAM_IN_INTERVAL (0x01U)

#define USB_VIDEO_CAMERA_STREAM_STILL_CAPTURE_METHOD (0x02U)
#define USB_VIDEO_CAMERA_STREAM_STILL_CAPTURE_TRIGGER_SUPPOTED (0x00U)

#define USB_VIDEO_CAMERA_VS_INTERFACE_HEADER_LENGTH (0x0EU)
#define USB_VIDEO_UNCOMPRESSED_FORMAT_DESCRIPTOR_LENGTH (0x1BU)
#define USB_VIDEO_UNCOMPRESSED_FRAME_DESCRIPTOR_LENGTH (34U)
#define USB_VIDEO_UNCOMPRESSED_FRAME_STILL_DESCRIPTOR_LENGTH (0x0AU)
#define USB_VIDEO_CAMERA_VS_INTERFACE_TOTAL_LENGTH                                                   \
    (USB_VIDEO_CAMERA_VS_INTERFACE_HEADER_LENGTH + USB_VIDEO_UNCOMPRESSED_FORMAT_DESCRIPTOR_LENGTH + \
     USB_VIDEO_UNCOMPRESSED_FRAME_DESCRIPTOR_LENGTH + USB_VIDEO_UNCOMPRESSED_FRAME_STILL_DESCRIPTOR_LENGTH)

#define USB_VIDEO_CAMERA_INTERFACE_COUNT \
    (USB_VIDEO_CAMERA_CONTROL_INTERFACE_COUNT + USB_VIDEO_CAMERA_STREAM_INTERFACE_COUNT)

/* Stream format */
#define USB_VIDEO_CAMERA_FORMAT_COUNT (1U)
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FORMAT_INDEX (1U)

#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_COUNT (1U)
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_INDEX (1U)

#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_DATA_BITS (CAMERA_BYTES_PER_PIXEL)
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_WIDTH (CAMERA_HORIZONTAL_POINTS)
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_HEIGHT (CAMERA_VERTICAL_POINTS)

#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_INTERVAL_TYPE (2U)
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_INTERVAL_30FPS (10000000U / 30U)
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_INTERVAL_10FPS (10000000U / 10U)

#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_DEFAULT_INTERVAL USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_INTERVAL_30FPS
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_MIN_INTERVAL USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_INTERVAL_30FPS
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_MAX_INTERVAL USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_INTERVAL_10FPS

#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_MIN_BIT_RATE                                              \
    (USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_DATA_BITS * 8U * USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_WIDTH * \
     USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_HEIGHT * (10000000U / USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_MAX_INTERVAL))
#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_MAX_BIT_RATE                                              \
    (USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_DATA_BITS * 8U * USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_WIDTH * \
     USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_HEIGHT * (10000000U / USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_MIN_INTERVAL))

#define USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_MAX_FRAME_SIZE                                       \
    (USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_DATA_BITS * USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_WIDTH * \
     USB_VIDEO_CAMERA_UNCOMPRESSED_FRAME_HEIGHT)

/* terminal unit ID */
#define USB_VIDEO_CAMERA_VC_INPUT_TERMINAL_ID (1U)
#define USB_VIDEO_CAMERA_VC_INPUT_TERMINAL_TYPE (USB_DEVICE_VIDEO_ITT_CAMERA)
#define USB_VIDEO_CAMERA_VC_OUTPUT_TERMINAL_ID (2U)
#define USB_VIDEO_CAMERA_VC_PROCESSING_UNIT_ID (3U)

/*******************************************************************************
 * API
 ******************************************************************************/

/* Configure the device according to the USB speed. */
extern usb_status_t USB_DeviceSetSpeed(usb_device_handle handle, uint8_t speed);

/* Get device descriptor request */
usb_status_t USB_DeviceGetDeviceDescriptor(usb_device_handle handle,
                                           usb_device_get_device_descriptor_struct_t *deviceDescriptor);

/* Get device configuration descriptor request */
usb_status_t USB_DeviceGetConfigurationDescriptor(
    usb_device_handle handle, usb_device_get_configuration_descriptor_struct_t *configurationDescriptor);

/* Get device string descriptor request */
usb_status_t USB_DeviceGetStringDescriptor(usb_device_handle handle,
                                           usb_device_get_string_descriptor_struct_t *stringDescriptor);

#endif /* __USB_DEVICE_DESCRIPTOR_H__ */
