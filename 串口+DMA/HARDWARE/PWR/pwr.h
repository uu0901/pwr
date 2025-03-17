#ifndef __PWR_H
#define __PWR_H
#include "sys.h"
#define longpress 1
//为0时 key0关机 keyup开机
//为1时 keyup长按关机 keyup长按开机
//为1时候的原理
                //上电-->进入待机->短按->唤醒了->但进入WKUP_Init仍然进入待机
                //                长按->进入待机-》唤醒->但进入WKUP_Init唤醒
                //开机-->短按->不进入待机
                //       长按->进入待机
void PVD_Init(uint32_t pls);
void pwr_wkup_key_init(void);
void pwr_enter_sleep(void);
void pwr_enter_stop(void);
void pwr_enter_standby(void);
uint8_t  Check_WKUP(void);
void Sys_Enter_Standby(void);
#endif 
