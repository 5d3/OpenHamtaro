

#ifndef _ZIGBEE_HA_H
#define _ZIGBEE_HA_H


// Use USART1 for ZB Moudule
#define ZB_IRQ_FUN  USART1_IRQHandler

#define MAX_ZB_BUFFER_LEN  256  //���λ�������󳤶�


void ZB_HA_Init(void);

#endif
