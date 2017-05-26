
#define DIM_MEM_CODIGO 32
#define DIM_MEM_DADOS 80
#define DIM_EPROM 32

#define DEBOUNCETIME 200
unsigned long now0, ago0;
unsigned long now1, ago1;

//Entradas dos registos do CPU(D)
byte DregA;
byte DregR[4];
byte DregPC;

//Saidas dos registos do CPU
volatile byte QregA;
volatile byte QregR[4];
volatile byte QregPC;

//Flags
boolean DflagCy;
boolean DflagZero;

//memoria de dados
byte memDados[DIM_MEM_DADOS];

//memoria de codigo
byte memCodigo[DIM_MEM_CODIGO];

//EPROM do modulo de controlo
word modControlo[DIM_EPROM];

//Sinais do modulo de controlo
boolean EnF;
boolean WR;
boolean RD, EnA, MA1, MA0, JMP, JNZ, JNC, EnRn;

// Enables dos registos
boolean EnR[4];

// bits provenientes do DB da mem codigo
// D5 e D4: bits do parametro Rn
// D0: para distinguir adicao e subtracao
boolean D5, D4, D0;

// selector do MUX que fornece Y1
boolean PC_0;

// saidas de cada modulo do modulo funcional (saidas de MUXs e da ALU)
byte Y1, Y2, Y3, Y4, Y5;
byte IN_1_Y1; //Entrada 1 do modulo que dá Y1

// funcoes com cada modulo do mod funcional
MUX_2x1(...)
adder(...)
MUX_4x1(...) {
    switch(S1 << 1 | S0) {

    }
}

DMUX_1x4(...) {
    return E << (S1 << 1 | S0)
}

byte AU(boolean Sel, byte A, byte B, boolean Cin) {
    if (Sel) {
        DflagZ = ((byte)) (A - B - Cin) == 0) ? 1 : 0);
        DflagY = ((int)) (A - B - Cin) > 255) ? 1 : 0);
        return A - B - Cin;
    }
    (...)
}

void mod_funcional() {
    byte demuxOutput;
    
    //Compor os bits de entrada do moduilo de controlo a partir dos bits de dados da memoria de codigo
    int inputMC = 0;

    // //Mascarar para deixar passar os bits D7 E D6
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
    D5 = bitRead(memCodigo[QregPC], 5);
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
      Serial.println();
  Serial.println("Sinais provenientes do modulo de controlo e outros auxiliares ");
  Serial.println("--------------------------");
  Serial.print(" EnF ="); Serial.print(EnF)
  WR..RD..EnA..A1(//MA1)..A0..JMP..JNC..JNZ
  displayMenu();
}
void showInstructions(int instruction){
  boolean validinstruction1 = true; boolean validinstruction2 = true;
  switch(instriction)
}

void MCLK(){
  now0 = millis();
  if(now0 - ago0 >DEBOUNCETIME){
    //atualizar o PC
    QregPC = DregPC;

    //Mostrar a instrucao a cumprir no proximo ciclo
    if(QregPC < 0x10)
    Serial.print("0x0");
    else
      Serial.print("0x");
    Serial.print(QregPC, HEX)
  
} 

 void MCLKneg(){
  now1 = millis();
  if(now1 - ago1 >DEBOUNCETIME){
    if(EnA) QregA = DregA;
    if(EnR[0]) QregR[0] = DregR[0];
    if(EnR[0]) QregR[0] = DregR[0];
    if(EnR[0]) QregR[0] = DregR[0];
    if(EnR[0]) QregR[0] = DregR[0];

    if(EnF){
      Qflagz = DflagZ;
      QflagCy = QflagCy;
      
    }
  }
 }

void preencherEPROM(){
  for(int i = 0x00; i <= 0x07; i++){
    modControlo[i] = 0x188;
  }
  for(int i = 0x08; i <= 0x0F; i++){
    modControlo[i] = 0x184;
  }
}

boid preencherPrograma1(){
  byte X = memDados[0x10];
  byte Y = memDados[0x11];
  byte R = X + Y;

  QregR[0] = 0x10;
  QregR[1] = 0x11;
  QregR[2] = 0x12;

  memCodigo[0x00] = 0xc0;
}

void preencheDados(){
  memDados[0x10] = 0x05;
  memDados[0x11] = 0x06;
  memDados[0x12] = 0xAA;
}




void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  randomizeMemories(), //Preencher o conteudo das memorias com valores random
  preencheEPROM(),
  preencheDados();
  preenchePrograma3();
  displayMenu();
  startExecuting();
  //funcoes interrupt realizam a actualizacao dos registos e flags do CPU
  attachInterrupt(digitalPinToInterrupt(2), MCLK, FALLING);
  interrupts();
  
}

void loop() {
  calcularFuncoesCombinatorias();
  LerRespostaUtilizador();
```


Mem Codigo, Mem dados, EPROM do mod controlo - array

32 mem codigo
80 mem dados
