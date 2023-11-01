
#include <stdint.h>
#include "clock.h"
#define RCC_ADDR_BASE 0x40023800    // Start address of RCC pheriperal
#define FLASH_ADDR_BASE 0x40023C00  // Start address of FLASH pheriperal
/**
 * @brief setup RCC to generate a clock 100Mhz (max)
 * @param None
 * @retval None
 */
void clock_init()
{
    /*
    * PLL clock(source from HSE), M = /8, N = x200, P = /2
    */
   uint32_t* RCC_CR = (uint32_t*)(RCC_ADDR_BASE + 0x00);
   uint32_t* RCC_PLLCFGR = (uint32_t*)(RCC_ADDR_BASE + 0x04);
   uint32_t* RCC_CFGR = (uint32_t*)(RCC_ADDR_BASE + 0x08);

   *RCC_CR |= (1<<16);                      //enable HSE
   while(((*RCC_CR >> 17) & 1) == 0);       //wait HSE ready

    //set M div = /8
   *RCC_PLLCFGR &= ~(0b111111 << 0);
   *RCC_PLLCFGR |= (8 << 0);

   //set N mul = x200
   *RCC_PLLCFGR &= ~(0b111111111 << 6);
   *RCC_PLLCFGR |= (200 << 6);

   //set P div = 2
    *RCC_PLLCFGR &= ~(0b11 << 16);
    *RCC_PLLCFGR |= (0b00 << 16);

    //set PLL source is HSE
    *RCC_PLLCFGR |= (1<<22);

    *RCC_CR |= (1<<24);                      //enable PLL
    while(((*RCC_CR >> 25) & 1) == 0);       //wait PLL ready

    /*
        AHB <= 100Mhz
        APB1 <= 50Mhz
        APB2 <= 100Mhz
    */
   //set APB1 pre-scaler = /2
    *RCC_CFGR &= ~(0b111 << 10);        
    *RCC_CFGR |= (0b100 << 10);

    uint32_t* FLASH_ACR = (uint32_t*)(FLASH_ADDR_BASE + 0x00);
    *FLASH_ACR |= (3<<0);

    *RCC_CFGR |= 0b10 << 0;                  //select system clock in PLL
    
}

void clock_enable_AHB1(AHP1_peripheral_t peripheral)
{
    uint32_t* RCC_AHB1ENR = (uint32_t*)(RCC_ADDR_BASE + 0x30);
    *RCC_AHB1ENR |= (1<<peripheral);
}

void clock_enable_AHB2(AHP2_peripheral_t peripheral)
{
    uint32_t* RCC_AHB2ENR = (uint32_t*)(RCC_ADDR_BASE + 0x34);
    *RCC_AHB2ENR |= (1<<peripheral);
}

void clock_enable_APB1(ABP1_peripheral_t peripheral)
{
    uint32_t* RCC_APB1ENR = (uint32_t*)(RCC_ADDR_BASE + 0x40);
    *RCC_APB1ENR |= (1<<peripheral);
}

void clock_enable_APB2(ABP2_peripheral_t peripheral)
{
    uint32_t* RCC_APB2ENR = (uint32_t*)(RCC_ADDR_BASE + 0x44);
    *RCC_APB2ENR |= (1<<peripheral);
}
