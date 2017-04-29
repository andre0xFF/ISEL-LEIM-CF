// Functional module

// Control module

// Flip flop J-K
boolean J;
boolean K;

void setup() {

}

void loop() {

}

void divisor() {

}

byte I_register(byte I_D) {
    return I_D;
}

byte R_register(byte R_D) {
    return R_D;
}

void functional_module() {
    I_R = subtract(A, B);
    I_Y = MUX_2x1(I_SELECTOR, D, I_R);
    I_Q = register_I(I_Y);
    REMAINDER = I_Q;

    R_R = add(A, B);
    R_Y = MUX_2x1(R_SELECTOR, 0, R_R);
    R_Q = R_register(R_Y);
    QUOTIENT = R_Q;
}

void control_module() {

}
// Subtract module
byte subtract(byte A, byte B) {
    return A - B;
}

// Sum module
byte add(byte A, byte B) {
    return A + B;
}

byte MUX_2x1(boolean selector, byte input_0, input_1) {
    switch(selector) {
        case 0:
            return input_0;
        case 1:
            return input_1;
    }
}
