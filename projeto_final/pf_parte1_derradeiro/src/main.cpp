#include <Arduino.h>

/*----------------------FreeRTOS Prep START------------------------*/
// Including FreeRTOS libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void vTask1();                   // Protótipo da função da task 1 ()
void vTask2();                   // Protótipo da função da task 2 ()
//void vTask3();                   // Protótipo da função da task 3 ()


/*-----------------------FreeRTOS Prep END-------------------------*/




/*-----------------Task 1 declarations START--------------------*/

// I2C dependencies
#include <Wire.h>
const int slaveAddress = 8;


// Function declarations:
int nob_read(int);
void leds();
void buttons_update();




// Defining pin numbers for the LEDs
#define PIN_RED    23 // GPIO23
#define PIN_GREEN  18 // GPIO18
#define PIN_BLUE   19 // GPIO19




// Defining pin numbers for the nobs (potentiometers)
#define NOB_RED    33  // GPIO33
#define NOB_GREEN  32  // GPIO3
#define NOB_BLUE   39  // GPIO39




// Defining pin numbers for the buttons (toggles)
#define TOG_RED    34 // GPIO34
#define TOG_GREEN  35 // GPIO35
#define TOG_BLUE   36 // GPIO36




#define DEBOUNCE_DELAY 200 // Setting debounce time in milliseconds




// Arrays to store debounce times and toggle states
unsigned long lastDebounceTimes[3] = {0, 0, 0};     // Stores debounce times for each button
int toggleStates[3] = {0, 0, 0};                    // Stores the current state of each button (toggle states)




// Arrays for pin numbers
int buttonPins[3] = {TOG_RED, TOG_GREEN, TOG_BLUE}; // Button pin numbers
int nobPins[3] = {NOB_RED, NOB_GREEN, NOB_BLUE};    // Nob (potentiometer) pin numbers
int ledPins[3] = {PIN_RED, PIN_GREEN, PIN_BLUE};    // LED pin numbers




// Global variables for Task1
int rgbValues[3] = {0, 0, 0};
int nobValues[3] = {0, 0, 0};
int sliderValues [3] = {0, 0, 0};




/*-----------------Task 1 declarations END-----------------------*/








//===================================================================================================<<<<








/*-----------------Task 2 declarations START---------------------*/




// Including libraries
#include <WiFi.h>
#include <HTTPClient.h>




// Function declarations:
//void wifi_turnon ();
//void wifi_turnoff ();
void wifiproblems_handler();
void send_to_sheets();




// Defining LED PIN for the WiFi visual indication.
#define On_Board_LED_PIN  2




// Defining the send_to_sheets button.
#define TOG_SEND   4 // GPIO4




// Geting the SSID and PASSWORD of the WiFi network.
const char* ssid = "jacksonhotspot";      //--> Wifi name
const char* password = "jack20wagner";    //--> Wifi password




// Google script Web_App_URL.
String Web_App_URL = "https://script.google.com/macros/s/AKfycbwSJGL05vZ2uXik5yF5dZX8mBzejE9RcapXY8oMdhuyWh-oTWsvVdN7TZlscTZAl-nP/exec";
// Test: https://script.google.com/macros/s/AKfycbwSJGL05vZ2uXik5yF5dZX8mBzejE9RcapXY8oMdhuyWh-oTWsvVdN7TZlscTZAl-nP/exec?value1=1&value2=2&value3=3&value4=4




// Global variables for Task2
int toggleS = 0;
int toggleS_last = 1;




/*-----------------Task 2 declarations END-----------------------*/








//===================================================================================================<<<<




/*-----------------------------------SETUP--------------------------------*/



void setup() {

// Criação das tasks
xTaskCreatePinnedToCore(vTask1, "TASK1", configMINIMAL_STACK_SIZE + 1024, NULL, 1, NULL, 0);
xTaskCreatePinnedToCore(vTask2, "TASK2", configMINIMAL_STACK_SIZE + 1024, NULL, 2, NULL, 1);



// Initializing the common serial, for debbuging
Serial.begin(115200);


// Setting the I2C
Wire.begin(0x8);
Wire.onReceive(receiveEvent);


// Setting LEDs as outputs
for (int i = 0; i < 3; i++) {
  pinMode(ledPins[i], OUTPUT);
}
pinMode(On_Board_LED_PIN, OUTPUT);




// Setting nobs as inputs
for (int i = 0; i < 3; i++) {
  pinMode(nobPins[i], INPUT);
}




// Setting buttons (toggles) as inputs
for (int i = 0; i < 3; i++) {
  pinMode(buttonPins[i], INPUT);
}




// Setting WiFi to STA mode and connecting
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
wifiproblems_handler();
}


/*--------------------------------LOOP MAIN--------------------------------*/


void loop() {
 
 delay(3000);
 //printf ("%d\n", toggleS);
}




//===================================================================================================<<<<

/*-----------------------------------TASK1--------------------------------*/

void vTask1() {
  while (1) { // Loop infinito para que a task continue executando    
    buttons_update();                         // Update button states
    leds();                                   // Control LEDs based on button states
    send_to_sheets();                         // Sends to Google Sheets if button state is pressed
  }
}

/*-----------------------------------TASK2--------------------------------*/

void vTask2() {
  while (1) { // Loop infinito para que a task continue executando
    i2c_update_send();
  }
}


//===================================================================================================<<<<

/*------------------------------FUNCTIONS BODIES--------------------------*/
// Function to send the values through I2C to rasp
void i2c_update_send() {

  for (int i = 0; i < 7; i++) {

    // Spam
    Wire.beginTransmission(slaveAddress);
    Wire.write(i);       
    if (i < 4) Wire.write(1);
    else Wire.write(nobValues[i + 4])               
    Wire.endTransmission();
  }
}

// Function to deal with receiving I2C data
void receiveEvent(int howMany) {

  while(Wire.available()) {
    if (howMany >= 2) {
      int identifier = Wire.read();
      int value = Wire.read();
    }
  }

  // Depending on what is received, one global variable is updated
  switch (identifier) {
    case 0: // Send button
      toggleS = !toggleS;
      break;

    case 1: // ToggleR button
      toggleStates[0] = !toggleStates[0];
      break;

    case 2: // ToggleGbutton
      toggleStates[1] = !toggleStates[1];
      break;

    case 3: // ToggleB button
      toggleStates[2] = !toggleStates[2];
      break;

     case 4: // SliderR 
      sliderValues [0] = value;
      break;

    case 5: // SliderG
      sliderValues [1] = value;
      break;

    case 6: // SliderB
      sliderValues [2] = value;
      break;
    
    default:
      break;

  }
}

// Function to update button states with debounce logic
void buttons_update() {
 // Current time
 unsigned long currentTime = millis();




 // Loop through all buttons to check debounce and update states
 for (int i = 0; i < 3; i++) {
   // Check if the button state has changed and debounce
   if (digitalRead(buttonPins[i]) && (currentTime - lastDebounceTimes[i]) > DEBOUNCE_DELAY) {
     toggleStates[i] = !toggleStates[i];   // Toggle the button state
     lastDebounceTimes[i] = currentTime;   // Register the debounce time
   }
 }

// Just some sad debbugins
int aiai = ((currentTime - lastDebounceTimes[2]) > DEBOUNCE_DELAY);
int aiaii = digitalRead(TOG_SEND);


printf ("%d -- %d\n", aiai, aiaii);


 if (digitalRead(TOG_SEND) && (currentTime - lastDebounceTimes[2]) > DEBOUNCE_DELAY) {
   toggleS = !toggleS;                     // Toggle the button state
   lastDebounceTimes[2] = currentTime;     // Register the debounce time
 }
}




// Function to read the potentiometer (nob) value and map it to 0-255 range
int nob_read(int nob) {
 int value = map(analogRead(nob), 0, 4095, 0, 255);
 return value;
}




// Function to control the LEDs based on the toggle states and nobs
void leds() {
 // Loop through all LEDs and set their brightness
 for (int i = 0; i < 3; i++) {
   // If the corresponding toggle is off, use the potentiometer value, otherwise, turn the LED off
   int value = (toggleStates[i] == 0) ? (nobValues[i] = nob_read(nobPins[i])) : (nobValues[i] = sliderValues[i]);

   // Send the pot value through i2c if their are being used
   if (toggleStates[i] == 0) {

    // Sending the toggles RGB
    Wire.beginTransmission(slaveAddress);
    Wire.write(i+1);       
    Wire.write(1);               
    Wire.endTransmission();  

    // Sending the RGB values
    Wire.beginTransmission(slaveAddress);
    Wire.write(i+4);       
    Wire.write(value);               
    Wire.endTransmission();     
   }
 
   // Stores the rgb values used (from the nobs or the sliders)
   rgbValues[i] = value;
   // Write the value to the LED pin
   analogWrite(ledPins[i], value);
 }




 delay(10); // Stabilize the values, adjust if needed
}




// Function to handle wifi problems
void wifiproblems_handler() { // Restart ESP if it doesnt connect in a set time




 int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
 connecting_process_timed_out = connecting_process_timed_out * 2;
 while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   digitalWrite(On_Board_LED_PIN, HIGH);
   delay(250);
   digitalWrite(On_Board_LED_PIN, LOW);
   delay(250);
   if (connecting_process_timed_out > 0) connecting_process_timed_out--;
   if (connecting_process_timed_out == 0) {
     delay(1000);
     ESP.restart();
   }
 }




 digitalWrite(On_Board_LED_PIN, LOW);
}




// Function to turn the WiFi on
// void wifi_turnon() {
 // Setting WiFi to STA mode and connecting
 // WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
 // wifiproblems_handler();
 // delay(100);
// }




// Function to turn the WiFi off
// void wifi_turnoff() {
 // WiFi.mode(WIFI_OFF);
 // delay(100);




// }




void send_to_sheets() {




 //wifi_turnon();
 // If WiFi is connected and the current state of toggleS is equal to toggleS_last (meaning a state
 // inversion), we send the data.
 if (WiFi.status() == WL_CONNECTED && (toggleS == toggleS_last)) {
   printf("WiFi conectado");
   digitalWrite(On_Board_LED_PIN, HIGH);




   // Create a URL for sending or writing data to Google Sheets.
   String Send_Data_URL = Web_App_URL + "?";
   Send_Data_URL += "&value1=" + String(rgbValues[0]);
   Send_Data_URL += "&value2=" + String(rgbValues[1]);
   Send_Data_URL += "&value3=" + String(rgbValues[2]);
   Send_Data_URL += "&value4=" + String(toggleStates[0]) + String(toggleStates[1]) + String(toggleStates[2]);




   //::::::::::::::::::The process of sending or writing data to Google Sheets.
     // Initialize HTTPClient as "http".
     HTTPClient http;
      // HTTP GET Request.
     http.begin(Send_Data_URL.c_str());
     http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      // Gets the HTTP status code.
     int httpCode = http.GET();
     //Serial.print("HTTP Status Code : ");
     //Serial.println(httpCode);
      // Getting response from google sheets.
     String payload;
     if (httpCode > 0) {
       payload = http.getString();
       //Serial.println("Payload : " + payload);  
     }
   
     http.end();
   //::::::::::::::::::
 
   digitalWrite(On_Board_LED_PIN, LOW);
 
   // Updating toggleS_last
   toggleS_last = !toggleS;




   //wifi_turnoff();
 }
}
