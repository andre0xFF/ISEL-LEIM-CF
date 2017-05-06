#define DEBOUNCE_DELAY 200

// Input variables
unsigned int divisor;
unsigned int dividend;

// Output variables
volatile unsigned int quotient;
volatile unsigned int remainder;
unsigned int ready;

// Flip flop JK
volatile boolean Q;
boolean J;
boolean K;
boolean enabler;
boolean selector;

// Clock
unsigned long time_positive;
unsigned long time_negative;

unsigned int I_D;
unsigned int I_Q;
unsigned int I_R;
unsigned int R_D;
unsigned int R_Q;
unsigned int R_R;
boolean I;
boolean DZ;

void setup() {
    Serial.begin(9600);

    attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
    interrupts();
}

void initialize() {
    randomSeed(analogRead(A0));
    dividend = random(16);
    divisor = random(16);

    quotient = 0;
    remainder = dividend;

    ready = false;
    enabler = false;
    selector = false;
    Q = false;
    J = false;
    K = false;
    I = false;
    DZ = false;

    Serial.print("\n > ");
    Serial.print(dividend);
    Serial.print(" % ");
    Serial.print(divisor);
    Serial.println();
}

void loop() {
    control_module();
    functional_module();

    if (Serial.available()) {
        read_input();
    }
}

unsigned int MUX_2x1(boolean S, unsigned int A, unsigned int B) {
    return S ? A : B;
}

unsigned int register_memory(boolean E, unsigned int D, unsigned int Q) {
    return E ? D : Q;
}

unsigned int subtract(unsigned int A, unsigned int B) {
    return A - B;
}

unsigned int add(unsigned int A, unsigned int B) {
    return A + B;
}

boolean NOR(boolean A, boolean B) {
    return !A | !B;
}

void functional_module() {
    I_R = subtract(remainder, divisor);
    I_D = MUX_2x1(selector, dividend, I_R);
    I_Q = register_memory(enabler, I_D, I_Q);

    R_R = add(1, quotient);
    R_D = MUX_2x1(selector, 0, R_R);
    R_Q = register_memory(enabler, R_D, R_Q);
}

boolean flip_flop_JK(boolean J, boolean K) {
    return Q & !K | !Q & J;
}

void control_module() {
    J = !Q & !DZ & I;
    K = Q & !I;
    enabler = !Q & DZ | !Q & !I & !DZ | Q & I;
    selector = !Q & DZ | !Q & !I & !DZ;

    I = remainder >= divisor;
    DZ = divisor == 0;
    ready = !I | DZ;
}

void MCLK_positive() {
    unsigned long current_time = millis();

    if (current_time - time_positive >= DEBOUNCE_DELAY) {
      	Q = flip_flop_JK(J, K);

        if (!Q) {
            initialize();
            print_results("");
        }

        attachInterrupt(digitalPinToInterrupt(2), MCLK_negative, FALLING);
    }


    time_positive = current_time;
}

void MCLK_negative() {
    unsigned long current_time = millis();

    if (current_time - time_positive >= DEBOUNCE_DELAY) {
        if (enabler) {
            remainder = I_Q;
            quotient = R_Q;
        }

        control_module();
        print_results("");

        attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
    }

    time_positive = current_time;
}

void print_results(String extra) {
    Serial.print(extra);
    Serial.print(" E ");
    Serial.print(enabler);
    Serial.print(" Q ");
    Serial.print(quotient);
    Serial.print(" R ");
    Serial.print(remainder);
    Serial.print(" DZ ");
    Serial.print(DZ);
    Serial.print(" RDY ");
    Serial.print(ready);
    Serial.println();
}

void read_input() {
    switch (Serial.read()) {
        case 'C':
            print_control_module();
            break;
        case 'F':
            print_functional_module();
            break;
        case 'R':
            print_memory();
            break;
    }
}

void print_control_module() {
    // Control module outputs
    Serial.print(" > I: ");
    Serial.print(" E ");
    Serial.print(enabler);
    Serial.print(" S ");
    Serial.print(selector);
    Serial.println();
    Serial.print("> R: ");
    Serial.print(" E ");
    Serial.print(enabler);
    Serial.print(" S ");
    Serial.print(selector);
    Serial.println();
}

void print_functional_module() {
    // Functional module outputs
    Serial.print(" > I: ");
    Serial.print(" SUB ");
    Serial.print(I_R);
    Serial.print(" MUX ");
    Serial.print(I_D);
    Serial.print(" MEM ");
    Serial.print(I_Q);
    Serial.println();
    Serial.print(" > R: ");
    Serial.print(" SUB ");
    Serial.print(R_R);
    Serial.print(" MUX ");
    Serial.print(R_D);
    Serial.print(" MEM ");
    Serial.print(R_Q);
    Serial.println();
}

void print_memory() {
    // Memory registers and flip flops
    Serial.print(" > Reg: ");
    Serial.print(" Q ");
    Serial.print(Q);
    Serial.print(" J ");
    Serial.print(J);
    Serial.print(" K ");
    Serial.print(K);
    Serial.print(" I_Q ");
    Serial.print(I_Q);
    Serial.print(" I_R ");
    Serial.print(I_R);
    Serial.println();
}
