//http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi
#include "ColorConverterLib.h"
#include <IRremote.h>

// Log all to Serial, comment this line to disable logging
#define LOG Serial
// Include must be placed after LOG definition to work
#include "log.h"

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

const int redPn = 9;  // the onboard LED
const int greenPn = 5;  // the onboard LED
const int bluePn = 6;  // the onboard LED

const int potRedPn = A0;
const int potGreenPn = A1;
const int potBluePn = A2;
const int potBrightPn = A3;

int potRedVal;
int potGreenVal;
int potBlueVal;

int potRedVal_prev;
int potGreenVal_prev;
int potBlueVal_prev;

int threshold_ar = 3;

double H;
double S;
double L;

double H_prev;
double S_prev;
double L_prev;

//int rgb[3];
uint8_t red;
uint8_t green;
uint8_t blue;


void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output
  pinMode(redPn, OUTPUT);  // initialize onboard LED as output
  pinMode(greenPn, OUTPUT);  // initialize onboard LED as output
  pinMode(bluePn, OUTPUT);  // initialize onboard LED as output

   
  irrecv.enableIRIn();
  irrecv.blink13(true);

  Serial.begin(9600);
  Serial.println("Initialization...");

}

void setRgb(int r, int g, int b) {

  //int redBr = map(r, 0, 255, 0, 80); // temporary needed not to burn the led
  int redBr = r;
  int greenBr =  g;
  int blueBr = b;

  //Serial.print("Final rgb: ");
  log_printf("R:%i G:%i B:%i\n", redBr, greenBr, blueBr);
  log_printf("R:%i G:%i B:%i\n", 255-redBr, 255-greenBr, 255-blueBr);

  //Serial.print(redBr);Serial.print(' ');   
  //Serial.print(greenBr);Serial.print(' '); 
  //Serial.println(blueBr);

  analogWrite(redPn, 255-redBr);
  analogWrite(greenPn, 255-greenBr);
  analogWrite(bluePn, 255-blueBr);

}

 void setHsl(double H, double S, double L) {
   ColorConverter::HslToRgb(H/360, S/100, L/100, red,green,blue);
   setRgb(red,green,blue);
 }
 


void loop() {


  potRedVal = analogRead(potRedPn);
  potGreenVal = analogRead(potGreenPn);
  potBlueVal = analogRead(potBluePn);

  if  ( (abs(potRedVal-potRedVal_prev)>threshold_ar) || (abs(potGreenVal-potGreenVal_prev)>threshold_ar) || (abs(potBlueVal-potBlueVal_prev)>threshold_ar) ) {
    
    Serial.println("From nobs");
    H = map(potRedVal, 0, 1023, 0, 360);
    S = map(potGreenVal, 0, 1023, 0, 100);
    L = map(potBlueVal, 0, 1023, 0, 100);

    potRedVal_prev = potRedVal;
    potGreenVal_prev = potGreenVal;
    potBlueVal_prev = potBlueVal;

    setHsl(H,S,L);
    
  }
  else if (irrecv.decode(&results)) {
    
    Serial.println(results.value);   

    switch (results.value) {
      case 16203967: //OFF

        if ( H != 0 || S != 0 || L != 0 ) {
          H_prev = H;
          S_prev = S;
          L_prev = L;
          H =0; S=0; L=0;
        }
      break;
      
      case 16236607: //ON
      
        if ( H==0 && S==0 && L==0 ) {
          H=H_prev; S=S_prev; L=L_prev;
        }
        
      break;
      
      case 16195807: //RED
        H=0; S=100; L=50;
      break;
      
      case 16228447: //GREEN
        H=120; S=100; L=50;
      break;
      
       case 16212127: //BLUE
        H=240; S=100; L=50;
      break;
      
      case 16244767: //WHITE
        H=0; S=0; L=100;
      break;
      
      case 16216207: //PURPLE
        H=300; S=100; L=25.1;
      break;
      
      case 16197847: //YELLOW
        H=51; S=100; L=50;
      break;
      
      default:
        // statements
      break;
    }
    
    irrecv.resume();
     
    //H = H_IR;
    //S = S_IR;
    //L = L_IR;
    
    setHsl(H,S,L);
    
    
  }

  //Serial.print("HSI: ");
  //Serial.print(H);Serial.print(' ');   
  //Serial.print(S);Serial.print(' '); 
  //Serial.println(L);
  
  //hsi2rgb(H, S, I, rgb);
  
 

  //Serial.print("Converted rgb: ");
  //Serial.print(red);Serial.print(' ');   
  //Serial.print(green);Serial.print(' '); 
  //Serial.println(blue);

  
  //setRgb(rgb[0],rgb[1],rgb[2]);



}
