#define STATE_WAITING 0
#define STATE_BUTTON_CLICKED 1
#define STATE_BUTTON_CHECKING 2
#define PIN_BUTTON 7

int button = STATE_WAITING;

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
    button_status = digitalRead(PIN_BUTTON);

    if (button == STATE_WAITING && button_status) {
        button_timer = millis();
        button = STATE_BUTTON_CHECKING;
        on_button_checking();
        return;
    }

    long timer = millis() - button_timer;

    if (button == STATE_BUTTON_CHECKING && timer > 20) {
        button = STATE_BUTTON_CLICKED;
        on_button_clicked();
        return;
    }

    if (button == STATE_BUTTON_CLICKED) {
        button = WAITING;
        on_button_waiting();
        return;
    }
}

/**
    STATE MACHINES EVENTS
*/
void on_button_waiting() {
    Serial.print("Just waiting for something to happen..");
}

void on_button_clicked() {
    Serial.print("I have been clicked!");
}

void on_button_checking() {
    Serial.print("Checking the thing");
}
