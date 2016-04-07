#define pinoI 2
#define pinoSPTA 3
#define pinoSPTF 4
#define pinoSPC 5
#define pinoC0 6
#define pinoC1 7
#define pinoC2 8
#define pinoC3 9
#define pinoM 10
#define pinoR 11

//Variáveis lógias de entrada
bool I;
bool SPTA;
bool C0;
bool C1;
bool C2;
bool C3;
bool CC;
bool SPTF;
bool SPC;

//Variáveis lógicas de saída
bool M;
bool R;

//Variáveis correspondentes aos 4 bits do codigo secreto
bool S0 = 1;
bool S1 = 0;
bool S2 = 0;
bool S3 = 1;

//Função lógica de confirmação do código certo
bool codigoCerto(bool C0, bool C1, bool C2, bool C3) {
  return !(C0 ^ S0) & !(C1 ^ S1) & !(C2 ^ S2) & !(C3 ^ S3);
}

//Função lógica de saída M
bool funcaoM(bool I, bool SPTA, bool CC, bool SPTF, bool SPC) {
  return I & !SPTA | !CC & !SPTF & !SPC & !I | !SPTA & CC & !I | !SPTF & !SPTA & SPC & !I;
  // I and ~SPTA or ~I and CC and ~SPTA or ~I and ~SPTF and ~CC and ~SPC or ~I and ~SPTF and ~SPTA and SPC
}

//Função lógica de saída R
bool funcaoR(bool I, bool SPTA, bool CC,bool SPTF, bool SPC) {
  return !SPTA & I | (SPC |(CC & !SPTA)) & !I;
}

//Função de Leitura correspondentes aos sensores de entrada
void lerSensores() {
  I = digitalRead(pinoI);
  SPTA = digitalRead(pinoSPTA);
  SPTF = digitalRead(pinoSPTF);
  SPC = digitalRead(pinoSPC);
  C0 = digitalRead(pinoC0);
  C1 = digitalRead(pinoC1);
  C2 = digitalRead(pinoC2);
  C3 = digitalRead(pinoC3);
  CC = codigoCerto(C0,C1,C2,C3);
}

//Calcular o valor lógico das variáveis de saída
void calcularFuncoesSaida() {
  M = funcaoM(I, SPTA, CC, SPTF, SPC);
  R = funcaoR(I, SPTA, CC, SPTF, SPC);
}

//Escrever as variaveis de saida calculadas na funcao calcularFuncoesSaida
void escreverSaida() {
  digitalWrite(pinoM, M);
  digitalWrite(pinoR, R);
}

void setup() {
  //Definir o modo de entrada/saida dos pinos
  Serial.begin(9600);
  pinMode(pinoI, INPUT);
  pinMode(pinoSPTA, INPUT);
  pinMode(pinoSPTF, INPUT);
  pinMode(pinoSPC, INPUT);
  pinMode(pinoC0, INPUT);
  pinMode(pinoC1, INPUT);
  pinMode(pinoC2, INPUT);
  pinMode(pinoC3, INPUT);
  pinMode(pinoM, OUTPUT);
  pinMode(pinoR, OUTPUT);
}

void loop() {
  lerSensores();
  calcularFuncoesSaida();
  escreverSaida();
}
