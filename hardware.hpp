#pragma once

#include <inttypes.h>
#include "Print.h"
#include "Arduino.h"

namespace board {
    class ShiftRegister
    {
        private: 
          int dataPin;
          int clockPin;
        public:
          ShiftRegister(int data, int clock);
          void set(int val);
          int value;
    };

    class Matrix
    {
        private:
          int d[3];
          ShiftRegister *reg;
          int row;
        public:
          void update();
          Matrix(int d1, int d2, int d3, ShiftRegister *r);
          bool state[4][3];
    };

    ///
    /// Modified version of the common 16x2 LCD lib
    ///
    class LCD_custom : public Print {
        public:
            LCD_custom(uint8_t enable, ShiftRegister *reg, int d4, int d5, int d6, int d7, int RS);

            void begin(uint8_t cols, uint8_t rows, uint8_t charsize = 0);

            void clear();
            void home();

            void noDisplay();
            void display();
            void noBlink();
            void blink();
            void noCursor();
            void cursor();
            void scrollDisplayLeft();
            void scrollDisplayRight();
            void leftToRight();
            void rightToLeft();
            void autoscroll();
            void noAutoscroll();

            void setRowOffsets(int row1, int row2, int row3, int row4);
            void createChar(uint8_t, uint8_t[]);
            void setCursor(uint8_t, uint8_t);

            void backlight();
            void noBacklight();
            
            virtual size_t write(uint8_t);
            void command(uint8_t);
            
        using Print::write;
        private:
            void send(uint8_t, uint8_t);
            void write4bits(uint8_t, bool mode);
            void pulseEnable();

            uint8_t _enable_pin; // activated by a HIGH pulse.

            uint8_t _displayfunction;
            uint8_t _displaycontrol;
            uint8_t _displaymode;
            
            uint8_t _initialized;

            uint8_t _rs_bit;
            uint8_t _data_bits[8];
            ShiftRegister *_reg;
            uint8_t _backlight_bit = 5;
            uint8_t _numlines;
            uint8_t _row_offsets[4];
    };

    class IO {
        private:
          ShiftRegister shift = ShiftRegister(16,15);  
        public:
          LCD_custom lcd = LCD_custom(D3, &shift, 0, 1, 2, 3, 4);
          Matrix matrix = Matrix(D5, D6, D7, &shift);
          void update();
    };
}
