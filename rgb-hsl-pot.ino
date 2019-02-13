//http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi
#include "ColorConverterLib.h"


const int redPn = 3;  // the onboard LED
const int greenPn = 5;  // the onboard LED
const int bluePn = 6;  // the onboard LED

const int potRedPn = A0;
const int potGreenPn = A1;
const int potBluePn = A2;
const int potBrightPn = A3;

int potRedVal;
int potGreenVal;
int potBlueVal;

double H;
double S;
double L;

//int rgb[3];
uint8_t red;
uint8_t green;
uint8_t blue;


void setup() {
  
  pinMode(redPn, OUTPUT);  // initialize onboard LED as output
  pinMode(greenPn, OUTPUT);  // initialize onboard LED as output
  pinMode(bluePn, OUTPUT);  // initialize onboard LED as output

  Serial.begin(9600);
  Serial.println("Initialization...");

}

void setRgb(int r, int g, int b) {

  int redBr = map(r, 0, 255, 0, 80); // temporary needed not to burn the led
  int greenBr =  g;
  int blueBr = b;

  Serial.print("Final rgb: ");
  Serial.print(redBr);Serial.print(' ');   
  Serial.print(greenBr);Serial.print(' '); 
  Serial.println(blueBr);

  analogWrite(redPn, redBr);
  analogWrite(greenPn, greenBr);
  analogWrite(bluePn, blueBr);

}


void loop() {

  potRedVal = analogRead(potRedPn);
  potGreenVal = analogRead(potGreenPn);
  potBlueVal = analogRead(potBluePn);

  H = map(potRedVal, 0, 1023, 0, 360);
  S = map(potGreenVal, 0, 1023, 0, 100);
  L = map(potBlueVal, 0, 1023, 0, 100);

  Serial.print("HSI: ");
  Serial.print(H);Serial.print(' ');   
  Serial.print(S);Serial.print(' '); 
  Serial.println(L);
  
  //hsi2rgb(H, S, I, rgb);
  ColorConverter::HslToRgb(H/360, S/100, L/100, red,green,blue);

  Serial.print("Converted rgb: ");
  Serial.print(red);Serial.print(' ');   
  Serial.print(green);Serial.print(' '); 
  Serial.println(blue);

  setRgb(red,green,blue);
  //setRgb(rgb[0],rgb[1],rgb[2]);



}

