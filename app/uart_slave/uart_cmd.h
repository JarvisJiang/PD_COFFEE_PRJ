#ifndef _UART_CMD_H_
#define _UART_CMD_H_

#define CMT_SET_GET_VERSION    		 					0X10

#define CMT_SET_GET_SWITCH_STATE     					0X11
#define 	GET_SWITCH_STATE_ATTACH_SWITCH					0X1
#define 	GET_SWITCH_STATE_ATTACH_VALVES          		0X2
#define 	GET_SWITCH_STATE_ATTACH_SOLENOIDS       		0X3  //螺线管
#define 	GET_SWITCH_STATE_ATTACH_RELAY           		0X4  //继电器
#define CMT_SET_CALIBRATE_GRIND_SIZE_STEP_MOTOR 		0X20
#define 	SET_STEP_MOTOR_DIRCTION						    0X1
#define     SET_PERCENTAGE_DISTANCE                         0X2
#define     SET_COARSE_MODE_DIRCTION                        0X3
#define     GET_PERCENTAGE_DISTANCE 						0X4//Present grind size ranging from
#define     SET_MOVE_TO_FINE_COARSE							0X5//
#define     SET_PERCENTAGE_DISTANCE							0X6
#define     SET_PERCENTAGE_DISTANCE							0X7//repeat 0x3
#define     GET_MOTOR_STATE   								0x8
#define CMT_SET_GRAND_MOTOR_TIME_CTL					 0X21  
#define 	SET_GRAND_TIME						    	 	0X1
#define     SET_STOP_IMMEDIATELY                       		0X2
#define     GET_WEIGHT				                        0X3
#define     SET_WEIGHT_TARE_LOAD_MODE			 			0X4
#define     SET_CAL_GRIND_SPEED_MODE						0X5//
#define     GET_CAL_GRIND_SPEED								0X6
#define     GET_CERTIAN_WEIGHT_OF_COFFE					    0X7//
#define     GET_GRINDER_STATE   							0x8
#define CMT_SET_HEAT_UP_CTL					 			 0X22  
#define 	SET_HEAT_UP_TEMP						    	0X1
#define     SET_TURN_ON_BOILER                       		0X2
#define     SET_TURN_OFF_BOILER				                0X3
#define     SET_PID_PERIOD						 			0X4//PWM 周期 pid???
#define     SET_PID_STEP						 			0X5//PWM 周期 pid???
#define     GET_CUREENT_TEMP								0X6//??
#define     GET_BOILER_STATE								0X7
#define CMT_PULL_WATER_CTL                               0X23
#define     GET_CERTIAN_VOLUME                              0X1
#define     GET_AND_PULL_MAX_VOLUME                         0X2
#define    	SET_PUSH_ALL_WATER_WITHIN_TIME                  0X3
#define     GET_PUSH_CERTIAN_SPEED                          0X4
#define     SET_PULL_MOVING_SPEED                           0X5
#define     SET_DEFAULT_MAX_VOLUME                          0X6
#define     SET_PULL_COMPRESSOR_DIRECTION					0X7
#define     GET_STATE_COMPRESSOR           					0X8
#define CMT_TAMPER_POSITION_CTL                          0X24
#define CMT_SOLENOID_CTL                                 0X25
#define CMT_STEAM_CTL                                    0X26
#define CMT_WATE_PUMP_CTL                                0X27
#define CMT_SOLENOID_AIR_CTL                             0X28
#define CMT_IR_SENSOR_CTL                                0X29
#define CMT_FORTHER_CTL                                  0X2A //泡沫发生器
#define CMT_GET_XX_CERTIAN_VALUE                         0X2B

#endif
