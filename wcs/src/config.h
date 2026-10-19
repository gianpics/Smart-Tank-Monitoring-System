#ifndef __CONFIG__
#define __CONFIG__


// PIN
#define BTN_PIN     3
#define LED1_PIN    6   // green
#define LED2_PIN    5   // green
#define LED3_PIN    4   // red
#define MOTOR_PIN   10
#define LCD_PORT    0x27
#define POT_PIN     A0
/* LCD SDA => A4, SCL => A5 */


// Button
#define BTN_DEBOUNCE_TIME 2000 // ms


// Servo motor gate
#define VALVE_OPENING_DEG   90      // degrees
#define SERVO_ADJUST_TIME  100    // ms

// Potentiometer
#define POT_SYNC_EXPIRATION 50  // ms

// LCD
#define LCD_TEST_STRING "! TEST !"
#define LCD_PORT 0x27
#define LCD_COLS_NUM 20
#define LCD_ROWS_NUM 4
#define LCD_ROW_POS_1 1
#define LCD_COL_POS_1 1
#define LCD_ROW_POS_2 2
#define LCD_COL_POS_2 1

// Controller
#define UNCONNECTED_TEXT "UNCONNECTED"
#define AUTO_TEXT "AUTO"
#define MANUAL_TEXT "MANUAL"




#endif
