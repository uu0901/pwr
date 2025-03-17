#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "pwr.h"
#include "usart.h"

int main(void)
{	 
    uint8_t t = 0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	uart_init(115200);
    LED_Init();
//    LCD_Init();
    PVD_Init(PWR_PVDLevel_2V9);
    
	POINT_COLOR = RED;//��������Ϊ��ɫ 
    
    LCD_ShowString(30, 50, 200, 16, 16, "STM32");
    LCD_ShowString(30, 70, 200, 16, 16, "PVD TEST");
    LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
    /* Ĭ��LCD��ʾ��ѹ���� */
    POINT_COLOR = BLUE;//��������Ϊ��ɫ 
    LCD_ShowString(30, 110, 200, 16, 16, "PVD Voltage OK! ");
	while(1)
	{
        if ((t % 20) == 0)
        {
            LED0 = !LED0;              /* ÿ200ms,��תһ��LED0 */
        }

        delay_ms(10);
        t++;
	}
 }

