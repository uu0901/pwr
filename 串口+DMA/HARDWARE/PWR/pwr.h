#ifndef __PWR_H
#define __PWR_H
#include "sys.h"
#define longpress 1
//Ϊ0ʱ key0�ػ� keyup����
//Ϊ1ʱ keyup�����ػ� keyup��������
//Ϊ1ʱ���ԭ��
                //�ϵ�-->�������->�̰�->������->������WKUP_Init��Ȼ�������
                //                ����->�������-������->������WKUP_Init����
                //����-->�̰�->���������
                //       ����->�������
void PVD_Init(uint32_t pls);
void pwr_wkup_key_init(void);
void pwr_enter_sleep(void);
void pwr_enter_stop(void);
void pwr_enter_standby(void);
uint8_t  Check_WKUP(void);
void Sys_Enter_Standby(void);
#endif 
