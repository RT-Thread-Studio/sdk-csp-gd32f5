/*
 * Copyright (c) 2006-2026, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-10-22     WangShun     compatible with RT-Studio
 */

#ifndef __CAN_CONFIG_H__
#define __CAN_CONFIG_H__

#include <rtthread.h>
#include <board.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BSP_USING_CAN0)
#ifndef BSP_CAN0_RX_PIN
#define BSP_CAN0_RX_PIN "PD0"
#define BSP_CAN0_TX_PIN "PD1"
#define BSP_CAN0_AFIO   "AF9"
#endif /* BSP_CAN0_RX_PIN */
#ifndef CAN0_CONFIG
#define CAN0_CONFIG                     \
    {                                   \
        .name = "can0",                 \
        .tx_pin_name = BSP_CAN0_TX_PIN, \
        .rx_pin_name = BSP_CAN0_RX_PIN, \
        .alternate = BSP_CAN0_AFIO,     \
    }
#endif /* CAN0_CONFIG */
#endif /* BSP_USING_CAN0 */

#if defined(BSP_USING_CAN1)
#ifndef BSP_CAN1_RX_PIN
#define BSP_CAN1_RX_PIN "PB5"
#define BSP_CAN1_TX_PIN "PB6"
#define BSP_CAN1_AFIO   "AF9"
#endif /* BSP_CAN0_RX_PIN */
#ifndef CAN1_CONFIG
#define CAN1_CONFIG                     \
    {                                   \
        .name = "can1",                 \
        .tx_pin_name = BSP_CAN1_TX_PIN, \
        .rx_pin_name = BSP_CAN1_RX_PIN, \
        .alternate = BSP_CAN1_AFIO,     \
    }
#endif /* CAN1_CONFIG */
#endif /* BSP_USING_CAN1 */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_CONFIG_H__ */
