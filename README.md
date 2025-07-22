# Arduino Auto-Ranging Multimeter

This project is a digital multimeter built using an Arduino UNO. It performs measurements of voltage, resistance, and capacitance, and includes a continuity tester. The system features auto range detection for resistance and capacitance, with a pushbutton interface to cycle between modes. Readings are displayed on a 128x64 OLED screen.

## Diagram 

<img width="3000" height="1989" alt="wiring_diagram" src="https://github.com/user-attachments/assets/7402313f-8a02-49c4-9eff-0c086ee45a05" />

## Features

- Voltage measurement (DC)
- Auto range resistance measurement with unit scaling (Ω, kΩ, MΩ)
- Auto range capacitance measurement with unit scaling (nF, µF)
- Continuity tester with buzzer, red LED and OLED visual output
- Mode selection via pushbutton
- Real time display using SSD1306 OLED

## Hardware Components

- Arduino UNO
- 128x64 OLED Display (I2C, SSD1306)
- Buzzer
- Red LED
- Pushbutton (mode selector)
- Assorted resistors 
- Breadboard and jumper wires

## Implementation Summary

The multimeter integrates multiple measurement modes into a single embedded system. The most significant challenge encountered was attempting to develop the full software stack before verifying the functionality of individual measurement modes. This resulted in significant debugging time.

The solution involved refactoring the code into modular sections and validating each mode (voltage, resistance, capacitance, continuity) independently before integration. Auto range detection was implemented for both resistance and capacitance, enabling accurate readings across a wide range of component values.

The continuity tester provides audio feedback through a buzzer and a message on the OLED display when a low-resistance connection is detected.

## Applications

This multimeter has been used for:
- Testing components and connections during Arduino prototyping
- Supporting university electronics coursework
- Verifying outputs in other personal projects

## Technical Skills Demonstrated

- Embedded systems programming (Arduino / C++)
- Analog measurement and signal conditioning
- Auto-ranging logic implementation
- OLED display interface via I2C
- Breadboard prototyping and hardware validation
- Modular embedded code design

## Future Development

- Add DC current measurement functionality
- Implement serial communication for logging measurements
- Improve measurement precision with filtering and calibration
- Design and 3D print an enclosure
- Expand continuity tester for threshold customisation
- Allow testing for smaller capacitor values
- Implement a signal generator
- Print a custom PCB to hold remove the need for breadboarding and jumper wiring
- Use an Arduino Nano instead of Arduino UNO to lower costs and footprint

## Author

**Alex Beitz**  
Bachelor of Engineering (Honours) – Electrical Engineering  
Queensland University of Technology  
[LinkedIn Profile](https://www.linkedin.com/in/alex-beitz-72501528a)  
[GitHub Profile](https://github.com/ABytes5)

