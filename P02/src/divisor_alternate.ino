#define DEBOUNCE_DELAY 200

// Input variables
byte divisor;
byte dividend;

// Output variables
byte quotient;
byte remainder;
boolean rdy;
byte div_zero;

// Flip flop JK
volatile boolean Q;
boolean enabler;
boolean selector;

// Clock
unsigned long time_positive;
unsigned long time_negative;

byte I_D;
byte I_Q;
byte I_R;
byte R_D;
byte R_Q;
byte R_R;
boolean I;

void setup() {
    Serial.begin(9600);

    attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
    // enables interrupts
    interrupts();
    initialize();
    print_results("");
}

void initialize() {
    randomSeed(analogRead(A0));
     dividend = random(16);
    //dividend = 4;
     divisor = random(16);
    //divisor = 2;

    quotient = 0;
    remainder = dividend;
    rdy=0;
}

void loop() {
    //control_module();
    functional_module();
}

byte MUX_2x1(boolean S, byte A, byte B) {
    return S ? A : B;
}

byte register_memory(boolean E, byte D, byte Q) {
    return E ? D : Q;
}

byte subtract(byte A, byte B) {
    return A - B;
}

byte add(byte A, byte B) {
    return A + B;
}

void functional_module() {
    
    I = remainder >= divisor;
   
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
    
  
    selector = !Q;
    enabler = !Q | I;
    
    boolean J = true;
    boolean K = !I;
    Q = flip_flop_JK(J, K); 
}

void MCLK_positive() {
    unsigned long current_time = millis();

    if (current_time - time_positive >= DEBOUNCE_DELAY) {
        
        
        control_module();
        Serial.println(Q);
        if (!Q) {
            initialize();
            print_results("P >");
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
            rdy = remainder<divisor;
            print_results("N >");
        }


        attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
    }

    time_positive = current_time;
}

void print_results(String extra) {
    Serial.println(extra);
    Serial.print(" D ");
    Serial.print(dividend);
    Serial.print(" d ");
    Serial.print(divisor);
    Serial.print(" E ");
    Serial.print(enabler);
    Serial.print(" S ");
    Serial.print(selector);
    Serial.print(" Q ");
    Serial.print(quotient);
    Serial.print(" R ");
    Serial.print(remainder);
    Serial.print(" DZ ");
    Serial.print(div_zero);
    Serial.print(" RDY ");
    Serial.print(rdy);
    Serial.print(" I ");
    Serial.print(I);
    Serial.print(" Q ");
    Serial.print(Q);
    Serial.println();
}
