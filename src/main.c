#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f4xx_conf.h"
#include "utils.h"

// Private function prototypes
static void delayMs(uint32_t timeMs);
static void init();

int main(void) {
  init();

  while (true) {
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
    delayMs(500);
    GPIO_ResetBits(GPIOD, GPIO_Pin_12);
    delayMs(500);
  }

  return 0;
}

void init() {
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  // ---------- SysTick timer --------
  if (SysTick_Config(SystemCoreClock / 1000)) {
    // Capture error
	  while (true)
      ;
  }

  // ------ LED ------

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  // ------ UART ------

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART2, &USART_InitStructure);

  USART_Cmd(USART2, ENABLE);
}

volatile uint32_t _timeVar1;
volatile uint32_t _timeVar2;

// Called from systick handler
void timingHandler() {
  if (_timeVar1)
    _timeVar1--;

  _timeVar2++;
}

// Delay a number of systick cycles (1ms)
void delayMs(volatile uint32_t count) {
  _timeVar1 = count;

  while (_timeVar1)
    ;
}

// Dummy function to avoid compiler error
void _init() {

}

