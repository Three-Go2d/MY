#include "bsp_adc.h" 
#include "bsp_uart4.h" 
#include "adc.h" 
#include "tim.h" 
#include <string.h>

uart_sample_t uart_buf_half[ADC_DMA_SAMPLES];
uart_sample_t uart_buf_full[ADC_DMA_SAMPLES];
uint16_t adc_dma_buf[ADC_DMA_SAMPLES * ADC_CH_NUM * 2];
uint32_t sample_index = 1;

extern UART_HandleTypeDef huart4;

void BSP_ADC_Start(void)
{
    sample_index = 1; 
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buf, ADC_DMA_SAMPLES * ADC_CH_NUM); 
}

static void FillUARTBuffer(uint16_t *src, uart_sample_t *dst)
{
    for(uint16_t i = 0; i < ADC_DMA_SAMPLES; i++)
    {
        dst[i].head = FRAME_HEAD;
        dst[i].index = sample_index++;
        for(uint8_t ch = 0; ch < ADC_CH_NUM; ch++)
            dst[i].ch[ch] = src[i*ADC_CH_NUM + ch];
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    FillUARTBuffer(adc_dma_buf, uart_buf_half);
    BSP_UART4_SendSamples((uint8_t*)uart_buf_half, sizeof(uart_buf_half));
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    FillUARTBuffer(&adc_dma_buf[ADC_DMA_SAMPLES*ADC_CH_NUM], uart_buf_full);
    BSP_UART4_SendSamples((uint8_t*)uart_buf_full, sizeof(uart_buf_full));
}
