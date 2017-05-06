```c
#define DEBOUNCETIME 200

// timestamp para guardar o instante da interrupcao anterior
unsigned long nowMCLK, agoMCLK, nowMCLKneg, agoMCLneg;

// operandos e resultado da multiplicacao
byte X, Y, R, I;

// registos e flip flop, compostos por D e Q. nas saidas dos
// registos e flip flop de estado, como estas variaveis sao afectadas na rotina de
// interrupt, devem ter qualificador volatile para serem guardadas em RM e nao num
// registo do CPU

boolean D;      // entrada D
boolean J, K;   // entradas J e K
byte DregI;
byte DregR;
volatile boolean Q;
volatile byte QregI;
volatile byte QregR;

// entrada do modulo de controlo
// "I diferente de zero"
boolean Id0;
// saidas do modulo de controlo
// "saidas dos multiplexers e somadores"
boolean EnI;
boolean EnR;
boolean SelI;
boolean SelR;

void algoritmoMultiplicador() {
    Serial.println("X = " + (String)X);

    for (R = 0, I = Y; I != 0; --I) {
        R += X;
        Serial.println("X = " + (String)X + "\t I = " + (String)I);
    }

    Serial.println("\n R = .. I");
    Serial.println("");
}

byte MUX_2x1(boolean S, byte IN_0, byte IN_1) {
    switch(s) {
        case 0:
            return IN_0;
        case 1:
            return IN_1;
    }
}

byte adder(byte A, byte B) {
    return A + B;
}

void calcularVariaveisCombinatorias() {
    // Modulo de controlo
    // "Modulo funcional"
    SelI = !Q;
    EnI = !Q | Id0;
    SelR = SelI;
    EnR = EnI;
    D = !Q | Id0;
    // J = 1;
    // K = !Id0;

    // Estrutura associada a I
    byte RI = adder(-1, QregI);
    byte YI = MUX_2x1(SelI, RI, Y);
    DregI = YI;
    Id0 = (QregR != 0 ? 1 : 0);

    // Estrutura associada a R
    RR = adder(X, QregR);
    YR = MUX_2x1(SelR, RR, 0);
    DregR = YR;
}

// Funcao que corre quando o clock sobe
void MCLK() {
    nowMCLK = millis();
    if (nowMCLK - agoMCLK > DEBOUNCETIME) {
        // Actualizar o flip flop (D) do estado do modulo de controlo
        Q = D;
        // Atualizar o flip flop (J-K) do estado do modulo de controlo
        // Q = Q & !K | !Q & J;
        if (Q == 0) {
            X = random(16);
            Y = random(16);
            // print X e Y,
        }

        attachInterrupt(digitalPinToInterrupt(2), MCLKneg, FALLING);

    }

    agoMCLK = nowMCLK;
}

// Funcao que corre quando o clock desce
void MCLKneg() {
    nowMCLK = millis();

    if (nowMCLKneg - agoMCLKneg > DEBOUNCETIME) {
        if(EnI) QregI = DregI;
        if(EnR) QregR = DregR;

        // Prints de:
        // Estado = Q
        // I = QregI
        // R = QregR
        // QregI == 0 ? : Resultado final!

        attachInterrupt(digitalPinToInterrupt(2), MCLK, RISING);
    }

    agoMCLKneg = nowMCLKneg;
}

void mostrarSinais() {
    // print SelI, SelR, EnR, EnI
}

void mostrarVariaveis() {
    // print YI, YR, RI, RR, QregI, QregR, Id0
}

void mostrarRegistos() {
    // print Q, QrefI, QregR
}

void help() {
    // ..
}

void lerRespostasUtilizador() {
    switch(Serial.read()) {
        // s : mostrarSinais()
        // v : mostrarVariaveis()
        // r : mostrarRegistos()
        // h : help()
    }
}

void setup() {
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(2), MCLK, RISING);
    interrupts();

    randomSeed(analogRead(A0));

    X = random(16);
    Y = random(16);

    Q = 0;

    // print X, Y
    // print estado Q
    Serial.println("X = " + X + ", Y = " + Y);
    Serial.print("Q = " + Q);
}

void loop() {
    calcularVariaveis();

    if (Serial.available()) {
        lerRespostasUtilizador();
    }
}

```

RI                  RR

YI                  YR

QregI       Id0     QregR



Id0
