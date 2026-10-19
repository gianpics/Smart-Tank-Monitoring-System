#include "Lcd.h"


Lcd::Lcd(int port, int colsNum, int rowsNum) : lcd(port, colsNum, rowsNum)
{
    lcd.init();
    lcd.backlight();
}

void Lcd::display(String text, int row, int col)
{
    lcd.setCursor(col, row);
    lcd.print(text);
}

void Lcd::clearLCD()
{
    lcd.clear();
}
