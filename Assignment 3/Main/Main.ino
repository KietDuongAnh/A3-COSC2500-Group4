
// Include the Library
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Password.h>
#include <Servo.h>


Password password = Password("12345");
int password_pos = 11;  // the postition of the password input


// Constant for row and column
const byte ROWS = 4;
const byte COLS = 3;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'},
};

// Keypad connection to Arduino
byte rowPins[ROWS] = {29, 28, 27, 26};  //connect to the row pinouts of the keypad
byte colPins[ROWS] = {30, 31, 32};      //connect to the column pinouts of the keypad

// Create Keypad Object
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


// Create LCD Object
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo
Servo servo;    // create servo object to control servo
int servoPos = 90;   // variable to store the servo position

// Constrains for LEDs, inputs and outputs
int redLEDPin = 22;
int greenLEDPin = 23;
int blueLEDPin = 25;

int pirSensor1 = A15;
int pirSensor2 = A14;
int buzzer = 24;


// Variable
int alarmActive = 0;
int zone = 0;
int alarmStatus = 0;


void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);
  
  // Output 
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Input
  pinMode(pirSensor1, INPUT);
  pinMode(pirSensor2, INPUT);

  // set variable
  digitalWrite(redLEDPin, HIGH);    // set the red LED turn on as alarmActive is 0
  digitalWrite(greenLEDPin, LOW);   // set the red LED turn on as alarmActive is 1
  digitalWrite(blueLEDPin, LOW);    // set the red LED turn on as alarmStatus is 0
  digitalWrite(buzzer, LOW); 

  servo.attach(52);   // attaches the servo on pin 52 to the servo object

  // Setup LCD with backlight and initialize
  lcd.init();
  lcd.backlight();
  displayEntryScreen(); // display screen
  
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad 
}

void loop()
{
  keypad.getKey();

  if (alarmActive == 1){
    if (digitalRead(pirSensor1) == HIGH){
      zone = 1;
      alarmTriggered();
    }
    if (digitalRead(pirSensor1) == HIGH){
      zone = 2;
      alarmTriggered();
    }
  }
}


void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()){
    case PRESSED:
      if (password_pos - 11 >= 6){
          return;
      }
      lcd.setCursor((password_pos++),1);
//      Serial.print(eKey);
      switch (eKey){
        case '#':                             // '#' to validate password
          password_pos = 11;
          checkPassword(); 
          break;
        case '*':                             // '*' to reset password
          password.reset();
          password_pos = 11;
          lcd.print("     ");                 // reset password input in LCD
          break;
        default: 
          password.append(eKey);
          lcd.print('-');
      }  
  }
}

void checkPassword(){
  if (password.evaluate()) {
    if(alarmActive == 0 && alarmStatus == 0)
    {
      activate();
//        lcd.clear();
//        lcd.print("ACTIVATE");
    } 
    else if( alarmActive == 1 || alarmStatus == 1) {
      deactivate();
//        lcd.clear();
//        lcd.print("DEACTIVATE");
    }
  }
  else {
    invalidPIN();
  }
}

void invalidPIN(){
    password.reset();
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("INVALID CODE!");
    lcd.setCursor(1,1);
    lcd.print("TRY AGAIN!");
    for (int i=0; i<3; i++){
      digitalWrite(redLEDPin, LOW);
      delay(500);
      digitalWrite(redLEDPin, HIGH);
      delay(500);
    }
    displayEntryScreen();
}


void activate(){
    alarmActive = 1;
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SYS ACTIVE!");
    password.reset();
    delay(2000);
    displayEntryScreen();
}
  
void deactivate() {
    alarmActive = 0;
    alarmStatus = 0;
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(buzzer, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SYS DEACTIVE!");
    password.reset();
    delay(4000);
    displayEntryScreen();
}

void alarmTriggered(){
  int expected_pos;
  int increased;


  password.reset();
  alarmStatus = 1;
 
  
  lcd.clear();
  lcd.setCursor(0,0);
  if (zone = 1){
    expected_pos = 45;
    lcd.print("Motion in P1  ");
    delay(1000);
  }
  else if (zone = 2){
    expected_pos = 80;
    lcd.print("Motion in P2  ");
    delay(1000);  
  }
    delay(500); 
  // set codition if pos < expectedd pos
  if (expected_pos > servoPos) {
    increased = 1;
  } else {
    increased = -1;
  }
  
  for (servoPos = servoPos; servoPos != expected_pos; servoPos += increased) {
    servo.write(servoPos);                  // tell servo to go to position in variable 'pos' 
    delay(15);                           // waits 15ms for the servo to reach the position 
  }
  
  digitalWrite(buzzer, HIGH);
  warningLight();
  delay (2000);
}

void displayEntryScreen() {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Enter PIN:");
}




void warningLight(){
    for (int k = 0; k < 5; k++) {
      digitalWrite(blueLEDPin, HIGH);
      delay(400);
      digitalWrite(blueLEDPin, LOW);
      delay(400);  
    }
}
