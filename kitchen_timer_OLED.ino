// STM32 Blue Pill based Kitchen Timer V1.0
// SH1106 OLED
// OLED SCL --> B6, OLED SDA --> B7, OLED VCC --> 5V, OLED GND --> GND
// BTN MENU --> A7, BTN [-] --> A3, BTN [+] --> A0. All three common pins of each buttons are connected with GND
// AsifAlam.com


#include "U8glib.h"

#include <SPI.h>
#include <Wire.h>

#include "pitches.h"

U8GLIB_SH1106_128X64 u8g(5, 4, 10, 6 , 3);

unsigned int min_c = 0;
unsigned int sec_c = 10;

boolean blink_all = false;
volatile int state = 2; // 1 == set min, 2 = set sec, 3 = counting, 4 = time up

int menu_select = 2;

char buf1[4];
char buf2[4];

const int btn_up = 7;
const int btn_down =  8;

const int speaker_pin = 9;
const int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
const int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

void draw(void) {

  if (!blink_all && state == 3)
  {
    //u8g.setFont(u8g_font_unifont);
    //u8g.setFont(u8g_font_osb35);
    u8g.setFont(u8g_font_fub35n);



    //Serial.print(min_c);
    //Serial.println(sec_c);

    sprintf (buf1, "%02i", min_c);
    u8g.drawStr(0, 36, buf1);

    u8g.drawStr( 56, 30, ":");

    sprintf (buf2, "%02i", sec_c);
    u8g.drawStr(73, 36, buf2);
  }
  else if (state == 1)
  {
    u8g.setFont(u8g_font_fub35n);
    sprintf (buf1, "%02i", min_c);
    u8g.drawStr(0, 36, buf1);

    u8g.drawStr( 56, 30, ":");
  }
  else if (state == 2)
  {
    u8g.setFont(u8g_font_fub35n);

    u8g.drawStr( 56, 30, ":");
    sprintf (buf2, "%02i", sec_c);
    u8g.drawStr(73, 36, buf2);
  }
  else
  {
    u8g.setFont(u8g_font_fub35n);

    sprintf (buf1, "%02i", min_c);
    u8g.drawStr(0, 36, buf1);

    u8g.drawStr( 56, 30, ":");

    sprintf (buf2, "%02i", sec_c);
    u8g.drawStr(73, 36, buf2);

    u8g.setFont(u8g_font_unifont);
    u8g.setScale2x2();
    u8g.drawStr(0, 31, "TIME UP!");
    u8g.undoScale();
  }
}

void blink() {
  tone(9, NOTE_A1, 5);
  if (state == 0)
  {
    state = 1;
  }
  else if (state == 1)
  {
    state = 2;
  }
  else if (state == 2)
  {
    blink_all = false;
    state = 3;
  }
  else if (state == 3)
  {
    state = 1;
  }
  Serial.println(state);
}

void setup(void) {

  Serial.begin(9600);
  Serial.println(u8g.getMode());
  u8g.setColorIndex(1);

  pinMode(btn_up, INPUT_PULLUP);
  pinMode(btn_down, INPUT_PULLUP);

  pinMode(menu_select, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(menu_select), blink, LOW);

  // flip screen, if required
  // u8g.setRot180();

  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  /*if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
    }
    else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
    }
    else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
    }
    else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
    }*/
}

void callback()
{
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
}

void loop() {

  if (state == 3)
  {
    if (min_c == 0 && sec_c == 0)
    {
      if (blink_all)
        blink_all = false;
      else
        blink_all = true;
        play_music();
        delay(500);
    }
    else
    {
      sec_c --;
      if (sec_c == -1)
      {
        sec_c = 59;
        min_c --;
      }
      delay(1000);
    }
  }
  else if (state == 1)
  {
    if(digitalRead(btn_up) == LOW)
    {
      tone(speaker_pin, NOTE_B3, 4);
      if(min_c<99)
      min_c ++;
    }
    else if(digitalRead(btn_down) == LOW)
    {
      tone(speaker_pin, NOTE_D3, 4);
      if(min_c>0)
      min_c --;
    }
  }
  else if (state == 2)
  {
    if(digitalRead(btn_up) == LOW)
    {
      tone(speaker_pin, NOTE_B3, 4);
      if(sec_c<59)
      sec_c ++;
    }
    else if(digitalRead(btn_down) == LOW)
    {
      tone(speaker_pin, NOTE_D3, 4);
      if(sec_c>0)
      sec_c --;
    }
  }


  callback();

}

void play_music()
{
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speaker_pin, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speaker_pin);
  }
}
