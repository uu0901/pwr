#include "pwr.h"
#include "led.h"
#include "lcd.h"
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
