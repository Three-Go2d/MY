#include "bsp_uart4.h"
#include "usart.h"
#include <string.h>
#include <stdbool.h>

extern UART_HandleTypeDef huart4;
volatile bool uart_busy = false;

static uint8_t tx_queue[UART_TX_QUEUE_SIZE][UART_TX_BUF_SIZE];
static uint16_t tx_len[UART_TX_QUEUE_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

// 初始化
void BSP_UART4_Init(void)
{
    uart_busy = false;
    tx_head = tx_tail = 0;
}

// 队列发送
void BSP_UART4_SendSamples(uint8_t *data, uint16_t len)
{
    if(len > UART_TX_BUF_SIZE) return;

    uint8_t next = (tx_head + 1) % UART_TX_QUEUE_SIZE;

    if(next == tx_tail) // 队列满就丢包
    {
        return;
    }

    memcpy(tx_queue[tx_head], data, len);
    tx_len[tx_head] = len;
    tx_head = next;

    // 关键修复：检查是否没有正在发送的数据
    if(!uart_busy)
    {
        uart_busy = true;
        // 发送第一个数据
        HAL_UART_Transmit_DMA(&huart4, tx_queue[tx_tail], tx_len[tx_tail]);
    }
}

// DMA 完成回调
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance != UART4) return;

    // 将指针移到下一个要发送的数据
    tx_tail = (tx_tail + 1) % UART_TX_QUEUE_SIZE;

    // 如果还有数据要发送
    if(tx_tail != tx_head)
    {
        // 继续发送下一个数据，uart_busy 保持 true
        HAL_UART_Transmit_DMA(&huart4, tx_queue[tx_tail], tx_len[tx_tail]);
    }
    else
    {
        // 队列为空，标记发送完成
        uart_busy = false;
    }
}