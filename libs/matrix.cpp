#include "shift.hpp"
#include "matrix.hpp"
#include "Arduino.h"

Matrix::Matrix(int d1, int d2, int d3, ShiftRegister *r)
{
    reg = r;
    d[0] = d1;
    d[1] = d2;
    d[2] = d3;
    for (int i = 0; i<3; i++)
    {
        pinMode(d[i], INPUT_PULLUP);
    }
    row = 0;
}

void Matrix::update()
{
  int val = reg->value & 0xf0;
  val |= (~(1<<row))&0xf;
  reg->set(val);
  for (int j = 0; j<3; j++)
  {
      auto s = digitalRead(d[j]);
      if (s)
      {
        Serial.print("row=");
      Serial.print(row);
      Serial.print(" column=");
      Serial.print(j);
      Serial.print(" state=");
      Serial.println(s);
      }
      state[row][j] = (s==0);
  }
  row = (row + 1)%1;
}
