#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "stm32f4xx_conf.h"
#include "utils.h"
#include "tm_stm32f4_fatfs.h"

// Private function prototypes
static void delayMs(uint32_t timeMs);
static void initLed();
static void initUart(uint32_t baudrate);
static void initDelayTimer();
static void print(const char* pText);
static void initSdCard();

int main(void) {
  initDelayTimer();
  initUart(115200);
  initLed();
  initSdCard();

  myprintf("Hello world!\n");

  while (true) {
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
    delayMs(500);
    GPIO_ResetBits(GPIOD, GPIO_Pin_12);
    delayMs(500);
  }

  return 0;
}

static void initDelayTimer() {
  if (SysTick_Config(SystemCoreClock / 1000)) {
	  while (true)
      ;
  }
}

static void initLed() {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static void initUart(uint32_t baudrate) {
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  USART_InitStruct.USART_BaudRate = baudrate;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &USART_InitStruct);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_Cmd(USART1, ENABLE);

  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    ;
}

static void initSdCard() {
  FATFS fatFs;

  myprintf("Mounting SD card... ");
  if (f_mount(&fatFs, "", 1) == FR_OK) {
    myprintf("success!\n");

    // Get total and free space on SD card:
    uint32_t free, total;
    TM_FATFS_DriveSize(&total, &free);

    myprintf("Total: %8u kB; %5u MB; %2u GB\n\r", total, total / 1024, total / 1048576);
    myprintf("Free:  %8u kB; %5u MB; %2u GB\n\r", free, free / 1024, free / 1048576);
  }
  else
    myprintf("Failed! No SD-Card?\n\r");

  f_mount(0, "", 1); // Unmount drive
}

void PrintCharUsr(char c) {
  // Wait until previous transmission is finished:
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    ;

  USART_SendData(USART1, (u8) c);
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

// TODO: Dummy function to avoid compiler error.
// Implementation only needed for c++ functions!?
// Find out how to avoid this dummy implementation.
void _init() {

}

