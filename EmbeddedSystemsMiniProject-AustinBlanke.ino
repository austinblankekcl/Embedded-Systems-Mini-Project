 /*
Author: Austin Blanke
LCD Pin 1 to Pico GND 
LCD Pin 2 to Pico 5V VBUS 
LCD Pin 3 to Pico GND 
LCD Pin 4 (RS) to pico GP2
LCD Pin 5 RW to GND 
LCD Pin 6 EN to Pico GP3
LCD Pin 11 (D4) to Pico GP4
LCD Pin 12 (D5) to Pico GP5
LCD Pin 13 (D6) to Pico GP6
LCD Pin 14 (D7) to Pico GP7

Keypad pin Y4        to Pico gp16
Keypad pin Y3        to Pico gp17
Keypad pin Y2        to Pico gp18
Keypad pin Y1        to Pico gp19
Keypad pin X4        to Pico gp20 
Keypad pin X3        to Pico gp21  
Keypad pin X2        to Pico gp22   
Keypad pin X1        to Pico gp26

Sensor input 1       to Pico gp14
Sensor input 2       to Pico gp15
Buzzer output        to Pico gp13
*/
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
int PinCode[4] = { 0,  5,  1,  2};           // Set password
int KeyCode[4] = {-1, -1, -1, -1};           // Initialize Input code array as all -1
bool Armed = false;                          // Initialize Boolean Armed
bool Alarm1 = false;                         // Initialize Two Sensors
bool Alarm2 = false;
int codeCount = 0;

void setup() {
  pinMode(16, INPUT_PULLUP);                 // Input from Keypad
  pinMode(17, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(20, OUTPUT);                       // Output to Keypad
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(26, OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Code:----");
  delay(3000);
}

int RdKeys (void) {
  int row,col;
  int KeyRow[4] = {20, 21, 22, 26};
  int KeyCol[4] = {16, 17, 18, 19};

  digitalWrite(KeyRow[0],1);                 // initialize all high
  digitalWrite(KeyRow[1],1);                 //
  digitalWrite(KeyRow[2],1);                 //
  digitalWrite(KeyRow[3],1);                 //

  for (row = 0; row < 4; row++) {
     digitalWrite(KeyRow[row],0);            // test row
     for (col = 0; col < 4; col++) {
       if (digitalRead(KeyCol[col]) == 0) {
         return row*4 + col;                 // return detected key press
       }
     }
     digitalWrite(KeyRow[row],1);
  }
  return -1;                                 // no key pressed
}

int ReadKeypad (void){
  int key;
  while(1) {
    key = RdKeys();
    delay(20);
    if (key == RdKeys()) {                   // read twice to eliminate 'key bounce'
      return key;                            // return debounced value
    }
  }
}

int CheckKeyCode() {
  int i;
  for (i = 0; i < 4; i++) {                  // check pincode
    if (KeyCode[i] != PinCode[i]) {
      return 0;
    }
  }
  return 1;
}

void PrintKeyCode() {
  int i;
  
  lcd.setCursor(0,1);                        // second line
  lcd.print("Code:");
  for (i = 0; i < 4; i++) {
    if (KeyCode[i] == -1) {
      lcd.print("-");
    } else {
      lcd.print("*");
    }
  }  

}

void TriggerAlarm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Report");
  digitalWrite(13, HIGH);
  delay(5000);
  digitalWrite(13, LOW);
  Alarm1 = false;
  Alarm2 = false;
}


void loop() {
  int key,k;
  int static n = 0;                          // valid entries
  int static j = 0;
  char KeyPadChar[]   = "123F456E789DA0BC";
  int  KeyPadInt [16] = {1,2,3,-1,4,5,6,-1,7,8,9,-1,-1,0,-1,-1};



      while (1) {
          lcd.setCursor(0, 0);
          if(Armed == true){lcd.print("Armed");}
          else{lcd.print("Disarmed");}


          if(digitalRead(14) == HIGH){Alarm1 = true;}     //Checking Alarms
          if(digitalRead(15) == HIGH){Alarm2 = true;}

          if((Alarm1 == true || Alarm2 == true) && Armed == true){TriggerAlarm();}  //Alarm when either sensor triggered while arm

          if((Alarm1 == true) && Armed == false){                                   // Check sensors when disarmed
            lcd.setCursor(0, 0);
            lcd.print("Check Doors");
            delay(5000);
            Alarm1 = false;
            Alarm2 = false;
            lcd.clear();
            }

          if((Alarm2 == true) && Armed == false){
            lcd.setCursor(0, 0);
            lcd.print("Check Windows");
            delay(5000);
            Alarm1 = false;
            Alarm2 = false;
            lcd.clear();
            }

          
          lcd.setCursor(0, 0);
          if(Armed == true){lcd.print("Armed");}
          else{lcd.print("Disarmed");}
      


        k = ReadKeypad();
        if (k != -1) {
          key = KeyPadInt[k];
          if (key != -1) {break;}                             // valid integer button
      }
    }

    KeyCode[j++] = key;                        // update keycode with new key entry

    lcd.clear();
    PrintKeyCode();                            // print the new keycode

    while (ReadKeypad() != -1) {               // wait for key release
    }

    if (j == 4) {                              // 4 digits entered
    while ((KeyPadChar[ReadKeypad()] != 'A' && Armed == false) || ((KeyPadChar[ReadKeypad()] != 'D' && Armed == true))) {
    }
    j = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    if (CheckKeyCode() == 1) {               // check keycode
      if(Armed == true){                     // Disarms
        Armed = false;
      }
      else{                            
        Armed = true;                       // Arms and sounds alarm
        digitalWrite(13, HIGH);
        delay(5000);
        digitalWrite(13, LOW);
      }
      codeCount = 0;
    }
    else{
      codeCount++;
      lcd.setCursor(0, 0);
      lcd.print("Incorrect Code");
      if(codeCount == 3){                   // Increment code count when false, if 3 times, sound alarm
        codeCount = 0;
        TriggerAlarm();
      }
    }
    KeyCode [0] = -1;                        // clear keycode
    KeyCode [1] = -1;
    KeyCode [2] = -1;
    KeyCode [3] = -1;
    PrintKeyCode();                          // print the cleared keycode
    }
}
