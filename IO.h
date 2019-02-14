#include "matrix.hpp"
#include "LCD.h"
#include "shift.hpp"
#include "Arduino.h"

class IO {
  private:
    ShiftRegister shift = ShiftRegister(16,15);  
  public:
    LCD lcd = LCD(D3, &shift, 0, 1, 2, 3, 4);
    Matrix matrix = Matrix(D5, D6, D7, &shift);
    void update();
};
