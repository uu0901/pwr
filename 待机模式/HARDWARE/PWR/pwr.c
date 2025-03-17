#include "pwr.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "key.h"
#include "stm32f10x.h"


/*
 * @brief       初始化PVD电压监视器
 * @param       pls: 电压等级
                    PWR_PVDLevel_2V2  // 2.2V
                    PWR_PVDLevel_2V3  // 2.3V
                    PWR_PVDLevel_2V4  // 2.4V
                    PWR_PVDLevel_2V5  // 2.5V
                    PWR_PVDLevel_2V6  // 2.6V
                    PWR_PVDLevel_2V7  // 2.7V
                    PWR_PVDLevel_2V8  // 2.8V
                    PWR_PVDLevel_2V9  // 2.9V
 * @retval      无
*/
// PVD初始化函数
void PVD_Init(uint32_t pls)
{
    // 1. 使能PWR时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // 2. 配置PVD阈值（根据实际需求选择）
    PWR_PVDLevelConfig(pls); // 2.8V阈值

    // 3. 使能PVD检测
    PWR_PVDCmd(ENABLE);

    // 4. 配置EXTI线16（PVD连接到此线）
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 双边沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 5. 清除可能存在的挂起中断标志
    EXTI_ClearITPendingBit(EXTI_Line16);

    // 6. 配置NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// PVD中断服务函数
void PVD_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line16) != RESET)
    {
        // 检查实际电压状态
        if (PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET)/* 电压比PLS所选电压还低 */
        {
            // 电压低于阈值（异常处理）
            // 例如：触发紧急保存、关闭外设等操作
            POINT_COLOR = RED;//设置字体为红色 
//            LCD_ShowString(30, 110, 200, 16, 16, "PVD Low Voltage!");/* LCD显示电压正常 */
            LED1 = 0;
        }
        else
        {
            // 电压恢复正常（可选处理）
            POINT_COLOR = BLUE;//设置字体为红色 
//            LCD_ShowString(30, 110, 200, 16, 16, "PVD Voltage OK! ");/* LCD显示电压正常 */
            LED1 = 1;
        }
        
        // 清除中断标志
        EXTI_ClearITPendingBit(EXTI_Line16);
    }
}
/**
 * @brief       低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式)
 * @param       无
 * @retval      无
 */
void pwr_wkup_key_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA和复用功能时钟

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;	 //PA.0
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化IO
    
    //使用外部中断方式
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);	//中断线0连接GPIOA.0

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//设置按键所有的外部线路
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	// 初始化外部中断

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //使能按键所在的外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级2级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}
//中断,检测到PA0脚的一个上升沿
//中断线0线上的中断检测
void EXTI0_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line0); // 清除LINE10上的中断标志位		  、
#if longpress == 1
	if(Check_WKUP())//关机?
    {
        Sys_Enter_Standby();
    }
#endif
} 
/**
 * @brief       进入CPU睡眠模式
 * @param       无
 * @retval      无
 */
void pwr_enter_sleep(void)
{
//    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // 手动关闭SysTick中断
    __WFI(); // 进入睡眠
//    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // 唤醒后恢复SysTick
}
/**
 * @brief       进入停止模式
 * @param       无
 * @retval      无
 */
void pwr_enter_stop(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
                                                            //停止模式和待机模式一定要记得开启
//    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // 手动关闭SysTick中断
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);	//STM32进入停止模式，并等待中断唤醒
}
/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void pwr_enter_standby(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
                                                            //停止模式和待机模式一定要记得开启
    /*使能WKUP引脚*/
    PWR_WakeUpPinCmd(ENABLE);						//使能位于PA0的WKUP引脚，WKUP引脚上升沿唤醒待机模式    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);        /* 需要清此标记，否则将保持唤醒状态 */
//    PWR_ClearFlag(PWR_FLAG_WU);
    PWR_EnterSTANDBYMode();						//STM32进入停止模式，并等待指定的唤醒事件（WKUP上升沿或RTC闹钟）
                                                /*待机模式唤醒后，程序会重头开始运行*/
}
//检测WKUP脚的信号
//返回值1:连续按下3s以上
//      0:错误的触发	
uint8_t  Check_WKUP(void) 
{
    u8 t=0;	//记录按下的时间
    LED0=0; //亮灯DS0 
    while(1)
    {
        if(WK_UP)
        {
            t++;			//已经按下了 
            delay_ms(30);
            if(t>=100)		//按下超过3秒钟
            {
                LED0=0;	 	//点亮DS0 
                return 1; 	//按下3s以上了
            }
        }else 
        { 
            LED0=1;
            return 0; //按下不足3秒
        }
    }
} 
//系统进入待机模式
void Sys_Enter_Standby(void)
{			 
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);	//复位所有IO口
	pwr_enter_standby();
}

