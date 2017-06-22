#include <Wire.h>

// Addresses
#define RS 0x01
#define RW 0x02
#define EN 0x04
#define LIGHT 0x08
#define LCD_ADDRESS 0x3F

// Pins
#define PIN_BUTTON 7

// States
#define STATE_WAITING 0
#define STATE_GENERATE_TRIGGER 1
#define STATE_CALCULATE_DISTANCE 2
#define STATE_BUTTON_CLICKED 1

int sensor = STATE_GENERATE_TRIGGER;
int button = STATE_WAITING;

// Properties
#define PIN_SENSOR_ECHO 2
#define PIN_SENSOR_TRIGGER 4
#define SENSOR_TRIGGER_GAP 60
#define SENSOR_TRIGGER_DELAY 10
#define SENSOR_ECHO_LIMIT 38
#define BUTTON_PRESSING_INTERVAL 20

unsigned long sensor_trigger_timer;
volatile boolean sensor_echo;
volatile unsigned long sensor_echo_initial_time;
volatile unsigned long sensor_echo_final_time;

double object_distance[] = {-1, -1};
long object_time[] = {-1, -1};
float object_velocity;

unsigned long button_timer;
boolean button_status;

void setup() {
    Serial.begin(9600);
    // Sensor
    pinMode(PIN_SENSOR_TRIGGER, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_ECHO), on_echo_released, RISING);
    interrupts();
    // LCD
    display_initialize();
    display_clear();
}

void loop() {
    sensor_state_machine();
    button_state_machine();
    read_input();
}

void read_input() {
    if (Serial.available()) {
        if (Serial.read() == 'R') {
            button = STATE_BUTTON_CLICKED;
        }
    }
}

/*
 * Sensor
 */
void sensor_state_machine() {
    if (sensor == STATE_GENERATE_TRIGGER && button == STATE_BUTTON_CLICKED) {
        boolean trigger = generate_trigger();

        if (trigger) {
            sensor = STATE_WAITING;
        }

        return;
    }

    if (sensor == STATE_WAITING) {
        long timer = millis() - sensor_trigger_timer;

        if (timer > SENSOR_TRIGGER_GAP && !sensor_echo) {
            on_echo_lost();
            sensor = STATE_GENERATE_TRIGGER;
            return;
        }

        if (sensor_echo) {
            sensor_process();
            sensor = STATE_GENERATE_TRIGGER;
            return;
        }
    }
}

boolean generate_trigger() {
    long trigger_delta = millis() - sensor_trigger_timer;

    if (trigger_delta < SENSOR_TRIGGER_GAP) {
        return false;
    }

    sensor_trigger_timer = millis();

    digitalWrite(PIN_SENSOR_TRIGGER, HIGH);
    delayMicroseconds(SENSOR_TRIGGER_DELAY);
    digitalWrite(PIN_SENSOR_TRIGGER, LOW);

    return true;
}

void on_echo_released() {
    sensor_echo = false;
    sensor_echo_initial_time = micros();
    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_ECHO), on_echo_received, FALLING);
}

void on_echo_received() {
    sensor_echo = true;
    sensor_echo_final_time = micros();
    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_ECHO), on_echo_released, RISING);
}

void on_echo_lost() {
    object_time[0] = -1;
    object_time[1] = -1;
    object_distance[0] = -1;
    object_distance[0] = -1;
    Serial.println("Echo lost.");
    display_print_string("echo lost");
}

void sensor_process() {
    unsigned long delta = sensor_echo_final_time - sensor_echo_initial_time;
    float distance = delta / 58;

    if (distance < 2 || distance > 400) {
        return;
    }

    object_distance[0] = object_distance[1];
    object_time[0] = object_time[1];

    object_distance[1] = distance;
    object_time[1] = sensor_echo_final_time;

    if (object_distance[0] == -1 || object_time[0] == -1) {
        return;
    }

    float delta_distance = fabs(object_distance[1] - object_distance[0]);
    float delta_time = abs(object_time[1] - object_time[0]);

    object_velocity = delta_distance / delta_time;
    object_velocity *= pow(10, 4);

    // if (object_velocity == 0) {
    //     return;
    // }

    // Serial.print(object_velocity);
    // Serial.println(" m/s");
}

/*
 * Button
 */
void button_state_machine() {
    boolean button_status_previous = button_status;
    button_status = digitalRead(PIN_BUTTON);

    if (button == STATE_WAITING) {
        on_button_waiting();
        long timer = millis() - button_timer;

        if (!button_status_previous && button_status && timer > BUTTON_PRESSING_INTERVAL) {
            button = STATE_BUTTON_CLICKED;
        }
        return;
    }

    if (button == STATE_BUTTON_CLICKED) {
        on_button_clicked();

        if (true) {
            button = STATE_WAITING;
        }
        return;
    }
}

void on_button_waiting() {

}

void on_button_clicked() {
    display_print_string("Speed (m/s):" + String(object_velocity)); 
    Serial.println(String(object_velocity)+" m/s");
}

/*
 * LCD
 */
void display_write_data_4(byte data) {
    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | RS);
    Wire.endTransmission();

    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | RS | EN );
    Wire.endTransmission();

    delayMicroseconds(1); // enable ativo >450ns

    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | RS);
    Wire.endTransmission();

    delayMicroseconds(40);
}

void display_write_command_4(byte data) {
    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT );
    Wire.endTransmission();
    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT | EN );
    Wire.endTransmission();
    delayMicroseconds(1); // enable ativo >450ns
    Wire.beginTransmission(LCD_ADDRESS);
    Wire.write((data << 4) | LIGHT);
    Wire.endTransmission();
    delayMicroseconds(40);
}

void display_write_command_8(byte data) {
    display_write_command_4(data >> 4);
    display_write_command_4(data);
}

void display_write_data_8(byte data) {
    display_write_data_4(data >> 4);
    display_write_data_4(data);
}

void display_initialize() {
    Wire.begin();
    delay(50);

    display_write_command_4(0x03);
    delay(5);
    display_write_command_4(0x03);

    delayMicroseconds(200);

    display_write_command_4(0x03);
    display_write_command_4(0x02);
    display_write_command_4(0x02);
    display_write_command_4(0x08);

    delayMicroseconds(120);

    display_write_command_4(0x00);
    display_write_command_4(0x0F);

    delayMicroseconds(120);

    display_write_command_4(0x08);
    display_write_command_4(0x00);

    delayMicroseconds(120);
}

void display_clear() {
    display_write_command_8(0x01);
    delay(5);
}

void display_set_cursor(byte line, byte column) {
    display_write_command_8(0x80 | line << 6 | column);
}

void display_print_char(char data) {
    display_write_data_8(data);
    delayMicroseconds(120);
}

void display_print_string(String data) {
    display_clear();
    display_set_cursor(0,0);

    for (int i = 0; i < data.length(); i++) {
        if(i == 16) {
            display_set_cursor(1,0);
        }

        display_print_char(data[i]);
    }
}

void display_print_int(int data) {
    String s = String(data);
    display_print_string(s);
}
