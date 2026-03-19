#ifndef BSP_UART4_H
#define BSP_UART4_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define UART_TX_QUEUE_SIZE 4   // 队列长度，可根据情况调整
#define UART_TX_BUF_SIZE 1024   // 单块大小

extern volatile bool uart_busy;

/* 初始化 UART4 */
void BSP_UART4_Init(void);

/* DMA 发送函数（队列发送） */
void BSP_UART4_SendSamples(uint8_t *data, uint16_t len);



#endif
