#ifndef __LCD__
#define __LCD__

#include "LiquidCrystal_I2C.h"


class Lcd {

public:
    Lcd(int port, int colsNum, int rowsNum); 
    void display(String text, int row, int col);
    void clearLCD();

private:
    LiquidCrystal_I2C lcd;
};

#endif