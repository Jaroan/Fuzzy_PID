#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

//PA0 is the Tx Pin
//PA1 is the Rx Pin
//GND of both the microcontrollers should be shorted

int ticks = 0;
int PWM_Fuzzy = 200;

void Initialise_LED()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_Init_Struct;
	GPIO_Init_Struct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Init_Struct.GPIO_OType=GPIO_OType_PP;
	GPIO_Init_Struct.GPIO_Pin=GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init_Struct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_Init_Struct);
	GPIO_SetBits(GPIOD,GPIO_Pin_12);
}

void Initialise_UART()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0,   GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1,   GPIO_AF_UART4);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate=9600;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b ; //Change this to increase the size of data send
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx ;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(UART4,&USART_InitStruct);

	USART_Cmd(UART4,ENABLE);
	USART_ITConfig(UART4,USART_IT_RXNE ,ENABLE);
}

void Initialise_NVIC()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=UART4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
}

void UART4_IRQHandler()
{
	GPIO_SetBits(GPIOD,GPIO_Pin_13); //Pin_13_LED set during Recieving

	//Recieving Data as ticks
	ticks=USART_ReceiveData(UART4);

	GPIO_ResetBits(GPIOD,GPIO_Pin_13);

	//Call the Fuzzy Code Function Here and return the value to PWM_Fuzzy

	GPIO_SetBits(GPIOD,GPIO_Pin_12); //Pin_12_LED set during Transmitting

	//Transmitting Data as Fuzzy_PWM
	while(USART_GetFlagStatus( UART4,USART_FLAG_TXE)!=SET){}
		USART_SendData(UART4, PWM_Fuzzy);

	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
}

int main(void)
{
	Initialise_LED();
	Initialise_UART();
	Initialise_NVIC();

	int PWM = 200;
	//Send a PWM for the First Time

	while(USART_GetFlagStatus( UART4,USART_FLAG_TXE)!=SET){}
		USART_SendData(UART4, PWM);
	//Subsequent Transmission will be initialise when any data is recieved in the IRQ_Handler();

    while(1)
    {
    }
}
