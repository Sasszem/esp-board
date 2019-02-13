


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
