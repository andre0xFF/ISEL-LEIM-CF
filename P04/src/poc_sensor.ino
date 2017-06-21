#define STATE_WAITING 0
#define STATE_GENERATE_TRIGGER 1
#define STATE_CALCULATE_DISTANCE 2

#define PIN_SENSOR_ECHO 2
#define PIN_SENSOR_TRIGGER 4

#define SENSOR_TRIGGER_GAP 60
#define SENSOR_TRIGGER_DELAY 10
#define SENSOR_ECHO_LIMIT 38

volatile int sensor = STATE_GENERATE_TRIGGER;

unsigned long sensor_trigger_timer;
volatile boolean sensor_echo;
volatile unsigned long sensor_echo_initial_time;
volatile unsigned long sensor_echo_final_time;

double object_distance[] = {-1, -1};
long object_time[] = {-1, -1};

void setup() {
    Serial.begin(9600);
    pinMode(PIN_SENSOR_TRIGGER, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_ECHO), on_echo_released, RISING);
    interrupts();
}

void loop() {
    sensor_state_machine();
}

void sensor_state_machine() {
    if (sensor == STATE_GENERATE_TRIGGER) {
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

    float delta_distance = abs(object_distance[1] - object_distance[0]);
    float delta_time = abs(object_time[1] - object_time[0]);
    float object_velocity = delta_distance / delta_time;
    object_velocity *= pow(10, 4);

    if (object_velocity == 0) {
        return;
    }

    Serial.print(object_velocity);
    Serial.println(" m/s");
}
