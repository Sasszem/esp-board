// hardware.cpp
// basic IO utilities
#include "hardware.hpp"
#include "Arduino.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

board::ShiftRegister::ShiftRegister(int data, int clock)
{
    dataPin = data;
    clockPin = clock;
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
}

void board::ShiftRegister::set(int value)
{
    shiftOut(dataPin, clockPin, MSBFIRST, value);
    this->value = value;
}

board::Matrix::Matrix(int d1, int d2, int d3, ShiftRegister *r)
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

void board::Matrix::update()
{
  int val = reg->value & 0xf0;
  val |= (~(1<<row))&0xf;
  reg->set(val);
  for (int j = 0; j<3; j++)
  {
      auto s = digitalRead(d[j]);
      state[row][j] = (s==0);
  }
  row = (row + 1)%1;
}

void board::IO::update()
{
  matrix.update();
}

// LCD constants

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00



// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LCD constructor is called).

board::LCD_custom::LCD_custom(uint8_t enable, ShiftRegister *reg, int d4, int d5, int d6, int d7, int RS)
{
  _enable_pin = enable;
  _rs_bit = RS;
  _reg = reg;
  _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  _data_bits[0] = d4;
  _data_bits[1] = d5;
  _data_bits[2] = d6;
  _data_bits[3] = d7; 
  begin(16, 1);  
}

void board::LCD_custom::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;

  setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);  

  pinMode(_enable_pin, OUTPUT);
  

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  delayMicroseconds(50000); 
  // Now we pull both RS and R/W low to begin commands
  digitalWrite(_enable_pin, LOW);
  

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03, false);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try
    write4bits(0x03, false);
    delayMicroseconds(4500); // wait min 4.1ms
    
    // third go!
    write4bits(0x03, false); 
    delayMicroseconds(150);

    // finally, set to 4-bit interface
    write4bits(0x02, false); 

  
  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);
  backlight();
}

void board::LCD_custom::setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void board::LCD_custom::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void board::LCD_custom::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void board::LCD_custom::setCursor(uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= _numlines ) {
    row = _numlines - 1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void board::LCD_custom::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void board::LCD_custom::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void board::LCD_custom::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void board::LCD_custom::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void board::LCD_custom::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void board::LCD_custom::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void board::LCD_custom::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void board::LCD_custom::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void board::LCD_custom::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void board::LCD_custom::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void board::LCD_custom::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void board::LCD_custom::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void board::LCD_custom::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}


void board::LCD_custom::backlight() {
  _reg->set(_reg->value | (1<<_backlight_bit));
}
void board::LCD_custom::noBacklight() {
  _reg->set(_reg->value & ~(1<<_backlight_bit));
}

/*********** mid level commands, for sending data/cmds */

inline void board::LCD_custom::command(uint8_t value) {
  send(value, LOW);
}

inline size_t board::LCD_custom::write(uint8_t value) {
  send(value, HIGH);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void board::LCD_custom::send(uint8_t value, uint8_t mode) {
    write4bits(value>>4, mode==HIGH);
    write4bits(value, mode==HIGH);
}

void board::LCD_custom::pulseEnable(void) {
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(1);    
  digitalWrite(_enable_pin, HIGH);
  delayMicroseconds(1);    // enable pulse must be >450ns
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(100);   // commands need > 37us to settle
}

void board::LCD_custom::write4bits(uint8_t value, bool RS) {
  uint8_t old_value = _reg->value;
  uint8_t to_write = 0;
  for (int i = 0; i < 4; i++) {
    to_write |= (((value >> i) & 0x01)<<_data_bits[i]);
  }

  if (RS)
  {
    to_write |= 1<<_rs_bit;
  }
  
  _reg->set(to_write);
  pulseEnable();

  _reg->set(old_value);
}
