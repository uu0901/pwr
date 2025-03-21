#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "pwr.h"
#include "usart.h"
#include "key.h"
#include "dma.h"
#include "string.h"
/*
���з���׷���߼���
                ÿ�� TEXT_TO_SEND �����ݱ��������Ƶ���������
                ��������׷�������ֽڵĻ��з� \r\n���� 0x0D �� 0x0A����
                ��ˣ�ÿ����һ���������ַ�����ʵ��ռ�õĿռ�Ϊ sizeof(TEXT_TO_SEND) + 2 �ֽڡ�
���������룺
                ������ͻ������Ĵ�С�� sizeof(TEXT_TO_SEND) + 2 ���������������ȷ����
                ÿ��ѭ�����ʱ���ַ����ͻ��з����������ȫ������������
                �������һ�����ʱ���ֲ��������ַ���Ƭ�λ��з����ضϵ����

��ֹ���ݴ�λ��
                ���������������ѭ��������ʣ��ռ䲻����޷���ȷ׷�������� \r\n��
                �������ݸ�ʽ�������д�� \r �� \a���������
                ÿ�������������� TEXT_TO_SEND + \r\n Ϊ��λ��ȷ������һ���ԡ�

���� TEXT_TO_SEND �ĳ���Ϊ 30 �ֽڣ���ÿ������Ϊ 30 + 2 = 32 �ֽڡ�����������СΪ 64���� 32 �� 2����
��ѭ�����μ�����������������ռ䡣���Ƕ��루�� 65 �ֽڣ������һ����佫�����������������쳣��
*/
#define SEND_BUF_SIZE           3200	                                             //�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.8200/40 =205
u8 SendBuff[SEND_BUF_SIZE];                                                          //�������ݻ�����
const u8 TEXT_TO_SEND[]={"¡��ǿ 123456"};
int main(void)
{	 
	float pro=0;                                                                            //����
    delay_init();	    	                                                             //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                                     //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);
    LED_Init();
    LCD_Init();			                                                                	//��ʼ��LCD 	
    KEY_Init();
    MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);           //DMA1ͨ��4,����Ϊ����1,�洢��ΪSendBuff,����SEND_BUF_SIZE.
    POINT_COLOR=RED;                                                                    //��������Ϊ��ɫ 
    LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
    LCD_ShowString(30,70,200,16,16,"DMA TEST");	
    LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,  110,200,16,16,"2015/1/15");	
    LCD_ShowString(30,130,200,16,16,"KEY0:Start");
	u16 i;
    u8 j,mask=0;
    u8 t=0;
    j=sizeof(TEXT_TO_SEND);                                                                    //�ַ������������� \0 ��ֹ������� sizeof(TEXT_TO_SEND) = �ַ������� + 1�� 
    printf("%d\r\n", j);//14                                                                  //38  
    // printf("Size: %d\r\n", (int)sizeof("ALIENTEK WarShip STM32F1 DMA ����ʵ��"));           //sizeof ������ֹ�� \0��printf ��������ֹ������� 38 vs 37 �Ĳ������Ԥ�ڡ�
    // printf("Size: %d\r\n", (int)strlen("ALIENTEK WarShip STM32F1 DMA ����ʵ��"));              //37
    // printf("ALIENTEK WarShip STM32F1 DMA ����ʵ��");                                        //37
    for(i=0;i<SEND_BUF_SIZE;i++)//������ݵ�SendBuff
    {
        if(t>=j)//���뻻�з�
        /*
        ��t >= jʱ����ʾ�Ѿ�����������TEXT_TO_SEND�ַ�������ʱ���뻻�з���
        ���򣬼��������ַ����е��ַ���
        ���ؼ������ڣ�TEXT_TO_SEND�����Ƿ������ֹ����
        �Լ����������Ƿ����������ֹ����
        j�ǰ�����ֹͣ������Ϊ��sizeof
        */
        {
            if(mask)
            {
                SendBuff[i]=0x0a;// \n
                t=0;
            }else 
            {
                SendBuff[i]=0x0d;// \r
                mask++;
            }	
        }
        else//����TEXT_TO_SEND���
        {
            mask=0;
            SendBuff[i]=TEXT_TO_SEND[t];
            t++;
        }    	   
    }		 
    POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
    i=0;
    while(1)
    {
        t=KEY_Scan(0);
		if(t==KEY0_PRES)//KEY0����
		{
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");//��ʾ�ٷֺ�
			// printf("\r\nDMA DATA:\r\n"); 	    
		    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���1��DMA����      
			MYDMA_Enable(DMA1_Channel4);//��ʼһ��DMA���䣡	  
		    //�ȴ�DMA������ɣ���ʱ������������һЩ�£����
		    //ʵ��Ӧ���У����������ڼ䣬����ִ�����������
		    while(1)
		    {
				if(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=RESET)	//�ж�ͨ��4�������
				{
					DMA_ClearFlag(DMA1_FLAG_TC4);//���ͨ��4������ɱ�־
					break; 
                }
				pro=DMA_GetCurrDataCounter(DMA1_Channel4);//�õ���ǰ��ʣ����ٸ�����
				pro=1- pro/SEND_BUF_SIZE;               //�õ��ٷֱ�	  
				pro*=100;                               //����100��

                /*         ʣ��������pro        �������            ��ʾ���
                ��ʼ        8200              1-8200/8200=0         0%
                ����һ��    4100              1-4100/8200=0.5       50%
                �������    0                 1-0/8200=1            100%
                */
				LCD_ShowNum(30,170,pro,3,16);	  
		    }			    
			LCD_ShowNum(30,170,100,3,16);//��ʾ100%	  
			LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//��ʾ�������
		}

		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}		   
    }
 }

