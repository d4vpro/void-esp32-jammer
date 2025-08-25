#include "RF24.h"
#include <SPI.h>
#include "esp_bt.h"
#include "esp_wifi.h"

// --- Recommended pins for ESP32-C3 with NRF24L01 ---
#define CE_PIN   7     // Pin to enable the NRF24 module (Chip Enable)
#define CSN_PIN 10     // Pin for SPI selection (Chip Select Not)
#define SCK_PIN  4     // SPI Clock
#define MOSI_PIN 3     // SPI Master Out Slave In
#define MISO_PIN 2     // SPI Master In Slave Out

// --- Initialize the nRF24L01 module with the defined pins ---
RF24 transmitter(CE_PIN, CSN_PIN);

// --- Initial channel (middle value within the Bluetooth spectrum) ---
int currentChannel = 40;

void setup() {
  Serial.begin(115200);

  // --- Disable unused internal functions to free resources ---
  esp_bt_controller_deinit();  // Disables the ESP32 Bluetooth
  esp_wifi_stop();             // Stops the WiFi stack
  esp_wifi_deinit();           // Frees WiFi memory
  esp_wifi_disconnect();       // Ensures complete disconnection

  // --- Initialize the SPI bus with custom pins ---
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);

  // --- Start the NRF24 module ---
  if (transmitter.begin(&SPI)) {
    Serial.println("RF24 module initialized successfully");

    // --- Continuous transmission configuration ---
    transmitter.setAutoAck(false);                  // No acknowledgment (not needed for jamming)
    transmitter.stopListening();                    // Only transmit
    transmitter.setRetries(0, 0);                   // No retransmission attempts
    transmitter.setPALevel(RF24_PA_MAX, true);      // Maximum power (true = bypass LNA, ideal for PA+LNA)
    transmitter.setDataRate(RF24_2MBPS);            // Higher speed = more interference per channel
    transmitter.setCRCLength(RF24_CRC_DISABLED);    // Disables integrity check (not required)
    transmitter.setChannel(currentChannel);         // Sets initial channel
    transmitter.startConstCarrier(RF24_PA_MAX, currentChannel);  // Starts continuous carrier
  } else {
    Serial.println("Failed to initialize the NRF24 module");
  }
}

void loop() {
  changeRandomChannel();  // Calls the function to change the channel randomly
}

// --- Changes the transmission channel randomly to cover more Bluetooth spectrum ---
void changeRandomChannel() {
  int randomChannel = random(2, 80);           // Bluetooth operates on channels 2 to 79
  transmitter.setChannel(randomChannel);       // Changes the channel
  delayMicroseconds(random(30, 70));           // Small random delay (30–69 µs)
}
