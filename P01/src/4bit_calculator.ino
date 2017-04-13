// Pin numbers
#define PIN_A1 10
#define PIN_A0 9
#define PIN_B1 8
#define PIN_B0 7

#define PIN_C2 11
#define PIN_C1 12
#define PIN_C0 13

#define PIN_Y3 6
#define PIN_Y2 5
#define PIN_Y1 4
#define PIN_Y0 3

#define PIN_FLAG 2

// Input variables
boolean _A1;
boolean _A0;
boolean _B1;
boolean _B0;

// Operation selectors
boolean _C2;
boolean _C1;
boolean _C0;

// Output variables
boolean _Y3;
boolean _Y2;
boolean _Y1;
boolean _Y0;
boolean _FLAG;

void setup() {
    Serial.begin(9600);
    pinMode(PIN_A1, INPUT);
    pinMode(PIN_A0, INPUT);
    pinMode(PIN_B1, INPUT);
    pinMode(PIN_B0, INPUT);
    pinMode(PIN_C2, INPUT);
    pinMode(PIN_C1, INPUT);
    pinMode(PIN_C0, INPUT);
    pinMode(PIN_Y3, OUTPUT);
    pinMode(PIN_Y2, OUTPUT);
    pinMode(PIN_Y1, OUTPUT);
    pinMode(PIN_Y0, OUTPUT);
    pinMode(PIN_FLAG, OUTPUT);
}

void read_inputs() {
    _A1 = digitalRead(PIN_A1);
    _A0 = digitalRead(PIN_A0);
    _B1 = digitalRead(PIN_B1);
    _B0 = digitalRead(PIN_B0);
    _C2 = digitalRead(PIN_C2);
    _C1 = digitalRead(PIN_C1);
    _C0 = digitalRead(PIN_C0);
}

void calculate() {
    _Y3 = 0;
    _Y2 = 0;
    _Y1 = 0;
    _Y0 = 0;
    _FLAG = 0;

    _Y3 = op_div(div_q1()) | op_mult(mult_s3()) | op_not(not_g1()) | op_sub(sub_s2());
    _Y2 = op_div(div_q0()) | op_mult(mult_s2()) | op_not(not_g0()) | op_sub(sub_s2()) | op_add(add_s2());
    _Y1 = op_div(div_r1()) | op_mult(mult_s1()) | op_not(not_h1()) | op_sub(sub_s1()) | op_add(add_s1()) | op_xor(xor_f1()) | op_or(or_f1()) | op_and(and_f1());
    _Y0 = op_div(div_r0()) | op_mult(mult_s0()) | op_not(not_h0()) | op_sub(sub_s0()) | op_add(add_s0()) | op_xor(xor_f0()) | op_or(or_f0()) | op_and(and_f0());
    _FLAG = op_div(div_dz());
}

boolean nand(boolean A, boolean B) { return !(A & B); }

boolean op_not(boolean bit) { return bit & _C2 & _C1 & !_C0; }
boolean op_div(boolean bit) { return bit & !_C2 & _C1 & _C0; }
boolean op_mult(boolean bit) { return bit & !_C2 & _C1 & !_C0; }
boolean op_add(boolean bit) { return bit & !_C2 & !_C1 & !_C0; }
boolean op_sub(boolean bit) { return bit & !_C2 & !_C1 & _C0; }
boolean op_xor(boolean bit) { return bit & _C2 & _C1 & _C0; }
boolean op_or(boolean bit) { return bit & _C2 & !_C1 & _C0; }
boolean op_and(boolean bit) { return bit & _C2 & !_C1 & !_C0; }

boolean not_g1() { return nand(_A1, 1); }
boolean not_g0() { return nand(_A0, 1); }
boolean not_h1() { return nand(_B1, 1); }
boolean not_h0() { return nand(_B0, 1); }

boolean div_q1() { return _A1 & !_B1; }
boolean div_q0() { return _A0 & _A1 | _A1 & _B1 & !_B0 | _A0 & _B0 & !_B1; }
boolean div_r1() { return _B0 & _B1 & _A1 & !_A0; }
boolean div_r0() { return _A0 & _B0 & !_A1 | _A0 & _B1 & !_B0; }
boolean div_dz() { return !_B1 & !_B0; }

boolean mult_s3() { return _B1 & _A0 & _B0 & _A1; }
boolean mult_s2() { return _B1 & _A1 & !_B0 | _B1 & _A1 & !_A0; }
boolean mult_s1() { return _A0 & _B1 & !_B0 | _A1 & _B0 & !_B1 | _B1 & _A0 & !_A1 | _B0 & _A1 & !_A0; }
boolean mult_s0() { return _A0 & _B0; }

boolean add_s2() { return _B1 & _A1 | _A0 & _A1 & _B0 | _A0 & _B1 & _B0; }
boolean add_s1() { return _B1 & !_A0 & !_A1 | _B1 & !_A1 & !_B0 | _A1 & !_B0 & !_B1 | _A1 & !_A0 & !_B1 | _A0 & _B0 & !_B1 & !_A1 | _B1 & _A0 & _B0 & _A1; }
boolean add_s0() { return _A0 & !_B0 | _B0 & !_A0; }

boolean sub_s2() { return _B1 & !_A1 | _B0 & _B1 & !_A0 | _B0 & !_A0 & !_A1; }
boolean sub_s1() { return _B1 & !_A1 & !_B0 | _B1 & _A0 & !_A1 | _B0 & _B1 & _A1 & !_A0 | _A1 & !_B1 & !_B0 | _A1 & _A0 & !_B1 | _B0 & !_A1 & !_B1 & !_A0; }
boolean sub_s0() { return _A0 & !_B0 | _B0 & !_A0; }

boolean xor_f1() { return nand(nand(_A1, nand(_B1, 1)), nand(_B1, nand(_A1, 1))); }
boolean xor_f0() { return nand(nand(_A0, nand(_B0, 1)), nand(_B0, nand(_A0, 1))); }

boolean or_f1() { return nand(nand(_B1, 1), nand(_A1, 1)); }
boolean or_f0() { return nand(nand(_A0, 1), nand(_B0, 1)); }

boolean and_f1() { return nand(nand(_A1, _B1), 1); }
boolean and_f0() { return nand(nand(_A0, _B0), 1); }

void write_outputs() {
    digitalWrite(PIN_Y3, _Y3);
    digitalWrite(PIN_Y2, _Y2);
    digitalWrite(PIN_Y1, _Y1);
    digitalWrite(PIN_Y0, _Y0);
    digitalWrite(PIN_FLAG, _FLAG);
}

void loop() {
    read_inputs();
    calculate();
    write_outputs();
}
