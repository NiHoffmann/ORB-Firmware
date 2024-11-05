//*******************************************************************
/*!
\author Nils Hoffmann
\date   12.10.2024
*/

#include "Sys/stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

//all functionality inside the fault handler we have to protect the fault handler from user mem write operations
void Firmware_Exception_Handler(){
    //make all variables static so we dont need to allocate memory for them
    static int exception_visited = 0;
    //disabled interrupt (IRQ)
    //found here :3.11.2 CPS https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf
    __asm volatile ("cpsid i");

    //i dont think this is even needed though running into a fault after a fult should stall the microcontroller might be wrong tho
    if(exception_visited) {
        //if the exception is triggered twice we know access to motors/leds caused problems -so we dont try to do it again :D
        __asm volatile ("b Infinite_Loop");
    }
    exception_visited = 1;

    //rest timer/gpio register this is a low risk/low level operation asuming the program memory has not been corupted
    //but since having a corrupted program memory can lead any kind of behaviour i think this is fine
    //it is always worth to try and stop servos/motors from running
    //a faulty program might break model servos
    RCC->AHB1ENR |=  (RCC_AHB1RSTR_GPIOARST|
                      RCC_AHB1RSTR_GPIOBRST|
                      RCC_AHB1RSTR_GPIOCRST|
                      RCC_AHB1RSTR_GPIODRST|
                      RCC_AHB1RSTR_GPIOERST|
                      RCC_AHB1RSTR_GPIOFRST|
                      RCC_AHB1RSTR_GPIOGRST|
                      RCC_AHB1RSTR_GPIOHRST|
                      RCC_AHB1RSTR_GPIOIRST);

    RCC->APB1RSTR |= (RCC_APB1RSTR_TIM2RST |
                      RCC_APB1RSTR_TIM3RST |
                      RCC_APB1RSTR_TIM4RST |
                      RCC_APB1RSTR_TIM5RST |
                      RCC_APB1RSTR_TIM6RST |
                      RCC_APB1RSTR_TIM7RST );

    RCC->APB2RSTR |= (RCC_APB2RSTR_TIM1RST |
                      RCC_APB2RSTR_TIM8RST |
                      RCC_APB2RSTR_TIM9RST |
                      RCC_APB2RSTR_TIM10RST|
                      RCC_APB2RSTR_TIM11RST);

    //turn on all leds to indicate the hard fault occuring
    RCC->AHB1RSTR |= ~RCC_AHB1RSTR_GPIOERST;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOE->MODER =  0x0 | (1 << 4) | (1 << 6) | (1 << 8);
    GPIOE->OTYPER = 0x0 |(1 << 2) | (1 << 3) | (1 << 4);
    GPIOE->ODR &= ~(1 << 2);
    GPIOE->ODR &= ~(1 << 3);
    GPIOE->ODR &= ~(1 << 4);

    //inf loop
    __asm volatile ("Infinite_Loop:\n"
                    "b Infinite_Loop");
}

#ifdef __cplusplus
}
#endif
