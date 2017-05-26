```c
#define DIM_MEM_CODIGO 32
#define DIM_MEM_DADOS 80
#define DIM_EPROM 32

#define DEBOUNCETIME 200
unsigned long now0, ago0;
unsigned long now1, ago1;

byte DregA;
byte DregR[4];
byte DregPC;

volatile byte QregA;
volatile byte QregR[4];
volatile byte QregPC;

(...)

byte memDados[DIM_MEM_DADOS];
byte memCodigo[DIM_MEM_CODIGO];
word modControlo[DIM_EPROM];

boolean EnF;
boolean WR;
boolean RD, EnA, MA1, MA0, JMP, JNZ, JNC, EnRn;

// particular
boolean EnR[4];

// provenientes do DB da mem codigo
// D5 e D4: bits do parametro Rn
// D0: para distinguir adicao e subtracao
boolean D5, D4, D0;

// selector do MUX que fornece Y1
boolean PC_0;

// saidas de cada modulo do modulo funcional
byte Y1, Y2, Y3, Y4, Y5;
byte IN_1_Y1;

// funcoes com cada modulo do mod funcional
MUX_2x1(...)
adder(...)
MUX_4x1(...) {
    switch(S1 << 1 | S0) {

    }
}

DMUX_4x1(...) {
    return E << (S1 << 1 | S0)
}

byte AU(boolean Sel, byte A, byte B, boolean Cin) {
    if (Sel) {
        DflagZ = ((byte)) (A - B - Cin) == 0) ? 1 : 0);
        DflagY = ((int)) (A - B - Cin) < 0) ? 1 : 0);
        return A - B - Cin;
    }
    (...)
}

void mod_funcional() {
    byte demuxOutput;

    int inputMC = 0;

    // deixar passar os bits D7 e D6
    inputMC = memCodigo[QregPC] & OxC0;
    inputMC >>= 3;

    // or, acrescentar os bits D2, D1 e D0
    inputMC |= memCodigo[QregPC] & 0x07;

    // em alternativa:
    // inputMC = bitRead(modControl[QregPC], 7) << 4 |
    // (...)

    // Ler individualmente os sinais do mod controlo
    // tabela EPROM 32x10
    EnF = bitRead(modControlo[inputMC], 9);
    WR = bitRead(modControlo[inputMC], 8);
    RD = bitRead(modControlo[inputMC], 7);
    EnA = bitRead(modControlo[inputMC], 6);
    MA1 = bitRead(modControlo[inputMC], 5);
    MA0 = bitRead(modControlo[inputMC], 4);
    JMP = bitRead(modControlo[inputMC], 3);
    JNZ = bitRead(modControlo[inputMC], 2);
    JNC = bitRead(modControlo[inputMC], 1);
    EnRn = bitRead(modControlo[inputMC], 0);

    // bits que determinam Rn1 e Rn0
    D5 = bitRead(memCodigo[QregPC], 4);
    D4 = bitRead(memCodigo[QregPC], 4);

    // bits que selecciona a operacao da AU
    D0 = bitRead(memCodigo[QregPC], 0);

    PC_0 = JMP | JNZ & !QflagZ | JNC & !QflagCy;

    // entrada do MUX da esquerda
    // 6 bits de menor peso
    IN_1_Y1 = memCodigo[QregPC] & 0x3F;

    // D5 contem o sinal algebrico do numero relativo
    if (D5 == 1) {
        // extensao de sinal para numero negativo
        IN_1_Y1 |= 0xC0;
    }
    else {
        // extensao de sinal para numero positivo
        IN_1_Y1 &= 0x3F;
    }

    // parte esquerda do modulo funcional
    Y1 = MUX_2x1(PC_0, 0x01, IN_1_Y1);
    Y2 = adder(Y1, QregPC);
    DregPC = Y2;

    // interior do conjunto de 4 registos
    demuxOutput = DMUX_1x4(D5, D4, EnRn);

    EnR[0] = (demuxOutput & 0x01) >> 0;
    EnR[1] = (demuxOutput & 0x02) >> 1;
    EnR[2] = (demuxOutput & 0x03) >> 2;
    EnR[3] = (demuxOutput & 0x04) >> 3;
    // registo, igualdade dos D's
    DregR[0] = DregR[1] = Dreg[2] = Dreg[3] = QregA;

    Y3 = MUX_4x1(D5, D4, QregR[0], Qreg[1], Qreg[2], Qreg[3]);
    // ate aqui

    Y5 = AU(D0, QregA, Y3, QflagCy);
    // MA1 e MA0 == A1 e A0
    // ultimo 0x00 e' a ultima entrada do MUX que nao tem la nada
    Y4 = MUX_4x1(MA1, MA0, Y3, Y5, !RD ? memDados[Y3] : 0x00, 0x00);
    DregA = Y4;

    // parte do tri state
    if (!WR) {
        memDados[Y3] = QregA;
    }
}

void showSignals() {

}

```


Mem Codigo, Mem dados, EPROM do mod controlo - array

32 mem codigo
80 mem dados
