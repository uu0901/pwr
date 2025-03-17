#include "pwr.h"
#include "led.h"
#include "lcd.h"
#include "stm32f10x.h"
/*
 * @brief       ��ʼ��PVD��ѹ������
 * @param       pls: ��ѹ�ȼ�
                    PWR_PVDLevel_2V2  // 2.2V
                    PWR_PVDLevel_2V3  // 2.3V
                    PWR_PVDLevel_2V4  // 2.4V
                    PWR_PVDLevel_2V5  // 2.5V
                    PWR_PVDLevel_2V6  // 2.6V
                    PWR_PVDLevel_2V7  // 2.7V
                    PWR_PVDLevel_2V8  // 2.8V
                    PWR_PVDLevel_2V9  // 2.9V
 * @retval      ��
*/
// PVD��ʼ������
void PVD_Init(uint32_t pls)
{
    // 1. ʹ��PWRʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // 2. ����PVD��ֵ������ʵ������ѡ��
    PWR_PVDLevelConfig(pls); // 2.8V��ֵ

    // 3. ʹ��PVD���
    PWR_PVDCmd(ENABLE);

    // 4. ����EXTI��16��PVD���ӵ����ߣ�
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // ˫���ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 5. ������ܴ��ڵĹ����жϱ�־
    EXTI_ClearITPendingBit(EXTI_Line16);

    // 6. ����NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// PVD�жϷ�����
void PVD_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line16) != RESET)
    {
        // ���ʵ�ʵ�ѹ״̬
        if (PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET)/* ��ѹ��PLS��ѡ��ѹ���� */
        {
            // ��ѹ������ֵ���쳣����
            // ���磺�����������桢�ر�����Ȳ���
            POINT_COLOR = RED;//��������Ϊ��ɫ 
//            LCD_ShowString(30, 110, 200, 16, 16, "PVD Low Voltage!");/* LCD��ʾ��ѹ���� */
            LED1 = 0;
        }
        else
        {
            // ��ѹ�ָ���������ѡ����
            POINT_COLOR = BLUE;//��������Ϊ��ɫ 
//            LCD_ShowString(30, 110, 200, 16, 16, "PVD Voltage OK! ");/* LCD��ʾ��ѹ���� */
            LED1 = 1;
        }
        
        // ����жϱ�־
        EXTI_ClearITPendingBit(EXTI_Line16);
    }
}
