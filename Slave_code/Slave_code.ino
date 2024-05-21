#include <ESP8266WiFi.h>
#include <espnow.h>

// Replace with your MAC addresses
uint8_t device_macs[][6] = {
  {0x08, 0xF9, 0xE0, 0x6B, 0x1C, 0xBB}, // Device 1
  {0x08, 0xF9, 0xE0, 0x6B, 0x2C, 0xDD}, // Device 2
  {0x08, 0xF9, 0xE0, 0x6B, 0x1F, 0xED}, // Device 3
  {0x48, 0x3F, 0xDA, 0x47, 0xE1, 0x82}, // Device 4
  {0x08, 0xF9, 0xE0, 0x6B, 0x2D, 0x08}, // Device 5
//  {0x08, 0xF9, 0xE0, 0x6B, 0x31, 0x74}, // Device 6
//  {0x08, 0xF9, 0xE0, 0x6B, 0x26, 0x3C}, // Device 7
//  {0x08, 0xF9, 0xE0, 0x6B, 0x2F, 0xFF}, // Device 8
//  {0x08, 0xF9, 0xE0, 0x6B, 0x20, 0x1C}  // Device 9
};

const int num_devices = 5; // Number of devices

// Variables to store x values from each device
float x1, x2, x3, x4, x5;//, x6, x7, x8, x9;

// Flags to indicate if data has been received from each device
bool received_flags[num_devices] = {false};

// Callback function that will be executed when data is received
void onDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len) {
  // Loop through all devices
  for (int i = 0; i < num_devices; ++i) {
    // Check if the received data is from this device
    if (memcmp(mac_addr, device_macs[i], 6) == 0) {
      // Assuming the data is float type (4 bytes each for x)
      if (data_len >= sizeof(float)) {
        float x_value;
        memcpy(&x_value, data, sizeof(float));
        
        // Store the received x value in the corresponding variable
        switch (i) {
          case 0: x1 = x_value; break;
          case 1: x2 = x_value; break;
          case 2: x3 = x_value; break;
          case 3: x4 = x_value; break;
          case 4: x5 = x_value; break;
          //case 5: x6 = x_value; break;
          //case 6: x7 = x_value; break;
          //case 7: x8 = x_value; break;
         // case 8: x9 = x_value; break;
        }

        // Set the received flag for this device
        received_flags[i] = true;

        // Print the received x value
        Serial.print("Sensor ");
        Serial.print(i + 1); // Device numbering starts from 1
        Serial.print(": ");
        Serial.println(x_value);
      }
      break; // Exit loop after processing data from this device
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function to handle received data
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(onDataRecv);
  
  // Add peers (devices)
  esp_now_add_peer(device_macs[0], ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  for (int i = 0; i < num_devices; ++i) {
    if (esp_now_add_peer(device_macs[i], ESP_NOW_ROLE_COMBO, 1, NULL, 0) != 0) {
      Serial.print("Failed to add peer ");
      Serial.println(i + 1);
    }
  }
  
  Serial.println("ESP-NOW initialized");
}

void loop() {
  // Loop through all devices and print the status of data reception
  for (int i = 0; i < num_devices; ++i) {
    if (received_flags[i]) {
      Serial.print("Sensor ");
      Serial.print(i + 1); 
      Serial.println(".");
    } else {
      Serial.print("Offline Sensor ");
      Serial.print(i + 1); // Device numbering starts from 1
      Serial.println(".");
    }
  }
  
  // Add a delay to avoid flooding the Serial monitor
  delay(5000); // 5 seconds delay
}
