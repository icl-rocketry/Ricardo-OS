//Global Config
#ifndef CONFIG_H
#define CONFIG_H

//uncomment to show all debug messages
#define VERBOSE

//nominal charged battery voltage (mV)
#define full_battery_voltage 4200

//message flag definitions//

#define NONE 0
//state flags
#define STATE_USBMODE (1 << 0) 
#define STATE_PREFLIGHT (1 << 1)
#define STATE_COUNTDOWN (1 << 2)
#define STATE_FLIGHT (1 << 3)
#define STATE_RECOVERY (1 << 4)
//critical messages 
#define ERROR_SPI (1 << 5)
#define ERROR_I2C (1 << 6)
#define ERROR_SERIAL (1 << 7)
#define ERROR_LORA (1 << 8)
#define ERROR_BARO (1 << 9)
#define ERROR_BATT (1 << 10)
#define ERROR_GPS (1 << 11)
#define ERROR_IMU (1 << 12)
#define ERROR_MAG (1 << 13)
#define ERROR_KALMAN (1 << 14)
#define ERROR_SD (1 << 15)
#define ERROR_FLASH (1 << 16)
//if rocket is inverted
#define ERROR_ORIENTATION (1 << 17)
//warn
#define WARN_BATT (1 << 18)

//info




#endif







