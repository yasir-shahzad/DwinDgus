/**
 * @file DwinDgus.h
 * @author Yasir Shahzad (yasirshahzad918@gmail.com)
 * @brief Professional DWIN DGUS Display Library for Arduino/ESP32
 * @date 2025-06-13
 *
 * This library provides a modern, object-oriented interface for DWIN DGUS displays.
 * Inspired by best practices from open-source community libraries.
 */

#ifndef DWIN_DGUS_H
#define DWIN_DGUS_H

#include <Arduino.h>

/**
 * @brief Main class for DWIN DGUS display control.
 */
class DwinDgus {
public:
    /**
     * @brief Construct a new DwinDgus object.
     * @param serial Reference to the hardware or software serial port.
     * @param baud Baud rate for communication (default: 115200).
     */
    DwinDgus(Stream& serial, uint32_t baud = 115200);

    /**
     * @brief Initialize the display communication.
     */
    void begin();

    /**
     * @brief Send data to the display.
     * @param data Data buffer.
     * @param len Length of data.
     */
    void sendData(const uint8_t* data, size_t len);

    /**
     * @brief Read data from the display.
     * @param buffer Buffer to store received data.
     * @param maxLen Maximum length to read.
     * @return size_t Number of bytes read.
     */
    size_t readData(uint8_t* buffer, size_t maxLen);

    /**
     * @brief Set a variable/register on the display.
     * @param address Register address.
     * @param value Value to set.
     */
    void setRegister(uint16_t address, uint16_t value);

    /**
     * @brief Get a variable/register value from the display.
     * @param address Register address.
     * @return uint16_t Value read.
     */
    uint16_t getRegister(uint16_t address);

    /**
     * @brief Show a message on the display.
     * @param page Page number.
     * @param message Message string.
     */
    void showMessage(uint8_t page, const String& message);

    /**
     * @brief Reset the display.
     */
    void reset();

    /**
     * @brief Put the display to sleep.
     */
    void sleep();

    /**
     * @brief Wake up the display.
     */
    void wake();

private:
    Stream& _serial;
    uint32_t _baud;
    void sendCommand(const uint8_t* cmd, size_t len);
    void write16(uint16_t value);
};

#endif // DWIN_DGUS_H
