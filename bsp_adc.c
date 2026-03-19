#include "bsp_adc.h"
#include "bsp_uart4.h"
#include "adc.h"
#include "tim.h"
#include <string.h>

uart_sample_t uart_buf_half[ADC_DMA_SAMPLES]; // 半缓冲独立存储
uart_sample_t uart_buf_full[ADC_DMA_SAMPLES]; //满缓冲独立存储
uint16_t adc_dma_buf[ADC_DMA_SAMPLES * ADC_CH_NUM * 2]; //  DMA 缓冲
uint32_t sample_index = 1; // 从 1 开始

extern UART_HandleTypeDef huart4;

// ------------------ 初始化 ADC DMA ------------------
void BSP_ADC_Start(void)
{
    sample_index = 1; 
    HAL_TIM_Base_Start(&htim6);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buf, ADC_DMA_SAMPLES * ADC_CH_NUM); 
}

// ------------------ 转换函数 ------------------
static void FillUARTBuffer(uint16_t *src, uart_sample_t *dst)
{
    for(uint16_t i = 0; i < ADC_DMA_SAMPLES; i++)
    {
        dst[i].head = FRAME_HEAD;        //  帧头
        dst[i].index = sample_index++;   // 序号累加
        for(uint8_t ch = 0; ch < ADC_CH_NUM; ch++)
            dst[i].ch[ch] = src[i*ADC_CH_NUM + ch]; //  通道赋值
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    FillUARTBuffer(adc_dma_buf, uart_buf_half);
    while(uart_busy); //  阻塞等待 DMA 完成
    uart_busy = 1;
    HAL_UART_Transmit_DMA(&huart4, (uint8_t*)uart_buf_half, sizeof(uart_buf_half));
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    FillUARTBuffer(&adc_dma_buf[ADC_DMA_SAMPLES*ADC_CH_NUM], uart_buf_full);
    while(uart_busy);
    uart_busy = 1;
    HAL_UART_Transmit_DMA(&huart4, (uint8_t*)uart_buf_full, sizeof(uart_buf_full));
}

