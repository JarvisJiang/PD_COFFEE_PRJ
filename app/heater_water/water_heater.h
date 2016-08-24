#ifndef _WATER_HEATER_H_
#define _WATER_HEATER_H_


void init_heat_up(void);//PA6
void all_speed_heat_up(void);
void stop_water_heat_up(void);
void config_heat_up_pwm(float ratio);
void test_heat_up(void);
void test_pwm_frq(void);
static void set_pwm_ch3_output(char bit);
void water_heater_test_pid(void);
void send_char_com(unsigned char  ch);
void test_labview_waveform(void);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);

extern int g_set_temper ,g_temp_offset;

struct PID_WATER_HEATER {
unsigned int Proportion; //  Proportional Const
unsigned int Integral; //  Integral Const
unsigned int Derivative; //  Derivative Const
unsigned int LastError; // Error[-1]
unsigned int PrevError; // Error[-2]
};
extern struct PID_WATER_HEATER spid; // PID Control Structure

extern char RI_Buffer[64];
void test_pid_version2(void);
#define  PID_ADJUST_USART   UART4
#define PID_ADJUST_USART_INT_FUN UART4_IRQHandler
#endif

