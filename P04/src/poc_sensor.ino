#define STATE_WAITING 0
#define STATE_GENERATE_TRIGGER 1
#define STATE_CALCULATE_DISTANCE 2

#define PIN_SENSOR_ECHO 2
#define PIN_SENSOR_TRIGGER 0

#define SENSOR_TRIGGER_GAP 60
#define SENSOR_TRIGGER_DELAY 10
#define SENSOR_ECHO_LIMIT 38

int sensor = STATE_GENERATE_TRIGGER;
int sensor_async = STATE_GENERATE_TRIGGER;

boolean sensor_echo;
long sensor_trigger_timer;
long echo_timer;
long echo_delta;

int object_distances[] = {-1, -1};
long object_time[] = {-1, -1};
int object_velocity;

void setup() {
    Serial.begin(9600);
    pinMode(PIN_SENSOR_TRIGGER, OUTPUT);
    pinMode(PIN_SENSOR_ECHO, INPUT);
    attachInterrupt(PIN_SENSOR_TRIGGER, sensor_interrupt, RISING);
}

void loop() {
    sensor_state_machine();
}

void sensor_interrupt() {
    if (sensor_async == STATE_GENERATE_TRIGGER) {
        sensor_echo = false;
        echo_timer = micros();

        attachInterrupt(PIN_SENSOR_TRIGGER, sensor_interrupt, RISING);
        if (true) {
            sensor_async = STATE_WAITING;
        }

        return;
    }

    if (sensor_async == STATE_WAITING) {
        sensor_echo = true;
        echo_delta = micros() - echo_timer;
        on_echo_received();

        attachInterrupt(PIN_SENSOR_TRIGGER, sensor_interrupt, FALLING);
        if (true) {
            sensor_async = STATE_GENERATE_TRIGGER;
        }

        return;
    }
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
        long timer = millis() - echo_timer * 1000;

        if (timer > SENSOR_ECHO_LIMIT && !sensor_echo) {
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

    // Serial.println("Echo released.");
    return true;
}

void on_echo_received() {
    // Serial.print("Echo received!");
}

void on_echo_lost() {
    Serial.println("Echo lost.");
}

void sensor_process() {
    object_time[1] = object_time[0];
    object_time[0] = millis();

    object_distances[1] = object_distances[0];
    object_distances[0] = echo_delta / 58;

    if (object_distances[1] == -1 || object_time[1] == -1) {
        return;
    }

    int delta_distance = abs(object_distances[1] - object_distances[0]);
    int delta_time = abs(object_time[1] - object_time[0]);

    object_velocity = delta_distance / delta_time;

    Serial.print(object_velocity);
    Serial.println(" cm/ms");
}
