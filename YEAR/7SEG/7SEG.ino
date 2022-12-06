#include <SegmentDisplay_CD4511B.h>
#include <HardwareSerial.h>

HardwareSerial SerialPort1(1);


#define BCD3 23
#define BCD2 22
#define BCD1 21
#define BCD0 19

#define LE3 26
#define LE2 14
#define LE1 12
#define LE0 13

// the number that will get displayed
int number = 0;
int _mode  = -1; // -1: normall, -2:blank, -3:confirm
// TODO: IMPLEMENT BLANK

/*
 * The parameters of the SegmentDisplay_CD4511B constructor:
 *    First 4 numbers are the Arduino pins connected to the  A, B, C, D pins of the CD4511B chip(s)
 *      in this example 6,8,7,5 are the pins used
 *    5-th number is the number of chips used 
 *      in this example 2 chips are used to drive 2 displays
 *    the remaining pins are the Arduino pins connected to the latching pins of the CD4511B chip(s) 
 *      in this example 9 and 10 are the numbers of the latching pins
*/
SegmentDisplay_CD4511B displayDriver(BCD3, BCD2, BCD1, BCD0, LE3, LE2, LE1, LE0);

void setup() {
  SerialPort1.begin(15200, SERIAL_8N1, 4, 2);  
}

void loop() {
  if (SerialPort1.available())
  {
    int num = SerialPort1.read();
    if(num >= 0) number = num;
    else if (num == -2) _mode = -2; 
    else _mode = -1;
  } 
  displayDriver.showNumber(number);    
}
