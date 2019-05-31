#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#include "pitches.h"

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

char buf1[4];
char buf2[4];

unsigned int min_c_set = 5;
unsigned int min_c = 5;
unsigned int sec_c = 00;

int state = 1; // 1 == set min, 2 = set sec, 3 = counting, 4 = time up
boolean blink_s = true;

boolean beep_sound = true;

const int speaker_pin = A2;

unsigned long lastInterrupt;
unsigned long last_ms;

void setup()   {                
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();

  pinMode(A7, INPUT_PULLUP);
  attachInterrupt(A7, show_menu, LOW);

  pinMode(A3, INPUT_PULLUP);
  attachInterrupt(A3, count_down, LOW);

  pinMode(A0, INPUT_PULLUP);
  attachInterrupt(A0, count_up, LOW);

  tone(speaker_pin, NOTE_C7, 100);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("AsifAlam.com");
  display.setTextSize(2); 
  display.setCursor(0,25);
  display.println("TIMER V1.0");
  display.display();
  delay(3000);
}

void show_menu()
{
  if(millis() - lastInterrupt > 500)
  {    
    lastInterrupt = millis();
    if(state == 4)
    {
      state = 1;
      min_c = min_c_set;
      tone(speaker_pin, NOTE_DS7, 100);
    }
    else if(state == 3)
    {
      state = 1;
      tone(speaker_pin, NOTE_E3, 300);
    }
    else if(state == 1)
    {
      state = 2;
      sec_c = 0;
      tone(speaker_pin, NOTE_DS7, 100);
    }
    else if(state == 2)
    {
      if(min_c == 0 && sec_c == 0)
      {
        state = 1;
        tone(speaker_pin, NOTE_GS1, 50);
      }
      else
      {
        state = 3;
        tone(speaker_pin, NOTE_FS6, 400);
      }
    }
  }
}

void count_down()
{
  if(millis() - lastInterrupt > 200)
  { 
    blink_s = true;
    last_ms = millis();   
    lastInterrupt = millis();
    if(state == 1 && min_c > 0)
    {
      min_c --;
      min_c_set = min_c;
    }
    else if(state == 2 && sec_c > 0)
    {
      sec_c --;
    }
    else if(state == 3)
    {
      beep_sound = false;
    }
    tone(speaker_pin, NOTE_D6, 100);
  }
}

void count_up()
{
  if(millis() - lastInterrupt > 200)
  { 
    blink_s = true;
    last_ms = millis();   
    lastInterrupt = millis();
    if(state == 1 && min_c < 99)
    {
      min_c ++;
      min_c_set = min_c;
    }
    else if(state == 2 && sec_c < 58)
    {
      sec_c ++;
    }
    else if(state == 3)
    {
      beep_sound = true;
    }
    tone(speaker_pin, NOTE_E6, 100);
  }
}

void draw()
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  if(state == 4)
  {
    if(blink_s)
    {
      display.setTextSize(2);
      display.setCursor(17,0);
      display.println("TIME UP!");
      blink_s = false;
    }
    else
    {
      blink_s = true;
    }
    play_music();
  }
  else if(state == 3)
  {
    display.setTextSize(2);
    display.setCursor(17,0);
    display.println("Timer ON");
  }
  else if(state == 1)
  {
    display.setTextSize(2);
    display.setCursor(17,0);
    display.println("Set Min:");
  }
  else if(state == 2)
  {
    display.setTextSize(2);
    display.setCursor(17,0);
    display.println("Set Sec:");
  }

  
  display.setTextSize(4);
  if(state == 1 && !blink_s)
  {
    
  }
  else{
    // 16 is the top blue
    display.setCursor(5,25);
    sprintf (buf1, "%02i", min_c);
    display.print(buf1);
  }

  display.setCursor(55,25);
  display.print(":");

  if(state == 2 && !blink_s)
  {
    
  }
  else{
    display.setCursor(80,25);
    sprintf (buf2, "%02i", sec_c);
    display.print(buf2);
  }
  
  display.display();
}

void loop() {
  if(state == 3)
  {
    if(min_c == 0 && sec_c == 0)
    {
      state = 4;
      delay(400);
    }
    else
    {
      if(last_ms+990 <= millis())
      {
        last_ms = millis();
        sec_c --;
        if (sec_c == -1)
        {
          sec_c = 59;
          min_c --;
        }
        if(beep_sound)
        {
          tone(speaker_pin, NOTE_C7, 100);
        }
      }
    }
  }
  else if(state == 1 || state == 2)
  {
    if(last_ms+500 <= millis())
    {
      last_ms = millis();
      blink_s = !blink_s;
    }
    
  }
  draw();
}

void play_music()
{
  tone(speaker_pin, NOTE_E5, 400);
  delay(650);
}
