#include <Arduino.h>

#include "LogSerial.h" 
#include <M5Core2.h>


LogSerial *syslog;
// Defines gestures
Gesture swipeRight("swipe right", 160, DIR_RIGHT, 30, true);
Gesture swipeDown("swipe down", 120, DIR_DOWN, 30, true);
Gesture swipeLeft("swipe left", 160, DIR_LEFT, 30, true);
Gesture swipeUp("swipe up", 120, DIR_UP, 30, true);

void eventDisplay(Event &e)
{ 

    syslog->printf("%-12s finger%d  %-18s (%3d, %3d) --> (%3d, %3d)   ",
                   e.typeName(), e.finger, e.objName(), e.from.x, e.from.y,
                   e.to.x, e.to.y);
    syslog->printf("( dir %d deg, dist %d, %d ms )\n", e.direction(),
                   e.distance(), e.duration);
}

void Write1Byte(uint8_t Addr, uint8_t Data)
{
  Wire1.beginTransmission(0x34);
  Wire1.write(Addr);
  Wire1.write(Data);
  Wire1.endTransmission();
}

uint8_t Read8bit(uint8_t Addr)
{
  Wire1.beginTransmission(0x34);
  Wire1.write(Addr);
  Wire1.endTransmission();
  Wire1.requestFrom(0x34, 1);
  return Wire1.read();
}

/* After M5Core2 is started or reset
  the program in the setUp () function will be run, and this part will only be run once.
  在 M5Core2 启动或者复位后，即会开始执行setup()函数中的程序，该部分只会执行一次。 */
void setup() {
  syslog = new LogSerial();
  syslog->begin(115200);

  Wire.begin(32, 33);     // I2C enable
  Wire1.begin(21, 22);    // AXP begin
  Wire1.setClock(400000); // AXP
  // AXP192 30H
  Write1Byte(0x30, (Read8bit(0x30) & 0x04) | 0X02);
  // AXP192 GPIO1:OD OUTPUT
  Write1Byte(0x92, Read8bit(0x92) & 0xf8);
  // AXP192 GPIO2:OD OUTPUT
  Write1Byte(0x93, Read8bit(0x93) & 0xf8);
  // AXP192 RTC CHG
  Write1Byte(0x35, (Read8bit(0x35) & 0x1c) | 0xa2);
  // AXP192 GPIO4
  Write1Byte(0X95, (Read8bit(0x95) & 0x72) | 0X84);
  Write1Byte(0X36, 0X4C);
  Write1Byte(0x82, 0xff);

  M5.Axp.SetESPVoltage(3350);
  M5.Axp.SetBusPowerMode(1); // 1 - Power from bus; 0 - Power from USB
  M5.Axp.SetLDOVoltage(2, 3300);
  M5.Axp.SetLDOVoltage(3, 2000);
  M5.Axp.SetLDOEnable(2, true);
  M5.Axp.SetDCDC3(false);
  M5.Axp.SetLed(false);
  M5.Axp.SetSpkEnable(false);

  M5.Axp.SetLcdVoltage(2500);
  M5.Axp.SetLCDRSet(0);
  delay(100);
  M5.Axp.SetLCDRSet(1);
  M5.Touch.begin();
  M5.Rtc.begin();
  delay(100);

  M5.begin();  //Init M5Core.  初始化 M5Core2
  M5.Lcd.setTextColor(
      YELLOW);  //Set the font color to yellow.  设置字体颜色为黄色
  M5.Lcd.setTextSize(2);  //Set the font size.  设置字体大小为2
  M5.Lcd.setCursor(
      65, 10);  //Move the cursor position to (x, y).  移动光标位置到 (x, y)处
  M5.Lcd.println(
      "Button example");  //The screen prints the formatted string and wraps the line.  输出格式化字符串并换行
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.println("Press button B for 700ms");
  M5.Lcd.println("to clear screen.");
  M5.Lcd.setTextColor(RED);
 
   M5.background.delHandlers();
  uint16_t events = (false) ? E_ALL : (E_ALL - E_MOVE); // Show all events, or everything but E_MOVE? Controlled with A button.
  M5.background.tapTime = 200;
  M5.background.dbltapTime = 300;
  
  M5.background.longPressTime = 700;
  M5.background.repeatDelay = 250;
  M5.background.repeatInterval = 250;
  M5.background.addHandler(eventDisplay, events);
   M5.Buttons.addHandler(eventDisplay, events);
}

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
在setup()函数中的程序执行完后，会接着执行loop()函数中的程序
loop()函数是一个死循环，其中的程序会不断的重复运行 */
void loop() {
  M5.update();  //Read the press state of the key.  读取按键 A, B, C 的状态
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) {
    M5.Lcd.print('A');
    syslog->println("A");
  } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
    M5.Lcd.print('B');
    syslog->println("B");
  } else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200)) {
    M5.Lcd.print('C');
    syslog->println("C");
  } else if (M5.BtnB.wasReleasefor(700)) {
    M5.Lcd.clear(
        WHITE);  // Clear the screen and set white to the background color.  清空屏幕并将白色设置为底色
    M5.Lcd.setCursor(0, 0);
  }

  M5.update();  //Read the press state of the key.  读取按键 A, B, C 的状态
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) {
    M5.Lcd.print('A');
    syslog->println("A2");
  } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
    M5.Lcd.print('B');
    syslog->println("B2");
  } else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200)) {
    M5.Lcd.print('C');
    syslog->println("C2");
  } else if (M5.BtnB.wasReleasefor(700)) {
    M5.Lcd.clear(
        WHITE);  // Clear the screen and set white to the background color.  清空屏幕并将白色设置为底色
    M5.Lcd.setCursor(0, 0);
  }
}