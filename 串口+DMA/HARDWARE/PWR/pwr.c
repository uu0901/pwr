#include "pwr.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "key.h"
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
/**
 * @brief       �͹���ģʽ�µİ�����ʼ��(���ڻ���˯��ģʽ/ֹͣģʽ)
 * @param       ��
 * @retval      ��
 */
void pwr_wkup_key_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIOA�͸��ù���ʱ��

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;	 //PA.0
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPD;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��IO
    
    //ʹ���ⲿ�жϷ�ʽ
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);	//�ж���0����GPIOA.0

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//���ð������е��ⲿ��·
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж�����
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	// ��ʼ���ⲿ�ж�

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //�����ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
//�ж�,��⵽PA0�ŵ�һ��������
//�ж���0���ϵ��жϼ��
void EXTI0_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line0); // ���LINE10�ϵ��жϱ�־λ		  ��
#if longpress == 1
	if(Check_WKUP())//�ػ�?
    {
        Sys_Enter_Standby();
    }
#endif
} 
/**
 * @brief       ����CPU˯��ģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_sleep(void)
{
//    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // �ֶ��ر�SysTick�ж�
    __WFI(); // ����˯��
//    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  // ���Ѻ�ָ�SysTick
}
/**
 * @brief       ����ֹͣģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_stop(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//����PWR��ʱ��
                                                            //ֹͣģʽ�ʹ���ģʽһ��Ҫ�ǵÿ���
//    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; // �ֶ��ر�SysTick�ж�
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);	//STM32����ֹͣģʽ�����ȴ��жϻ���
}
/**
 * @brief       �������ģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_standby(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//����PWR��ʱ��
                                                            //ֹͣģʽ�ʹ���ģʽһ��Ҫ�ǵÿ���
    /*ʹ��WKUP����*/
    PWR_WakeUpPinCmd(ENABLE);						//ʹ��λ��PA0��WKUP���ţ�WKUP���������ػ��Ѵ���ģʽ    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);        /* ��Ҫ��˱�ǣ����򽫱��ֻ���״̬ */
//    PWR_ClearFlag(PWR_FLAG_WU);
    PWR_EnterSTANDBYMode();						//STM32����ֹͣģʽ�����ȴ�ָ���Ļ����¼���WKUP�����ػ�RTC���ӣ�
                                                /*����ģʽ���Ѻ󣬳������ͷ��ʼ����*/
}
//���WKUP�ŵ��ź�
//����ֵ1:��������3s����
//      0:����Ĵ���	
uint8_t  Check_WKUP(void) 
{
    u8 t=0;	//��¼���µ�ʱ��
    LED0=0; //����DS0 
    while(1)
    {
        if(WK_UP)
        {
            t++;			//�Ѿ������� 
            delay_ms(30);
            if(t>=100)		//���³���3����
            {
                LED0=0;	 	//����DS0 
                return 1; 	//����3s������
            }
        }else 
        { 
            LED0=1;
            return 0; //���²���3��
        }
    }
} 
//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{			 
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);	//��λ����IO��
	pwr_enter_standby();
}

