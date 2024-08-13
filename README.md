# Mandatory Helmet Detection System for Rider and Pillion

## Overview

This project is a safety system designed to ensure that both the rider and the pillion on a two-wheeler are wearing helmets before the vehicle can start. The system uses load cells to detect the presence of a rider and pillion, and VirtualWire communication to receive signals confirming helmet usage. An LED indicator is used to represent whether the vehicle can be started based on the helmet checks.

## Components and Libraries Used

- **Arduino/ESP32/ESP8266 MCU**: The microcontroller that runs the code.
- **HX711 Load Cell Amplifier**: Used to measure weight from load cells.
- **Load Cell**: Detects the presence of a rider and pillion by measuring weight.
- **LiquidCrystal_I2C**: Controls a 16x2 LCD display over I2C to show weight and detection status.
- **VirtualWire**: Handles wireless communication between the helmet and the MCU.
- **EEPROM**: Stores and retrieves calibration data for the load cell.
- **LED**: Indicates whether the vehicle can be started.

## Main Code (`main.ino`)

### Functionality

1. **Load Cell Setup**:
   - Initializes the HX711 load cell amplifier and starts the load cell to detect the rider's weight.
   - A calibration value is set, which can optionally be retrieved from EEPROM.

2. **LCD Setup**:
   - A 16x2 LCD is initialized to display information such as the detected weight and whether a pillion is detected.

3. **VirtualWire Setup**:
   - VirtualWire is configured for communication, and the system starts listening for signals from helmet units.

4. **Main Loop**:
   - **Weight Detection**: The system continuously checks the load cell for weight data. If a weight greater than 0 is detected, it checks if the weight exceeds 80 kg to determine if a pillion is present.
   - **LCD Display**: The weight and pillion status are displayed on the LCD.
   - **VirtualWire Signal Handling**: The system listens for signals (`'a'`, `'b'`, `'c'`, `'d'`) indicating helmet status.
     - Signal `'a'` means the rider’s helmet is worn.
     - Signal `'b'` means the rider’s helmet is removed.
     - Signal `'c'` means the pillion’s helmet is worn.
     - Signal `'d'` means the pillion’s helmet is removed.
   - **LED Control**: The LED is turned on only if:
     - A pillion is detected, and both the rider and pillion helmets are worn.
     - No pillion is detected, but the rider's helmet is worn.

5. **Serial Monitor**: Optional output to the serial monitor for debugging purposes.

### Pin Configuration

- **HX711_dout**: Connected to pin 4 of the MCU.
- **HX711_sck**: Connected to pin 5 of the MCU.
- **LCD I2C**: Default address `0x27`, connected via I2C.
- **VirtualWire RX Pin**: Signal pin connected to pin 11.
- **LED Pin**: Connected to pin 6.

## TX-1 and TX-2 Receiver Codes

### Functionality

These codes represent the receiver units for the helmet detection signals. Each helmet is associated with a transmitter that sends a specific signal to the MCU when worn.

- **TX-1 Receiver**:
  - **Signal `'a'`**: Indicates that the rider's helmet is being worn; turns the LED on.
  - **Signal `'b'`**: Indicates that the rider's helmet is removed; turns the LED off.

- **TX-2 Receiver**:
  - **Signal `'c'`**: Indicates that the pillion's helmet is being worn; turns the LED on.
  - **Signal `'d'`**: Indicates that the pillion's helmet is removed; turns the LED off.

### Pin Configuration

- **VirtualWire RX Pin**: Signal pin connected to pin 11.
- **LED Pin**: Connected to pin 6.

## How It Works

1. **Setup**:
   - Load cells measure the rider's and pillion's weight.
   - The system listens for helmet signals.
   
2. **Helmet Detection**:
   - Signals from helmet transmitters indicate whether the helmets are being worn.

3. **Conditional Operation**:
   - The system checks both the weight and the signals to decide whether the vehicle can be started (indicated by the LED).

4. **Output**:
   - The results are displayed on an LCD, and an LED indicates if it's safe to start the vehicle.

## Usage Instructions

1. **Compile and Upload**: Upload the main code to your MCU.
2. **Connect Components**: Connect the HX711, load cells, LCD, and VirtualWire module as per the pin configuration.
3. **Run the System**: Once powered, the system will start checking the rider and pillion status. The vehicle's LED will turn on only if the conditions for helmet detection are met.