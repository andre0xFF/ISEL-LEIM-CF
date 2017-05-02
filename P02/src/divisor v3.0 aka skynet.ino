#define DEBOUNCE_DELAY 200

//Input variables
byte divisor;
byte dividend;

// Output variables
byte quotient;
byte remainder;

// Flip flop JK
volatile boolean Q;
boolean enabler;
boolean selector;

// Modules
volatile byte I_Q;
volatile byte R_Q;

// Clock
boolean mclock;
unsigned long time_positive;
unsigned long time_negative;

void setup() {
    Serial.begin(9600);

    attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
    // enables interrupts
    interrupts();
    initialize();

     Serial.println("------------Estado inicial do modulo de controlo-------------");

    Serial.print("Q = ");
    Serial.println(Q);

    Serial.print("Enabler = ");
    Serial.println(enabler);

    Serial.print("Selector = ");
    Serial.println(selector);

    Serial.print("Dividendo = ");
    Serial.println(dividend);

    Serial.print("Divisor = ");
    Serial.println(divisor);

    Serial.print("quotient = ");
    Serial.println(quotient);

    Serial.print("remainder = ");
    Serial.println(remainder);
    Serial.println("------------Fim do estado inicial do modulo de controlo-------------");
}

void initialize() {
    randomSeed(analogRead(A0));
    divisor =3;//random(16);
    dividend =15;//random(16);
    quotient = 0;
    //remainder = dividend;
    remainder=MUX_2x1(selector, dividend, remainder);
}

void loop() {

    functional_module(dividend, divisor, selector, enabler, mclock);

    // TODO: Add functionality for when the Serial is available
    // Print stuff and help menu

    // if (Serial.available()) {
    //
    // }

}

byte register_memory(byte D, boolean E, boolean CLK) {
    byte _Q = D;
    return _Q;
}

void functional_module(byte D, byte d, boolean S, boolean E, boolean CLK) {
    boolean I_selector = S;
    boolean R_selector = S;
    boolean I_enable = E;
    boolean R_enable = E;
    boolean I_clock = CLK;
    boolean R_clock = CLK;

    byte I_R = subtract(remainder, d);
    byte I_Y = MUX_2x1(I_selector, D, I_R);

    byte I_D = I_Y;

   

    I_Q = register_memory(I_D, I_enable, I_clock);
    
    byte R_R = add(1, quotient);
    byte R_Y = MUX_2x1(R_selector, 0, R_R);
    byte R_D = R_Y;

    R_Q = register_memory(R_D, R_enable, R_clock);
}

boolean flip_flop_JK(boolean J, boolean K, boolean clk) {
    return Q & !K | !Q & J;
}

void control_module(boolean condition, boolean clk) {
    Serial.println("-----------inicio do modulo de controlo(clock positivo)------------");
    
    Serial.print("condition = ");
    Serial.println(condition);

    Serial.print("clk = ");
    Serial.println(clk);
    Serial.println("-----------Fim do modulo de controlo(clock positivo)------------");
    boolean J = true;
    boolean K = !condition;

    if (!condition && clk) {
      Serial.println("inicializou!!!");
      Serial.println("********NOVA CONTA*******");
      initialize();
    }
    // left side of the condition
    Q = flip_flop_JK(J, K, clk);
    selector = Q;
    // left or right side of the condition
    enabler = Q | condition;
}

// TODO: Review and debug this module
// TODO: positive and negative MCLK functions can probably
// be refactored to 1 function
// Control module runs first (rising clock)
void MCLK_positive() {
    unsigned long current_time = millis();

    if (current_time - time_positive > DEBOUNCE_DELAY) {
        mclock = true;
        boolean condition = remainder - divisor >= 0;

        control_module(condition, mclock);

        if (!Q) {
            initialize();
        }
        attachInterrupt(digitalPinToInterrupt(2), MCLK_negative, FALLING);
    }

    time_positive = current_time;
}

// TODO: Review and debug this module
// Functional module runs last (falling clock)
void MCLK_negative() {
  Serial.println("------------Inicio do clock negativo--------------------");
    unsigned long current_time = millis();

    if (current_time - time_negative > DEBOUNCE_DELAY) {
        if (enabler) {
          
            Serial.print("I_Q during during negative clock = ");
            Serial.println(I_Q);
            
            remainder = I_Q;
            quotient = R_Q;
        }

        mclock = false;
        
        boolean condition = remainder - divisor >= 0;
        
        Serial.print("condition during negative clock.....(remainder-divisor>=0)= ");
        Serial.println(condition);
        attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
    }

    time_negative = current_time;
    Serial.println("  ");

    Serial.print("Q = ");
    Serial.println(Q);

    Serial.print("Dividendo = ");
    Serial.println(dividend);

    Serial.print("Divisor = ");
    Serial.println(divisor);

    Serial.print("enabler = ");
    Serial.println(enabler);

    Serial.print("selector = ");
    Serial.println(selector);

    Serial.print("quotient = ");
    Serial.println(quotient);

    Serial.print("remainder = ");
    Serial.println(remainder);

    Serial.println("-----------------------Fim do clock negativo ----------------------");
    Serial.println("");
}

// Subtract module
byte subtract(byte A, byte B) {
    return A - B;
}

// Sum module
byte add(byte A, byte B) {
    return A + B;
}

// Multiplexer module
byte MUX_2x1(boolean selector, byte input_0, byte input_1) {
    switch(selector) {
        case 0:
            //return input_1;
            return input_0;
        case 1:
           // return input_0;
            return input_1;
    }
}
