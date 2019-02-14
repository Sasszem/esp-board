#include "shift.hpp"

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
