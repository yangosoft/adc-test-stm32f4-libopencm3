#include <cstdint>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>

#define LED_DISCO_GREEN_PORT GPIOG
#define LED_DISCO_GREEN_PIN GPIO13



#define USART_CONSOLE USART1


extern "C"{
/**
 * Use USART_CONSOLE as a console.
 * This is a syscall for newlib
 * @param file
 * @param ptr
 * @param len
 * @return
 */
    int _write(int /*file*/, char *ptr, int len)
    {
        int i;
        for (i = 0; i < len; i++) {

            usart_send_blocking(USART_CONSOLE, ptr[i]);
        }
        return i;
    }
}

/* milliseconds since boot */
volatile uint32_t system_millis;

/* Called when systick fires */
void sys_tick_handler(void)
{
    system_millis++;
}

/* simple sleep for delay milliseconds */
void msleep(uint32_t delay)
{
    uint32_t wake = system_millis + delay;
    while (wake > system_millis);
}

/* Getter function for the current time */
uint32_t mtime(void)
{
    return system_millis;
}

void adc_setup(void)
{
   gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO3);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO5);

	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);

	adc_power_on(ADC1);

}

uint16_t read_adc_naiive(uint8_t channel)
{
    uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}


/* Set STM32 to 24 MHz. */
void clock_setup(void)
{

    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	/* Enable GPIOD clock for LED & USARTs. */
	rcc_periph_clock_enable(RCC_GPIOG);
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART2 and dac */
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_DAC);

	/* And ADC*/
	rcc_periph_clock_enable(RCC_ADC1);
 systick_interrupt_enable();
}

void gpio_setup(void)
{


   
	/* Setup GPIO pins for USART1 transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	/* Setup USART1 TX pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9);

    usart_set_baudrate(USART1, 38400);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    /* Finally enable the USART. */
    usart_enable(USART1);

}




int main( void )
{
    clock_setup();
    gpio_setup();
    adc_setup();

    
gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,	LED_DISCO_GREEN_PIN);

    while(1)
    {
	

	/* green led for ticking */
	
        uint16_t adcValue1 = read_adc_naiive(3);
        uint16_t adcValue2 = read_adc_naiive(5);
        
        printf("PA3: %d PA5: %d\n",adcValue1, adcValue2);

        
	

	msleep(600);
    }

    return 0;
}
