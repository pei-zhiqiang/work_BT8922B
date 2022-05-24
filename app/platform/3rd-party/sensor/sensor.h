#ifndef _API_SENSOR_H
#define _API_SENSOR_H

#include "include.h"

#if SC7A20_EN
    #include "..\..\platform\3rd-party\sensor\sc7a20.h"
    #define gsensor_init()          SC7A20_Config()
    #define gsensor_process()       sc7a20_status_scan()
    #define gsensor_sleep()
    #define gsensor_wakeup()        SC7A20_Config_int()     //唤醒后，重新配置敲击芯片使能中断
    #define gsensor_lowpwr()        sc7a20_enter_lowpow()
#else
    #define gsensor_init()          //初始化
    #define gsensor_process()       //状态处理
    #define gsensor_sleep()         //进入sleep
    #define gsensor_wakeup()        //退出sleep
    #define gsensor_lowpwr()        //进入lowpwr
#endif

#endif //_API_SENSOR_H

