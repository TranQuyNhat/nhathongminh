#include <SoftwareSerial.h>
#include <DHT.h>
//led
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// steper
#include <Stepper.h>

// config communicate
const byte RX = 2;
const byte TX = 3;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);
String outputString = "", inputString = "";    // a String to hold incomingdata
bool stringComplete = false;                   // whether the string is complete
String outputString1 = "", inputString1 = "";  // a String to hold incomingdata
bool stringComplete1 = false;                  // whether the string is complete
long last = 0;


//led1
int led1 = 6;


// DHT11
const int DHTPIN = 5;       //Chân Out của cảm biến nối chân số 5 Arduino
const int DHTTYPE = DHT11;  // DHT 21
DHT dht(DHTPIN, DHTTYPE);   //Khai báo thư viện chân cảm biến và kiểu cảm biến
float _nhietDo = 0;
float _doAm = 0;



// mq2
#define Threshold 660
#define MQ2pin 0
float _mq2;  //variable to store sensor value

// kq037
const int soundSensor = A2;
int _kq037;


//buzz
int buzzPin = 4;


//led
LiquidCrystal_I2C lcd(0x27, 20, 4);







// steper
int buttonPin = 12;
int btnState = 2;
const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);



// light 
 int lightSs = 4;
int _lightVal;



void setup() {
  // initialize serial:
  Serial.begin(9600);
  mySerial.begin(9600);
  outputString = "A";
  outputString1 = "send to esp";




  // DHT
  dht.begin();


  // buzz
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, LOW);


  //led
  lcd.init();  // initialize the lcd
  lcd.backlight();



  //led1
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);

// light 
 pinMode(lightSs,INPUT);

}
void loop() {
  //read_UART();
  //read_UART_1();
  dht11();
  mq2();
  kq037();
  light();
  read_UART_1();
  String result = String(_nhietDo) + "-" + String(_doAm) + "-" + String(_mq2) + "-" + String(_kq037) + "-"+ String(_lightVal)+ ".0"+"-";
  lcd.setCursor(0, 0);
  Serial.println(result);
  lcd.print("Nhiet do C: ");
  lcd.print(_nhietDo);
  lcd.setCursor(0, 1);
  lcd.print("MQ2: ");
  lcd.print(_mq2);
  // if (millis() - last >= 5000) {
  //   send2ESP(result);
  //    delay(1000);
  //   last = millis();
   
  // }
  send2ESP(result);
  delay(2000);
}
void steper() {
  int buttonValue = digitalRead(buttonPin);
  if (buttonValue == LOW) {
    btnState += 1;
    delay(500);
  }
  int analogValue = analogRead(A3);
  float voltage = map(analogValue, 0, 1023, 1, 20);
  Serial.print("Toc do theo bien tro: ");
  Serial.println(voltage);
  myStepper.setSpeed(voltage);
  if (btnState % 2 == 0) {
    myStepper.step(stepsPerRevolution);
  } else {
    myStepper.step(-stepsPerRevolution);
  }
}
void dht11() {
  float doam = dht.readHumidity();  //Đọc độ ẩm
  _doAm = doam;
  float doC = dht.readTemperature();  //Đọc nhiệt độ C
  _nhietDo = doC;
  float doF = dht.readTemperature(true);  //Đọc nhiệt độ F

  // Kiểm tra cảm biến có hoạt động hay không
  if (isnan(doam) || isnan(doC) || isnan(doF)) {
    Serial.println("Không có giá trị trả về từ cảm biến DHT");
    return;
  }

  Serial.print("Độ ẩm: ");
  Serial.print(doam);

  Serial.print("%  Nhiệt độ: ");
  Serial.print(doC);

  Serial.print("°C | ");
  Serial.print(doF);
  Serial.println("°F");
}
void mq2() {
  _mq2 = analogRead(MQ2pin);  // read analog input pin 0

  Serial.print("Sensor Value mq02: ");
  Serial.print(_mq2);

  if (_mq2 > Threshold) {
    Serial.print(" | Smoke detected!");
    digitalWrite(buzzPin, HIGH);
    delay(5000);
    digitalWrite(buzzPin, LOW);
  }

  Serial.println("");
  //delay(2000);  // wait 2s for next reading
}
void read_UART() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the mainloopcan// do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    if (stringComplete == true) {
      Serial.print("Received data: ");
      Serial.println(inputString);
      inputString = "";
      stringComplete = false;
    }
  }
}
void read_UART_1() {
  while (mySerial.available()) {
    // get the new byte:
    char inChar = (char)mySerial.read();
    // add it to the inputString:
    inputString1 += inChar;
    // if the incoming character is a newline, set a flag so the mainloopcan// do something about it:

    if (inChar == '\n') {
      stringComplete1 = true;
    }
    if (stringComplete1 == true) {
      Serial.print("Received data1: ");
      Serial.print(inputString1);
      inputString1 = "";
      stringComplete1 = false;
       digitalWrite(led1, LOW);
       delay(500);
    }
  }
}
void send2ESP(String str) {
 digitalWrite(led1, HIGH);
  Serial.println("======");
  Serial.print("Send Data: ");
  Serial.println(str);
  Serial.println("======");
  mySerial.println(str);
  mySerial.flush();
}
void kq037() {
  _kq037 = analogRead(soundSensor);
  Serial.print("Sound: ");
  Serial.println(_kq037);
  if (_kq037 > 300) {
    delay(1000);
  }

  //Serial.println(_kq037);
}
void light(){
  _lightVal = digitalRead(lightSs);
  Serial.print("Value light: ");
   Serial.println(_lightVal);
}