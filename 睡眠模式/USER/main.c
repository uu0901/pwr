#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "pwr.h"
#include "usart.h"
#include "key.h"
int main(void)
{	 
    uint8_t t = 0;
    uint8_t key = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);
    LED_Init();
    LCD_Init();
    
//    PVD_Init(PWR_PVDLevel_2V9);
    KEY_Init();
    
    pwr_wkup_key_init();
    
	POINT_COLOR = RED;//设置字体为红色 
    LCD_ShowString(30, 50, 200, 16, 16,  "STM32");
    LCD_ShowString(30, 70, 200, 16, 16,  "SLEEP TEST");
    LCD_ShowString(30, 90, 200, 16, 16,  "ATOM@ALIENTEK");
    LCD_ShowString(30, 110, 200, 16, 16, "KEY0:Enter SLEEP MODE");
    LCD_ShowString(30, 130, 200, 16, 16, "KEY_UP:Exit SLEEP MODE");
    while(1)
	{
        key = KEY_Scan(0);

        if (key == KEY0_PRES)
        {
            LED1 = 0;                    /* 点亮绿灯,提示进入睡眠模式 */

            pwr_enter_sleep();          /* 进入睡眠模式 */

            LED1 = 1;                    /* 关闭绿灯,提示退出睡眠模式 */
        }
        if ((t % 20) == 0)
        {
            LED0 = !LED0;              /* 每200ms,翻转一次LED0 */
        }

        delay_ms(10);
        t++;
	}
 }

