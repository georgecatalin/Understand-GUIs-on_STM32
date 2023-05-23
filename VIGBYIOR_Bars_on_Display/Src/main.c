/**
 ******************************************************************************
 * @file           : main.c
 * @author         : George Calin
 * @date          :  05.05.2023
 ******************************************************************************
 * @attention

 ******************************************************************************
 */

#include <registry_utilities.h>
#include <stdint.h>
#include "stm32f429xx.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


//TODO Set the main PLL
/* **************************************************************************************************************************************************************************************************
 * Information to be taken from the RM0090 Reference Manual: section Reset and clock control for STM32F42xxx and STM32F43xxx (RCC)  >> Figure 16. Clock tree
 * 	  section RCC PLL configuration register (RCC_PLLCFGR)
 *
 * 	We need to set /M divider at the entrance of the PLL to 8 hence we need to handle Bits 5:0 PLLM: Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
 * 				CAUTION :The software has to set these bits correctly to ensure that the VCO input frequency ranges from 1 to 2 MHz. It is recommended to select a frequency of 2 MHz to limit PLL jitter.
 * 				000000: PLLM = 0, wrong configuration
				000001: PLLM = 1, wrong configuration
				000010: PLLM = 2
				000011: PLLM = 3
				000100: PLLM = 4
				...
				111110: PLLM = 62
				111111: PLLM = 63
				...

 * 	We need to set the xN multiplier inside the PLL to 180 hence we need to handle Bits 14:6 PLLN: Main PLL (PLL) multiplication factor for VCO
 * 				Set and cleared by software to control the multiplication factor of the VCO. These bits can be written only when PLL is disabled.
 * 				CAUTION: The software has to set these bits correctly to ensure that the VCO output frequency is between 100 and 432 MHz.
 * 				000000000: PLLN = 0, wrong configuration
				000000001: PLLN = 1, wrong configuration
				...
				000110010: PLLN = 50
				...
				001100011: PLLN = 99
				001100100: PLLN = 100
				...
				110110000: PLLN = 432
				110110001: PLLN = 433, wrong configuration
				...
				111111111: PLLN = 511, wrong configuration
				Note: Multiplication factors ranging from 50 and 99 are possible for VCO input frequency higher than 1 MHz. However care must be taken that the minimum VCO output frequency respects the value specified above.
 *
 *
 * We need to set the /P divider at the exit of the PLL to 00 hence we need to handle Bits 17:16 PLLP: Main PLL (PLL) division factor for main system clock
 * 				Set and cleared by software to control the frequency of the general PLL output clock. These bits can be written only if PLL is disabled.
 * 				Caution: The software has to set these bits correctly not to exceed 180 MHz on this domain. PLL output clock frequency = VCO frequency / PLLP with PLLP = 2, 4, 6, or 8
 * 				00: PLLP = 2
				01: PLLP = 4
				10: PLLP = 6
				11: PLLP = 8
 *
 * ************************************************************************************************************************************************************************************************** */
void Setup_MainPLL(void);

//TODO Set the PLLSAI
/* ****************************************************************************************************************************************************************************************************
 * Information to be taken from the RM0090 Reference Manual: section Reset and clock control for STM32F42xxx and STM32F43xxx (RCC)  >> Figure 16. Clock tree
 *
 * 	We need to set the xN multiplier inside PLLSAI : information to be taken from section 6.3.24 RCC PLL configuration register (RCC_PLLSAICFGR)
 * 		Bits 14:6 PLLSAIN: PLLSAI multiplication factor for VCO
 * 		These bits are set and cleared by software to control the multiplication factor of the VCO. These bits can be written only when PLLSAI is disabled.
 * 		Caution: The software has to set these bits correctly to ensure that the VCO output frequency is between 100 and 432 MHz.
 * 		000000000: PLLISAIN = 0, wrong configuration
		000000001: PLLISAIN = 1, wrong configuration
		...
		000110010: PLLISAIN = 50
		...
		001100011: PLLISAIN = 99
		001100100: PLLISAIN = 100
		001100101: PLLISAIN = 101
		001100110: PLLISAIN = 102
		...
		110110000: PLLISAIN = 432
		110110001: PLLISAIN = 433, wrong configuration
		...
		111111111: PLLISAIN = 511, wrong configuration
 *
 * 	We need to set the divider /R inside PLLSAI : information to be taken from section 6.3.24 RCC PLL configuration register (RCC_PLLSAICFGR)
 * 		Bits 30:28 PLLSAIR: PLLSAI division factor for LCD clock
 * 		Set and reset by software to control the LCD clock frequency.
 * 		000: PLLSAIR = 0, wrong configuration
		001: PLLSAIR = 1, wrong configuration
		010: PLLSAIR = 2
		...
		111: PLLSAIR = 7
 *
 *
 * 	We need to set the divider DIV which resides outside the PLLSAI, before the LCD-TFT_CLK : information to be taken from section 6.3.25 RCC Dedicated Clock Configuration Register (RCC_DCKCFGR)
 * 		Bits 17:16 PLLSAIDIVR: division factor for LCD_CLK
 * 		These bits are set and cleared by software to control the frequency of LCD_CLK. They should be written only if PLLSAI is disabled.
 * 		00: PLLSAIDIVR = /2
		01: PLLSAIDIVR = /4
		10: PLLSAIDIVR = /8
		11: PLLSAIDIVR = /16
 *
 *
 ***************************************************************************************************************************************************************************************************** */
void Setup_PLLSAI(void);

//TODO Setup AHB and APB clocks
/* ****************************************************************************************************************************************************************************************************
 *
 * Information extracted from  RM0090 Reference Manual: section Reset and clock control for STM32F42xxx and STM32F43xxx (RCC)  >> Figure 16. Clock tree
 * 		The maximum frequency of the AHB domain is 180 MHz.
 * 		The maximum allowed frequency of the high-speed APB2 domain is 90 MHz.
 * 		The	maximum allowed frequency of the low-speed APB1 domain is 45 MHz
 *
 * The dividers for the AHB1 and APB1/APB2 can be seen in 6.3.3 RCC clock configuration register (RCC_CFGR)
 *
 * 		Bits 7:4 HPRE: AHB prescaler
			Set and cleared by software to control AHB clock division factor.
			Caution: The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after HPRE write.
			Caution: The AHB clock frequency must be at least 25 MHz when the Ethernet is used.
				0xxx: system clock not divided
				1000: system clock divided by 2
				1001: system clock divided by 4
				1010: system clock divided by 8
				1011: system clock divided by 16
				1100: system clock divided by 64
				1101: system clock divided by 128
				1110: system clock divided by 256
				1111: system clock divided by 512

		Bits 12:10 PPRE1: APB Low speed prescaler (APB1)
			Set and cleared by software to control APB low-speed clock division factor.
			Caution: The software has to set these bits correctly not to exceed 45 MHz on this domain.
			The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after PPRE1 write.
				0xx: AHB clock not divided
				100: AHB clock divided by 2
				101: AHB clock divided by 4
				110: AHB clock divided by 8
				111: AHB clock divided by 16

		Bits 15:13 PPRE2: APB high-speed prescaler (APB2)
			Set and cleared by software to control APB high-speed clock division factor.
			Caution: The software has to set these bits correctly not to exceed 90 MHz on this domain.
			The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after PPRE2 write.
				0xx: AHB clock not divided
				100: AHB clock divided by 2
				101: AHB clock divided by 4
				110: AHB clock divided by 8
				111: AHB clock divided by 16
 *
 * ****************************************************************************************************************************************************************************************************/
void Setup_dividers_AHB1_APB1_APB2(void);
void Turn_on_PLL_wait_PLLCLK_ready(void);
void Switch_PLLCLK_as_SYSCLK(void);

int main(void)
{
	Setup_MainPLL();

    /* Loop forever */
	for(;;);
}

void Setup_MainPLL(void)
{
	/* Setting the  PLLM = 8 = 1000 */
	RCC->PLLCFGR &= ~(1UL<<0); // '0'
	RCC->PLLCFGR &=~ (1UL<<1); // '0'
	RCC->PLLCFGR &=~ (1UL<<2); // '0'
	RCC ->PLLCFGR |=(1UL<<3); // '1'
	RCC->PLLCFGR &=~ (1UL<<4); //'0'
	RCC->PLLCFGR &=~ (1UL<<5); // '0'

	/* Setting the PLLN to 180 */
	RCC->PLLCFGR &= ~(1UL<<6); // '0'
	RCC->PLLCFGR &= ~(1UL<<7); // '0'
	RCC->PLLCFGR |= (1UL<<8); // '1'
	RCC->PLLCFGR &= ~(1UL<<9); // '0'
	RCC->PLLCFGR |= (1UL<<10); // '1'
	RCC->PLLCFGR |= (1UL<<11); // '1'
	RCC->PLLCFGR &= ~(1UL<<12); // '0'
	RCC->PLLCFGR |= (1UL<<13); // '1'
	RCC->PLLCFGR &=~ (1UL<<14); // '0'

	/* Setting the PLLP to 2 */
	RCC->PLLCFGR &=~ (1UL<<16); //'0'
	RCC->PLLCFGR &=~ (1UL<<17); //'0'
}

void Setup_PLLSAI(void)
{
	/* Setting inside the PLLSAI module of the xN multiplier */
	// from outside ,after the /M we are getting 16/8=2Mhz at the entrance of the PLLSAI module clock. We can set the xN to 50 --> 000110010: PLLISAIN = 50
	RESET_BITS_REGISTRY(RCC->PLLSAICFGR,6,9);
	RCC->PLLSAICFGR |= (1UL<<7);
	SET_BITS_REGISTRY(RCC->PLLSAICFGR,10,2);

	/* Setting inside the PLLSAI module of the /R divider */
	// After setting the xN multiplier we have 100Mhz at the exit of the VCO, hence we will set /R as 010: PLLSAIR = 2 thus we will be obtaining 50Mhz at the exit of the PLLSAI module
	RCC->PLLSAICFGR &= ~(1UL<<28); // '0'
	RCC->PLLSAICFGR |= (1UL<<29); // '1'
	RCC->PLLSAICFGR &= ~(1UL<<30); // '0'

	/* Setting the DIV divider outside the PLLSAI module, just before the LCD-TFT clock */
	// We should be obtaining between 6-8Mhz at the LCD-TFT clock, and we have at the entrance of the divider 50Mhz. If we set this divider to 10: PLLSAIDIVR = /8, then we will have at the clock 50/8=6.25Mhz
	RCC->DCKCFGR &= ~(1UL<<16); //'0'
	RCC->DCKCFGR |= (1UL<<17); // '1'

}


void Setup_dividers_AHB1_APB1_APB2(void)
{
	/* Setting the prescaler/divider for AHB1 */
	// Since we have SYSCLK=180Mhz and we can have a clock of maximum speed of 180Mhz for AHB1, we shall keep the maximum speed on AHB1 hence 180Mhz => prescaler =1 0xxx: system clock not divided
	RCC->CFGR &= ~(1UL<<4); // '0'
	RCC->CFGR &= ~(1UL<<5); // '0'
	RCC->CFGR &= ~(1UL<<6); // '0'
	RCC->CFGR &= ~(1UL<<7); // '0'

	/* Setting the prescaler/divider for APB1 */
	// Since we can have a maximum speed of 45Mhz on APB1, then we are going to use a prescaler of 4 to divide from 180Mhz => 180/4=45Mhz speed of APB1 max
	// 101: AHB clock divided by 4
	RCC->CFGR |= (1UL<<10); //'1'
	RCC->CFGR &= ~(1UL<<11); //'0'
	RCC->CFGR |= (1UL<<12); //'1'

	/* Setting the prescaler/divider for APB2 */
	// Since we can have a maximum speed of 90Mhz on APB2, then we are going to use a prescaler of 2 to divide from 180Mhz => 180/2=90Mhz speed of APB2 max
	// 100: AHB clock divided by 2
	RCC->CFGR |= (1UL<<13); //'1'
	RCC->CFGR &= ~(1UL<<14); //'0'
	RCC->CFGR &= ~(1UL<<15); // '0'

}

//Using 6.3.1 RCC clock control register (RCC_CR)
void Turn_on_PLL_wait_PLLCLK_ready(void)
{
	/* Turn on PLL */
	RCC->CR |= (1UL<<24); // '1' PLLON

	/* Wait until PLLCLK is ready */
	while(!RCC->CR & (1UL<<25));
}

void Switch_PLLCLK_as_SYSCLK(void)
{

}
