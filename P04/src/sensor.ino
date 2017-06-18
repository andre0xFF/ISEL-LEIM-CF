#include <Wire.h>

// Addresses
#define RS 0x01
#define RW 0x02
#define EN 0x04
#define LIGHT 0x08
#define LCD_ADDRESS 0x3F

// Pins
#define SENSOR_TRIGGER_PIN 4
#define SENSOR_ECHO_PIN 2
#define BUTTON_PIN 7

// Autonomous systems states
// Multiple autonomous systems may use WAITING state
#define WAITING 0
#define SENSOR_ECHO_UP 0
#define SENSOR_ECHO_DOWN 1
#define MEASURE_DISTANCE_TRIGGER 1
#define BUTTON_PRESSED 1

int sensor_echo_state = SENSOR_ECHO_DOWN;
int measure_distance_state = MEASURE_DISTANCE_TRIGGER;
int button_state = WAITING;

// Variables
#define SENSOR_TRIGGER_GAP 60
#define BUTTON_DEBOUNCE_TIME 20

boolean sensor_echo;
long sensor_trigger;
long sensor_echo_time;
long button_time;
int measured_distance;

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

void display_set_cursor(byte line, byte column) {
    display_write_command_8(0x80 | line << 6 | column);
}

void display_print_char(char data) {
    display_write_command_8(data);
}

void display_print_string(String data) {
    for (int i = 0; i < data.length(); i++) {
        display_print_char(data[i]);
    }
}

void display_print_int(int data) {
    String s = String(data);
    display_print_string(s);
}

void print_distance(double distance) {
    // TODO
    if (distance >= 2 && distance <= 400) {

        return;
    }

    if (distance < 2) {

        return;
    }
}

boolean sensor_generate_trigger() {
    if (millis() - sensor_trigger < SENSOR_TRIGGER_GAP) {
        return false;
    }

    sensor_echo = false;
    sensor_trigger = millis();
    digitalWrite(SENSOR_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SENSOR_TRIGGER_PIN, LOW);
    return true;
}

void sensor_interrupt() {
    if (sensor_echo_state == SENSOR_ECHO_UP) {
        sensor_echo_time = micros();
        attachInterrupt(digitalPinToInterrupt(SENSOR_ECHO_PIN), sensor_interrupt, FALLING);
        sensor_echo_state = SENSOR_ECHO_DOWN;
        return;
    }

    if (sensor_echo_state == SENSOR_ECHO_DOWN) {
        sensor_echo = true;
        sensor_echo_time = micros() - sensor_echo_time;
        attachInterrupt(digitalPinToInterrupt(SENSOR_ECHO_PIN), sensor_interrupt, RISING);
        sensor_echo_state = SENSOR_ECHO_UP;
        return;
    }
}

void measure_distance() {
    if (measure_distance_state == WAITING) {
        if (!sensor_echo) {
            display_clear();
            measure_distance_state = MEASURE_DISTANCE_TRIGGER;
            return;
        }

        measured_distance = sensor_echo_time / 58;
        measure_distance_state = MEASURE_DISTANCE_TRIGGER;
        print_distance(measured_distance);
        return;
    }

    if (measure_distance_state == MEASURE_DISTANCE_TRIGGER) {
        boolean trigger = sensor_generate_trigger();

        measure_distance_state = trigger ? WAITING : MEASURE_DISTANCE_TRIGGER;
        return;
    }
}

void button() {
    boolean pressed = digitalRead(BUTTON_PIN);
    long current_time = millis();

    if (button_state == WAITING && pressed) {
        if (current_time - button_time < BUTTON_DEBOUNCE_TIME) {
            button_time = millis();
            return;
        }

        button_state = BUTTON_PRESSED;
    }

    if (button_state == BUTTON_PRESSED && !pressed) {
        if (current_time - button_time < BUTTON_DEBOUNCE_TIME) {
            button_time = millis();
            return;
        }

        button_state = WAITING;
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(SENSOR_TRIGGER_PIN, OUTPUT);
    pinMode(SENSOR_ECHO_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(SENSOR_ECHO_PIN), sensor_interrupt, RISING);
}

void loop() {
    if (Serial.available()) {
        // TODO
    }
    measure_distance();
    button();
}
