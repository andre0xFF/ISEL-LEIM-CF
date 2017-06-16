#include <Wire.h>

#define RS 0x01
#define RW 0x02
#define EN 0x04
#define LIGHT 0x08
#define LCD_ADDRESS 0x3F

#define SENSOR_TRIGGER_PIN 4
#define SENSOR_ECHO_PIN 2
long sensor_trigger;
boolean sensor_echo = false;

#define INTERRUPT_UP 0
#define INTERRUPT_DOWN 1
int interrupt_state = INTERRUPT_DOWN;

// int button_state
// #define WAITING 0
// #define PRESSED 1

void display_write_data_4(byte data) {
    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | RS);
    Wire.endTransmission();

    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | RS | EN);
    Wire.endTransmission();

    delayMicroseconds(1);

    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | RS);
    Wire.endTransmission();

    delayMicroseconds(100);
}

void display_write_command_4(byte data) {
    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT);
    Wire.endTransmission();

    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | EN);
    Wire.endTransmission();

    delayMicroseconds(1);

    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT);
    Wire.endTransmission();

    delayMicroseconds(100);
}

void display_write_command_8(byte data) {
    display_write_command_4(data >> 8);
    display_write_command_4(data);
}

void display_write_data_8(byte data) {
    display_write_data_4(data >> 4);
    display_write_data_4(data);
}

void display_initialize() {
    Wire.begin();
    // delay(50);

    display_write_command_4(0x03);
    // delay(5);
    display_write_command_4(0x03);

    // delayMicroseconds(200);

    display_write_command_4(0x03);
    display_write_command_4(0x02);
    display_write_command_4(0x02);
    display_write_command_4(0x08);
 
    // delayMicroseconds(120);
 
    display_write_command_4(0x00);
    display_write_command_4(0x0F);
 
    // delayMicroseconds(120);
 
    display_write_command_4(0x08);
    display_write_command_4(0x00);
 
    // delayMicroseconds(120);
}

void display_clear() {
    display_write_command_8(0x01);
}

void display_set_cursor(byte line, bute column) {
    display_write_command_8(0x80 | line << 6 | column);
}

void display_print_char(char data) {
    display_write_command_8(data);
}

void display_print_string(String data) {
    for (int i = 0; i = data.length(); i++) {
        display_print_char(data[i])
    }
}

void display_print_int(int data) {
    String s = String(data);
    display_print_string(s);
}

void sensor_generate_trigger() {
    sensor_trigger = millis();
    digitalWrite(SENSOR_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SENSOR_TRIGGER_PIN, LOW);
}

void sensor_interrupt() {
    if (interrupt_state == INTERRUPT_UP) {

        return;
    }

    if (interrupt_state == INTERRUPT_DOWN) {

        return;
    }
}
