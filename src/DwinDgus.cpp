#include "DwinDgus.h"

DwinDgus::DwinDgus(Stream& serial, uint32_t baud)
    : _serial(serial), _baud(baud) {}

void DwinDgus::begin() {
    // If using HardwareSerial, set baud rate
    // User must call Serial.begin() before passing to this class
}

void DwinDgus::sendData(const uint8_t* data, size_t len) {
    _serial.write(data, len);
    _serial.flush();
}

size_t DwinDgus::readData(uint8_t* buffer, size_t maxLen) {
    size_t count = 0;
    unsigned long start = millis();
    while (_serial.available() && count < maxLen && (millis() - start < 100)) {
        buffer[count++] = _serial.read();
    }
    return count;
}

void DwinDgus::setRegister(uint16_t address, uint16_t value) {
    uint8_t cmd[7] = {0x5A, 0xA5, 0x05, 0x82, (uint8_t)(address >> 8), (uint8_t)(address & 0xFF), (uint8_t)(value & 0xFF)};
    sendCommand(cmd, sizeof(cmd));
}

uint16_t DwinDgus::getRegister(uint16_t address) {
    uint8_t cmd[6] = {0x5A, 0xA5, 0x04, 0x83, (uint8_t)(address >> 8), (uint8_t)(address & 0xFF)};
    sendCommand(cmd, sizeof(cmd));
    delay(10);
    uint8_t resp[8] = {0};
    size_t len = readData(resp, sizeof(resp));
    if (len >= 7) {
        return (resp[5] << 8) | resp[6];
    }
    return 0;
}

void DwinDgus::showMessage(uint8_t page, const String& message) {
    // Example: send a string to a specific page (user must adapt address)
    uint16_t address = 0x2000 + page * 0x10; // Example address calculation
    uint8_t cmd[32] = {0x5A, 0xA5, (uint8_t)(message.length() + 4), 0x82, (uint8_t)(address >> 8), (uint8_t)(address & 0xFF)};
    for (size_t i = 0; i < message.length(); ++i) {
        cmd[6 + i] = message[i];
    }
    sendCommand(cmd, 6 + message.length());
}

void DwinDgus::reset() {
    uint8_t cmd[7] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x04, 0x01};
    sendCommand(cmd, sizeof(cmd));
}

void DwinDgus::sleep() {
    uint8_t cmd[7] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x82, 0x01};
    sendCommand(cmd, sizeof(cmd));
}

void DwinDgus::wake() {
    uint8_t cmd[7] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x82, 0x00};
    sendCommand(cmd, sizeof(cmd));
}

void DwinDgus::sendCommand(const uint8_t* cmd, size_t len) {
    _serial.write(cmd, len);
    _serial.flush();
}

void DwinDgus::write16(uint16_t value) {
    _serial.write((uint8_t)(value >> 8));
    _serial.write((uint8_t)(value & 0xFF));
}
