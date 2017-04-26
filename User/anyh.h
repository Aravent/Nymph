#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <timer.h>
#include <sys.h>
#include <oled.h>

#include <can.h>
#include <delay.h>


extern int flag_send;
extern int flag_testsend;
extern uint8_t testsend[21];
extern int flag_error_getDroneVersion;
extern int flag_dyhx_switch;
extern uint16_t mobile_data_t[100];

extern int co;
extern int so2;
extern int o3;
extern int no2;
extern int pm25;
extern int pm10;
