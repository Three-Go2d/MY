#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "main.h"

#define ADC_CH_NUM 4
#define ADC_DMA_SAMPLES 64
#define FRAME_HEAD 0x55AA55AA  //

typedef struct {
    uint32_t head;       // 帧头
    uint32_t index;      // 样本序号
    uint16_t ch[ADC_CH_NUM]; // 四通道
} uart_sample_t;

// DMA 缓冲区
extern uart_sample_t uart_buf_half[ADC_DMA_SAMPLES];
extern uart_sample_t uart_buf_full[ADC_DMA_SAMPLES];
extern uint16_t adc_dma_buf[ADC_DMA_SAMPLES * ADC_CH_NUM * 2];
extern uint32_t sample_index;

// 函数声明
void BSP_ADC_Start(void);

#endif

