#include <stdio.h>
#include <string.h>
#include <stm32f10x.h>
#include "gizwits_product.h"
#include "Hal_key/Hal_key.h"
#include "Hal_Usart/hal_uart.h"
#include "Hal_led/Hal_led.h"
#include "Hal_motor/Hal_motor.h"
#include "Hal_rgb_led/Hal_rgb_led.h"
#include "Hal_temp_hum/Hal_temp_hum.h"
#include "Hal_infrared/Hal_infrared.h"
#include "zigbee_ha.h"
#include "header.h"

extern volatile gizwitsReport_t reportData;

//custom �´����ݷ�����������
int8_t eventProcess(eventInfo_t *info, uint8_t *data, uint32_t len)
{
    uint8_t i = 0;
    int16_t motorValue = 0;
    gizwitsIssued_t *issuedData = (gizwitsIssued_t *)data;
    moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;
    ZB_LIGHT_CMD_T zbLightCmd;

    if((NULL == info) || (NULL == issuedData))
    {
        return -1;
    }

    for(i=0; i<info->num; i++)
    {
        switch(info->event[i])
        {
            case SetLED_OnOff:
                if(LED_OnOn == issuedData->attrVals.LED_OnOff)
                {
                    reportData.devStatus.LED_OnOff = LED_OnOn;
                    ledRgbControl(254,0,0);       
                }
                else
                {
                    reportData.devStatus.LED_OnOff = LED_OnOff;
                    ledRgbControl(0,0,0);
                }
                
                zbLightCmd.clusterId = ZCL_Cluster_ID_OnOff;
                zbLightCmd.attr.cmd_onoff.onoff = reportData.devStatus.LED_OnOff;
                ZB_HA_LightControl(&zbLightCmd);
                #if EN_DEBUG > 0
                GIZWITS_LOG("Evt: SetLED_OnOff %d\r\n",reportData.devStatus.LED_OnOff);
                #endif
                break;
            case SetLED_Color:
                zbLightCmd.clusterId = ZCL_Cluster_ID_ColorControl;
                switch(issuedData->attrVals.LED_Color)
                {
                    case LED_Costom:
                        reportData.devStatus.LED_Color = LED_Costom;
                        ledRgbControl(reportData.devStatus.LED_R, reportData.devStatus.LED_G, reportData.devStatus.LED_B);
                        zbLightCmd.attr.cmd_colorctrl.red = reportData.devStatus.LED_R;
                        zbLightCmd.attr.cmd_colorctrl.green = reportData.devStatus.LED_G;
                        zbLightCmd.attr.cmd_colorctrl.blue = reportData.devStatus.LED_B;
                        break;
                    case LED_Yellow:
                        reportData.devStatus.LED_Color = LED_Yellow;
                        ledRgbControl(254, 254, 0);    
                        zbLightCmd.attr.cmd_colorctrl.red = 254;
                        zbLightCmd.attr.cmd_colorctrl.green = 254;
                        zbLightCmd.attr.cmd_colorctrl.blue = 0;
                        break;
                    case LED_Purple:
                        reportData.devStatus.LED_Color = LED_Purple;
                        ledRgbControl(254, 0, 70);
                        zbLightCmd.attr.cmd_colorctrl.red = 254;
                        zbLightCmd.attr.cmd_colorctrl.green = 0;
                        zbLightCmd.attr.cmd_colorctrl.blue = 70;
                        break;
                    case LED_Pink:
                        reportData.devStatus.LED_Color = LED_Pink;
                        ledRgbControl(238 ,30 ,30);   
                        zbLightCmd.attr.cmd_colorctrl.red = 238;
                        zbLightCmd.attr.cmd_colorctrl.green = 30;
                        zbLightCmd.attr.cmd_colorctrl.blue = 30;
                        break;
                    default:
                        break;
                }
                ZB_HA_LightControl(&zbLightCmd);
                #if EN_DEBUG > 0
                GIZWITS_LOG("Evt: SetLED_Color %d\r\n",issuedData->attrVals.LED_Color);
                #endif
                break;
            case SetLED_R:
                reportData.devStatus.LED_R = issuedData->attrVals.LED_R;
                issuedData->attrVals.LED_R = X2Y(LED_B_RATIO,LED_B_ADDITION,issuedData->attrVals.LED_R);
                ledRgbControl(issuedData->attrVals.LED_R,X2Y(LED_B_RATIO,LED_B_ADDITION,\
            reportData.devStatus.LED_G),X2Y(LED_B_RATIO,  LED_B_ADDITION, reportData.devStatus.LED_G));

                zbLightCmd.clusterId = ZCL_Cluster_ID_LevelControl;
                zbLightCmd.attr.cmd_levelctrl.level = reportData.devStatus.LED_R;
                ZB_HA_LightControl(&zbLightCmd);
                #if EN_DEBUG > 0
                GIZWITS_LOG("Evt:color %d SetLED_R %d\r\n",issuedData->attrVals.LED_Color,issuedData->attrVals.LED_R);
                #endif
                break;
            case SetLED_G:
                reportData.devStatus.LED_G = issuedData->attrVals.LED_G;
                issuedData->attrVals.LED_G = X2Y(LED_B_RATIO,LED_B_ADDITION,issuedData->attrVals.LED_G);
                ledRgbControl(X2Y(LED_B_RATIO,LED_B_ADDITION,reportData.devStatus.LED_G),\
            issuedData->attrVals.LED_G,X2Y(LED_B_RATIO, LED_B_ADDITION,reportData.devStatus.LED_B));

                zbLightCmd.clusterId = ZCL_Cluster_ID_LevelControl;
                zbLightCmd.attr.cmd_levelctrl.level = reportData.devStatus.LED_G;
                ZB_HA_LightControl(&zbLightCmd);
                #if EN_DEBUG > 0
                GIZWITS_LOG("Evt: color %d SetLED_G %d\r\n", issuedData->attrVals.LED_Color, issuedData->attrVals.LED_G);
                #endif
                break;
            case SetLED_B:
                reportData.devStatus.LED_B = issuedData->attrVals.LED_B;
                issuedData->attrVals.LED_B = X2Y(LED_B_RATIO,LED_B_ADDITION,issuedData->attrVals.LED_B);
                ledRgbControl(X2Y(LED_B_RATIO,LED_B_ADDITION,reportData.devStatus.LED_G),\
            X2Y(LED_B_RATIO,LED_B_ADDITION,reportData.devStatus.LED_G),issuedData->attrVals.LED_B );

                zbLightCmd.clusterId = ZCL_Cluster_ID_LevelControl;
                zbLightCmd.attr.cmd_levelctrl.level = reportData.devStatus.LED_B;
                ZB_HA_LightControl(&zbLightCmd);
                #if EN_DEBUG > 0
                GIZWITS_LOG("Evt: color %dSetLED_B %d\r\n",issuedData->attrVals.LED_Color,issuedData->attrVals.LED_B);
                #endif
                break;
            case SetMotor:
                reportData.devStatus.Motor_Speed = issuedData->attrVals.Motor_Speed;  
                motorValue = X2Y(MOTOR_SPEED_RATIO,MOTOR_SPEED_ADDITION,protocolExchangeBytes(issuedData->attrVals.Motor_Speed));
                motorStatus(motorValue);
                #if EN_DEBUG > 0
                GIZWITS_LOG("Evt: SetMotor %d\r\n", motorValue);
                #endif
                break;
            
            case WIFI_SOFTAP:
                break;
            case WIFI_AIRLINK:
                break;
            case WIFI_STATION:
                break;
            case WIFI_CON_ROUTER:
                ledRgbControl(0, 0, 0);
                break;
            case WIFI_DISCON_ROUTER:
                break;
            case WIFI_CON_M2M:
                break;
            case WIFI_DISCON_M2M:
                break;
            case WIFI_RSSI:
                #if EN_DEBUG > 0
                printf("RSSI %d\r\n", wifiData->rssi);
                #endif
                break;
            default:
                break;
        }
    }
    
    return 0;
}

/**
  * TIM3_IRQHandler
  */
extern keyTypedef_t singleKey[2];
extern keysTypedef_t keys;
void TIMER_IRQ_FUN(void)
{
    if (TIM_GetITStatus(TIMER, TIM_IT_Update) != RESET)  
    {
        TIM_ClearITPendingBit(TIMER, TIM_IT_Update  );
        gizwitsTimerMs();
    }
}

/*******************************************************************************
* Function Name  : timerInit
* Description    : timer initialization function
* Input          : arr ��װ��ֵ   psc Ԥ��Ƶ
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void timerInit(void)
{
    u16 arr = 7199;
    u16 psc = 9;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(TIMER_RCC, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ����ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʵ�Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIMER, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ0

    TIM_ITConfig(TIMER, TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIMER_IRQ;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
    TIM_Cmd(TIMER, ENABLE);  //ʹ��TIMx
}

/**
  * USART2_IRQHandler
  */
void UART_IRQ_FUN(void)
{
    uint8_t value = 0;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        value = USART_ReceiveData(USART2);

        gizwitsPutData(&value, 1);
    }
}


//custom ����д�������������ݵ�WiFiģ��
int8_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    
    if(NULL == buf)
    {
        return -1;
    }

#if EN_DEBUG > 0
    GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizwitsGetTimerCount(), len);
#endif
    
    for(i=0; i<len; i++)
    {
        USART_SendData(UART, buf[i]);
        //Loop until the end of transmission
        while (USART_GetFlagStatus(UART, USART_FLAG_TXE) == RESET);
        
#if EN_DEBUG > 0
        GIZWITS_LOG("%02x ", buf[i]);
#endif
    }
    
#if EN_DEBUG > 0
    GIZWITS_LOG("\r\n");
#endif
    
    return len;
}

void uartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    UART_GPIO_Cmd(UART_GPIO_CLK, ENABLE);
    UART_CLK_Cmd(UART_CLK, ENABLE);
    UART_AFIO_Cmd(UART_AFIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = UART_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UART_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = UART_RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(UART_GPIO_PORT, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = UART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART, &USART_InitStructure);

    USART_ITConfig(UART,USART_IT_RXNE,ENABLE);
    USART_Cmd(UART, ENABLE);
    USART_ClearFlag(UART, USART_FLAG_TC); /*��շ�����ɱ�־,Transmission Complete flag */

    /*ʹ�ܴ����ж�,���������ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
