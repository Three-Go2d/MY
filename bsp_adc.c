#include "bsp_adc.h"
#include "bsp_uart.h"

// Assuming the UART queue function is declared in bsp_uart.h

void HAL_ADC_ConvHalfCpltCallback(void)
{
    // Get sample data and queue it
    uint8_t *data = get_adc_half_data(); // Define this function to retrieve the half data
    BSP_UART4_SendSamples(data, sizeof(data)); // Send through UART4 queue
}

void HAL_ADC_ConvCpltCallback(void)
{
    // Get sample data and queue it
    uint8_t *data = get_adc_complete_data(); // Define this function to retrieve the complete data
    BSP_UART4_SendSamples(data, sizeof(data)); // Send through UART4 queue
}