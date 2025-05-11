#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define DEVICES_PER_BRAND 15
#define ADVERTISING_INTERVAL 20

#pragma pack(push, 1)
struct BleDevice {
    const char name[28];
    uint8_t payload[31];
    uint8_t size;
};
#pragma pack(pop)

// Apple
const BleDevice apple[] = {
    {"AirPods Pro", {0x1e,0xff,0x4c,0x00,0x07,0x19,0x07,0x0e}, 31},
    // +14 других устройств Apple
};

// Samsung
const BleDevice samsung[] = {
    {"Galaxy S24 Ultra", {0x1e,0xff,0x75,0x01,0x42,0x1c,0x11,0x03}, 31},
    // +14 устройств Samsung
};

// Xiaomi
const BleDevice xiaomi[] = {
    {"Mi Band 8 Pro", {0x1e,0xff,0xdd,0x01,0x11,0x16,0x11,0x01}, 31},
    // +14 Xiaomi
};

// Huawei
const BleDevice huawei[] = {
    {"Mate 60 Pro", {0x1e,0xff,0x57,0x01,0x32,0x1a,0x11,0x01}, 31},
    // +14 Huawei
};

// Honor
const BleDevice honor[] = {
    {"Honor Magic 6", {0x1e,0xff,0x58,0x01,0x33,0x1b,0x11,0x01}, 31},
    // +14 Honor
};

// Realme
const BleDevice realme[] = {
    {"Realme GT 5 Pro", {0x1e,0xff,0x7a,0x01,0x22,0x18,0x11,0x01}, 31},
    // +14 Realme
};

// Tecno
const BleDevice tecno[] = {
    {"Tecno Phantom V", {0x1e,0xff,0x7b,0x01,0x23,0x19,0x11,0x01}, 31},
    // +14 Tecno
};

// Google
const BleDevice google[] = {
    {"Pixel 8 Pro", {0x1e,0xff,0x47,0x01,0x24,0x1d,0x11,0x01}, 31},
    // +14 Google
};

// OPPO
const BleDevice oppo[] = {
    {"OPPO Find X7", {0x1e,0xff,0x4f,0x01,0x25,0x1e,0x11,0x01}, 31},
    // +14 OPPO
};

// VIVO
const BleDevice vivo[] = {
    {"Vivo X100 Pro", {0x1e,0xff,0x56,0x01,0x26,0x1f,0x11,0x01}, 31},
    // +14 Vivo
};

// Infinix
const BleDevice infinix[] = {
    {"Infinix Zero 30", {0x1e,0xff,0x49,0x01,0x27,0x20,0x11,0x01}, 31},
    // +14 Infinix
};

const BleDevice* brands[] = {
    apple, samsung, xiaomi, huawei, 
    honor, realme, tecno, google, 
    oppo, vivo, infinix
};

const char* brandNames[] = {
    "Apple", "Samsung", "Xiaomi", "Huawei",
    "Honor", "Realme", "Tecno", "Google",
    "OPPO", "VIVO", "Infinix"
};

const uint8_t brandCount = sizeof(brands)/sizeof(brands[0]);

uint8_t currentBrand = 0;
uint16_t currentDevice = 0;
BLEAdvertising* pAdvertising;

void setup() {
    Serial.begin(115200);
    
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bt_controller_set_ble_scan_duplicate_mode(ESP_BLE_SCAN_DUPLICATE_DISABLE);
    
    BLEDevice::init("");
    BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_PWR_LVL_P9);
    pAdvertising = BLEDevice::getAdvertising();
    
    pAdvertising->setMinInterval(ADVERTISING_INTERVAL);
    pAdvertising->setMaxInterval(ADVERTISING_INTERVAL);
    pAdvertising->setMinPreferred(ADVERTISING_INTERVAL);
    pAdvertising->setMaxPreferred(ADVERTISING_INTERVAL);
}

void advertiseNextDevice() {
    const BleDevice* d = &brands[currentBrand][currentDevice];
    
    BLEAdvertisementData advData;
    advData.setFlags(0x06);
    advData.addData(d->payload, d->size);
    
    pAdvertising->setAdvertisementData(advData);
    pAdvertising->start();
    
    if(++currentDevice >= DEVICES_PER_BRAND) {
        currentDevice = 0;
        currentBrand = (currentBrand + 1) % brandCount;
    }
}

void loop() {
    static uint32_t lastSwitch = 0;
    
    if(micros() - lastSwitch >= 50000) {
        advertiseNextDevice();
        lastSwitch = micros();
        
        Serial.printf("[%s] %s\n", 
            brandNames[currentBrand],
            brands[currentBrand][currentDevice].name);
    }
    
    vTaskDelay(1);
}
