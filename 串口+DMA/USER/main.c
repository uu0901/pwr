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
换行符的追加逻辑：
                每当 TEXT_TO_SEND 的内容被完整复制到缓冲区后，
                代码会额外追加两个字节的换行符 \r\n（即 0x0D 和 0x0A）。
                因此，每复制一次完整的字符串，实际占用的空间为 sizeof(TEXT_TO_SEND) + 2 字节。
缓冲区对齐：
                如果发送缓冲区的大小是 sizeof(TEXT_TO_SEND) + 2 的整数倍，则可以确保：
                每次循环填充时，字符串和换行符的组合能完全填满缓冲区。
                避免最后一次填充时出现不完整的字符串片段或换行符被截断的情况

防止数据错位：
                若非整数倍，填充循环可能因剩余空间不足而无法正确追加完整的 \r\n，
                导致数据格式错误（如仅写入 \r 或 \a）。对齐后，
                每次填充均以完整的 TEXT_TO_SEND + \r\n 为单位，确保数据一致性。

假设 TEXT_TO_SEND 的长度为 30 字节，则每段数据为 30 + 2 = 32 字节。若缓冲区大小为 64（即 32 × 2），
则循环两次即可填满，避免残留空间。若非对齐（如 65 字节），最后一次填充将不完整，可能引发异常。
*/
#define SEND_BUF_SIZE           3200	                                             //发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.8200/40 =205
u8 SendBuff[SEND_BUF_SIZE];                                                          //发送数据缓冲区
const u8 TEXT_TO_SEND[]={"隆冬强 123456"};
int main(void)
{	 
	float pro=0;                                                                            //进度
    delay_init();	    	                                                             //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                                     //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    uart_init(115200);
    LED_Init();
    LCD_Init();			                                                                	//初始化LCD 	
    KEY_Init();
    MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);           //DMA1通道4,外设为串口1,存储器为SendBuff,长度SEND_BUF_SIZE.
    POINT_COLOR=RED;                                                                    //设置字体为红色 
    LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
    LCD_ShowString(30,70,200,16,16,"DMA TEST");	
    LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,  110,200,16,16,"2015/1/15");	
    LCD_ShowString(30,130,200,16,16,"KEY0:Start");
	u16 i;
    u8 j,mask=0;
    u8 t=0;
    j=sizeof(TEXT_TO_SEND);                                                                    //字符串字面量隐含 \0 终止符，因此 sizeof(TEXT_TO_SEND) = 字符串长度 + 1。 
    printf("%d\r\n", j);//14                                                                  //38  
    // printf("Size: %d\r\n", (int)sizeof("ALIENTEK WarShip STM32F1 DMA 串口实验"));           //sizeof 包含终止符 \0，printf 不发送终止符，因此 38 vs 37 的差异符合预期。
    // printf("Size: %d\r\n", (int)strlen("ALIENTEK WarShip STM32F1 DMA 串口实验"));              //37
    // printf("ALIENTEK WarShip STM32F1 DMA 串口实验");                                        //37
    for(i=0;i<SEND_BUF_SIZE;i++)//填充数据到SendBuff
    {
        if(t>=j)//加入换行符
        /*
        当t >= j时，表示已经复制完整个TEXT_TO_SEND字符串，此时加入换行符。
        否则，继续复制字符串中的字符。
        但关键点在于，TEXT_TO_SEND本身是否包含终止符，
        以及填充过程中是否复制了这个终止符。
        j是包含了停止符的因为是sizeof
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
        else//复制TEXT_TO_SEND语句
        {
            mask=0;
            SendBuff[i]=TEXT_TO_SEND[t];
            t++;
        }    	   
    }		 
    POINT_COLOR=BLUE;//设置字体为蓝色	  
    i=0;
    while(1)
    {
        t=KEY_Scan(0);
		if(t==KEY0_PRES)//KEY0按下
		{
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");//显示百分号
			// printf("\r\nDMA DATA:\r\n"); 	    
		    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //使能串口1的DMA发送      
			MYDMA_Enable(DMA1_Channel4);//开始一次DMA传输！	  
		    //等待DMA传输完成，此时我们来做另外一些事，点灯
		    //实际应用中，传输数据期间，可以执行另外的任务
		    while(1)
		    {
				if(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=RESET)	//判断通道4传输完成
				{
					DMA_ClearFlag(DMA1_FLAG_TC4);//清除通道4传输完成标志
					break; 
                }
				pro=DMA_GetCurrDataCounter(DMA1_Channel4);//得到当前还剩余多少个数据
				pro=1- pro/SEND_BUF_SIZE;               //得到百分比	  
				pro*=100;                               //扩大100倍

                /*         剩余数据量pro        计算过程            显示结果
                初始        8200              1-8200/8200=0         0%
                传输一半    4100              1-4100/8200=0.5       50%
                传输完成    0                 1-0/8200=1            100%
                */
				LCD_ShowNum(30,170,pro,3,16);	  
		    }			    
			LCD_ShowNum(30,170,100,3,16);//显示100%	  
			LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//提示传送完成
		}

		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
    }
 }

