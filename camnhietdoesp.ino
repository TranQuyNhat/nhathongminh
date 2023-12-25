// #include <SoftwareSerial.h>
// #ifdef ESP32
// #include <WiFi.h>
// #include <HTTPClient.h>
// #else
// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #endif

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// Replace with your network credentials
const char* ssid = "WIFI SINH VIEN";
const char* password = "";
const byte RX = 14;
const byte TX = 12;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);
String outputString = "", inputString = "";  // a String to hold incomingdata
bool stringComplete = false;                 // whether the string is complete
String inputString1 = "";                    // a String to hold incoming data
bool stringComplete1 = false;                // whether the string is complete
// http
HTTPClient http;  //Declare an object of class HTTPClient
WiFiClient client;


long last = 0;
void setup() {
  // put your setup code here, to run once:
  // initialize serial:
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  mySerial.begin(9600);
  while (!mySerial) {
    Serial.print("my serial error");
  }
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    delay(250);
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  last = millis();
  outputString = "send to uno";
}
void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  //read_UART();
  read_UART_1();
  //send2UNO(outputString);
}
// this method makes a HTTP connection to the server:
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
      //Serial.print("Da nhan UNO: ");
      //Serial.println(inputString);
      Serial.print("Received read_UART: ");
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
      //Serial.print("Da nhan UNO: ");
      //Serial.println(inputString);
      Serial.print("Received read_UART_1: ");
      Serial.print(inputString1);
      send2Api(inputString1);
      inputString1 = "";
      stringComplete1 = false;
    }
  }
}
void send2UNO(String str) {
  Serial.print("Send Data: ");
  Serial.println(str);
  mySerial.println(str);
  mySerial.flush();
}
void send2Api(String str) {
  Serial.print("Send: ");
  Serial.println(str);
  String name = str;
  String array[100];
  int r = 0, t = 0;

  for (int i = 0; i < name.length(); i++) {
    if (name[i] == '-') {
      if (i - r > 1) {
        array[t] = name.substring(r, i);
        // Serial.println("ARRAY: ");
        // Serial.println(array[t]);
        t++;
      }
      r = (i + 1);
    }
  }

  // for (int k = 0; k <= t; k++) {
  //   Serial.println(array[k]);
  // }
  if (WiFi.status() == WL_CONNECTED) {  //Check WiFi connection status
    String url = "http://dweet.io/dweet/for/dnu_cntt1502_nhom10?";
    url.concat("NhietDo=");
    url.concat(array[0]);
    url.concat("&DoAm=");
    url.concat(array[1]);
    url.concat("&Gas=");
    url.concat(array[2]);
    url.concat("&AmThanh=");
    url.concat(array[3]);
    url.concat("&AnhSang=");
    url.concat(array[4]);
    Serial.println(url);
    http.begin(client, url);    //Specify request destination
    int httpCode = http.GET();  //Send the request
    String payload = http.getString();
    if (httpCode > 0) {         //Check the returning code
      Serial.println(payload);  //Print the response payload
    } else {
      // Bad Response Code
      //String errorMessage = "Error response (" + String(httpCode) + "): " + payload;
      Serial.print("Error: ");
      Serial.println(String(httpCode));
      Serial.println(String(payload));
      return;
    }
    http.end();  //Close connection
  }

 // delay(5000);  //Send a request every 30 seconds
}