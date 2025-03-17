#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "pwr.h"
#include "usart.h"

int main(void)
{	 
    uint8_t t = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
//	uart_init(115200);
    LED_Init();
//    LCD_Init();
    PVD_Init(PWR_PVDLevel_2V9);
    
	POINT_COLOR = RED;//设置字体为红色 
    
    LCD_ShowString(30, 50, 200, 16, 16, "STM32");
    LCD_ShowString(30, 70, 200, 16, 16, "PVD TEST");
    LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
    /* 默认LCD显示电压正常 */
    POINT_COLOR = BLUE;//设置字体为红色 
    LCD_ShowString(30, 110, 200, 16, 16, "PVD Voltage OK! ");
	while(1)
	{
        if ((t % 20) == 0)
        {
            LED0 = !LED0;              /* 每200ms,翻转一次LED0 */
        }

        delay_ms(10);
        t++;
	}
 }

