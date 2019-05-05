//http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi
#include "ColorConverterLib.h"
#include <timer.h>

#include <OneWire.h> // fo temperatures DS18B20
#include <DallasTemperature.h>

//#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const int DISPLAY_ROWS = 4;
const int DISPLAY_COLS = 20;

LiquidCrystal_I2C lcd(0x27,DISPLAY_ROWS,DISPLAY_COLS); 
auto timer = timer_create_default();

const int MIN_FAN_SPEED = 900;
const int MAX_FAN_SPEED = 2700;
const int FAN_PIN =12;
const int FAN_RPM_PIN =36;

const int SLIDER_A = A0; 
const int SLIDER_B = A1; 
const int SLIDER_C = A2;

const int RED_PIN = 3;  
const int GREEN_PIN = 4; 
const int BLUE_PIN = 5;  

const int pot_color_threshold = 2;

const int ONE_WIRE_BUS = 10;

OneWire oneWire(ONE_WIRE_BUS); //term pin with 4.7k resistore
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
//byte addr[8]; //enable for debugging purpuses

int potVal_A;
int potVal_B;
int potVal_C;

int potVal_A_prev;
int potVal_B_prev;
int potVal_C_prev;

double H;
double S;
double L;

//int fan_ticks=0; 

char LCD_line[DISPLAY_ROWS][DISPLAY_COLS];
char LCD_line_prev[DISPLAY_ROWS][DISPLAY_COLS];

/*char LCD_line2[20];
char LCD_line3[20];
char LCD_line4[20];

char LCD_line1_prev[20];
char LCD_line2_prev[20];
char LCD_line3_prev[20];
char LCD_line4_prev[20];*/

//void pickrpm ()
//This is the interrupt subroutine that increments ticks counts for each HES response.
//{ fan_ticks++; }

void setHsl(double _H, double _S, double _L) {

  uint8_t red;
  uint8_t green;
  uint8_t blue;

  H = _H;
  S = _S;
  L = _L;
  
  ColorConverter::HslToRgb(H/360, S/100, L/100, red,green,blue);
 
  sprintf(LCD_line[0], "HSL %d %d %d", (int)H,(int)S,(int)L);
  sprintf(LCD_line[1], "RGB %d %d %d", red,green,blue);
    
  setRgb(red,green,blue);
  
}
 
void setRgb(int r, int g, int b) {

  //int redBr = map(r, 0, 255, 0, 80); // temporary needed not to burn the led
  //log_printf("R:%i(%i) G:%i(%i) B:%i(%i)\n", redBr, 255-redBr,greenBr, 255-greenBr, blueBr,255-blueBr);

  analogWrite(RED_PIN, 255-r);
  analogWrite(GREEN_PIN, 255-g);
  analogWrite(BLUE_PIN, 255-b);

}

void update_lcd() {

   char LCD_line_empty[DISPLAY_COLS];

   for (int line=0; line < DISPLAY_ROWS; line++) {

    //Serial.println(LCD_line[line]);
    //Serial.println(LCD_line_prev[line]);
    int ret = strcmp(LCD_line[line], LCD_line_prev[line]);
    
    //Serial.print("======");Serial.println(ret);
    
    if (strcmp(LCD_line[line], LCD_line_prev[line]) !=0 ) {
      
       //Serial.print(line);Serial.print(": ");
       lcd.setCursor(0,line);
       for (int i=0; i < DISPLAY_COLS-1; i++) {
        LCD_line_empty[i]=(char)32;
       }
       LCD_line_empty[19]="\0";
       
      strncpy(LCD_line_empty, LCD_line[line], strlen(LCD_line[line]));
      strncpy(LCD_line[line], LCD_line_empty, 20);   
      strncpy(LCD_line_prev[line], LCD_line[line], 20);
      lcd.print(LCD_line[line]);
      

       
      }
     }
   
   /*if (strcmp(LCD_line2, LCD_line2_prev) !=0 ) {
     lcd.setCursor(0,1);
     lcd.print(LCD_line2);
   }
   if (strcmp(LCD_line3, LCD_line3_prev) !=0 ) {
     lcd.setCursor(0,2);
     lcd.print(LCD_line3);
   }
   if (strcmp(LCD_line4, LCD_line4_prev) !=0 ) {
     lcd.setCursor(0,3);
     lcd.print(LCD_line4);
   }

   strncpy(LCD_line1_prev, LCD_line1, 20);
   strncpy(LCD_line2_prev, LCD_line2, 20);
   strncpy(LCD_line3_prev, LCD_line3, 20);
   strncpy(LCD_line4_prev, LCD_line4, 20);*/


   //Serial.println("freq. (Hz):");

}

void setup() {

  int myEraser = 7;
  TCCR1B &= ~myEraser; 
  int myPrescaler = 5;   
  TCCR1B |= myPrescaler;

  lcd.init(); 
  lcd.backlight();

  Serial.begin(115200);
  sensors.begin(); //for temp
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 

  //pinMode(FAN_PIN, OUTPUT); 
  pinMode(FAN_PIN, OUTPUT); 
  pinMode(FAN_RPM_PIN, INPUT); 
  
  //attachInterrupt(digitalPinToInterrupt(FAN_RPM_PIN), pickrpm, RISING); 
  digitalWrite(FAN_RPM_PIN,HIGH);

  //initialization of the arrays, otherwise the display stuck
  //for (int line=0; line < DISPLAY_LINES; line++) {
  sprintf(LCD_line[0], " ");
  sprintf(LCD_line[1], " ");
  sprintf(LCD_line[2], " ");
  sprintf(LCD_line[3], " ");
  
  

  //}

  /*sprintf(LCD_line1, " ");
  sprintf(LCD_line2, " ");
  sprintf(LCD_line3, " ");
  sprintf(LCD_line4, " ");*/

  timer.every(400, update_lcd);
  timer.every(1000, led_security_manager);

}

// function to print a device address, used for debug
/*void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}*/

void trigger_warning(int error) {
  // 0 fan
  // 1 temperature
}

void led_security_manager() { 

  
  float tempC = get_led_temp();
  int fanRpm =  get_fan_rpm();

  if ( (fanRpm < MIN_FAN_SPEED) || (fanRpm > MAX_FAN_SPEED) )
    trigger_warning(0);

  Serial.println(tempC);
  Serial.println(fanRpm);
  Serial.println("-----");

}

int get_fan_rpm() {

  unsigned long pulseDuration;
  pulseDuration = pulseIn(FAN_RPM_PIN, LOW);
  double frequency = 1000000/pulseDuration;

  return (int)(frequency/2*60)/2;

}

float get_led_temp() {

  // if F is needed  //Serial.println(DallasTemperature::toFahrenheit(tempC)); 
  // Arduino does not have %f in sprinf lol.
  // 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  //char str_temp[6];
  //char returnString[20];

  sensors.setResolution(insideThermometer, 9);
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(insideThermometer);
  //dtostrf(tempC, 4, 2, str_temp);
  //sprintf(returnString, "%s -",str_temp);

  return tempC;
}



void loop() {

  timer.tick();

  potVal_A = analogRead(SLIDER_A);
  potVal_B = analogRead(SLIDER_B);
  potVal_C = analogRead(SLIDER_C);

  //analogWrite(FAN_PIN,map(potVal_C, 0, 1023, 0, 255));
  analogWrite(FAN_PIN,255);

 
  sprintf(LCD_line[3], "%i, %i, %i",potVal_A,potVal_B,potVal_C);

  if  ( (abs(potVal_A-potVal_A_prev)>pot_color_threshold) || (abs(potVal_B-potVal_B_prev)>pot_color_threshold) || (abs(potVal_C-potVal_C_prev)>pot_color_threshold) ) {
    
    //Serial.println("From nobs");
    H = map(potVal_A, 0, 1023, 0, 360);
    S = map(potVal_B, 0, 1023, 0, 100);
    L = map(potVal_C, 0, 1023, 0, 100);

    potVal_A_prev = potVal_A;
    potVal_B_prev = potVal_B;
    potVal_C_prev = potVal_C;

    setHsl(H,S,L);
    
  }
  // sprintf(LCD_line3, "Fan rpm %i",(int)(frequency/2*60)/2);
  // sprintf(LCD_line4, "Fan rpm %i",(int)(frequency/2*60)/2);


  //update_lcd();
  //delay(1000);
  //Serial.println(firstPotVal);

}