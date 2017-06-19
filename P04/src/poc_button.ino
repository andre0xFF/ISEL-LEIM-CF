#define STATE_WAITING 0
#define STATE_BUTTON_CLICKED 1
#define PIN_BUTTON 7

#define BUTTON_PRESSING_INTERVAL 20

int button = STATE_WAITING;
long button_timer;
boolean button_status;

void setup() {
    Serial.begin(9600);
    pinMode(PIN_BUTTON, INPUT);
}

void loop() {
    button_state_machine();
}

/**
    STATE MACHINES
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

    if (button = STATE_BUTTON_CLICKED) {
        on_button_clicked();

        if (true) {
            button = STATE_WAITING;
        }
        return;
    }
}

/**
    STATE MACHINES EVENTS
*/
void on_button_waiting() {
    Serial.println("Just waiting for something to happen..");
}

void on_button_clicked() {
    Serial.println("I have been clicked!");
}

void on_button_checking() {
    Serial.println("Checking the thing");
}
