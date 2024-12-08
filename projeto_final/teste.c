#include <Wire.h>

#define I2C_SLAVE_ADDRESS 0x08

// States of the 4 boolean variables
bool states[4] = {false, false, false, false};

// Pins for the physical buttons
const int buttonPins[4] = {2, 4, 5, 18}; // Adjust pins as needed

// Function to send updated states to the Raspberry Pi
void sendStates() {
    Wire.write((uint8_t*)states, 4);
    Serial.printf("Sent states: %d %d %d %d\n", states[0], states[1], states[2], states[3]);
}

// Callback for I2C data reception
void onReceive(int numBytes) {
    if (numBytes == 4) {
        for (int i = 0; i < 4; i++) {
            states[i] = Wire.read();
        }
        Serial.printf("Received states: %d %d %d %d\n", states[0], states[1], states[2], states[3]);
    }
}

// Check physical buttons for toggles
void checkButtons() {
    for (int i = 0; i < 4; i++) {
        if (digitalRead(buttonPins[i]) == LOW) { // Assuming active LOW buttons
            delay(50); // Debounce
            if (digitalRead(buttonPins[i]) == LOW) {
                states[i] = !states[i];
                sendStates(); // Send updated states to Raspberry Pi
                while (digitalRead(buttonPins[i]) == LOW) {
                    delay(10); // Wait until button is released
                }
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(I2C_SLAVE_ADDRESS);
    Wire.onReceive(onReceive);

    // Configure button pins as inputs with pull-up resistors
    for (int i = 0; i < 4; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
    Serial.println("ESP32 I2C Slave Initialized");
}

void loop() {
    checkButtons(); // Continuously check for button presses
    delay(100); // Small delay for efficiency
}
