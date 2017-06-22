#include <Wire.h>
#define RS 0x01 //pq o RS está ligado ao D0
#define RW 0x02
#define EN 0x04//como está ligado no D2 =>(D2)=1 (D1)=0 (D0)=0
#define LUZ 0x08
#define ENDERECO 0x3F
 
int estado;
int estadoBtn;
#define Premido 0
#define Repouso 1
#define PrepararMedicao 0
#define esperarEco 1
#define gerarTrig 1
#define EsperarTpwm 2
#define CalcularDistancia 5
#define MostrarDistancia 6
 
int estadoPWM;
#define Tascendente 3
#define Tdescendente 4
 
unsigned long Ttrig, Tasc, Tdes, T1, pressed;
int pinBotao = 7;
int TriggerGap = 60;
boolean haEco, Botao, start;
int Distancia, DistanciaNow;
int pinTrigger = 4;
int pinEco = 2;
unsigned long now, ago, now2;
 
/*----------MÉTODOS----------*/
void escreverDados4(byte quatroBits) {
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatroBits << 4) | LUZ | RS);
  Wire.endTransmission();
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatroBits << 4) | LUZ | RS | EN );
  Wire.endTransmission();
  delayMicroseconds(1); // enable ativo >450ns
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatroBits << 4) | LUZ | RS);
  Wire.endTransmission();
  delayMicroseconds(40);
}
void escreverComando4(byte quatrobits) {
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ );
  Wire.endTransmission();
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ | EN );
  Wire.endTransmission();
  delayMicroseconds(1); // enable ativo >450ns
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ);
  Wire.endTransmission();
  delayMicroseconds(40);
}
void escreverComando8(byte oitoBits) {
  escreverComando4(oitoBits >> 4);
  escreverComando4(oitoBits);
}


 
void escreverDados8(byte oitoBits) {
  escreverDados4(oitoBits >> 4);
  escreverDados4(oitoBits);
}
 
 
void DisplayInit() {
  Wire.begin();
  delay(50);
 
  escreverComando4(0x03);
  delay(5);
  escreverComando4(0x03);
 
  delayMicroseconds(200);
 
  escreverComando4(0x03);
  escreverComando4(0x02);
  escreverComando4(0x02);
  escreverComando4(0x08);
 
  delayMicroseconds(120);
 
  escreverComando4(0x00);
  escreverComando4(0x0F);
 
  delayMicroseconds(120);
 
  escreverComando4(0x08);
  escreverComando4(0x00);
 
  delayMicroseconds(120);
}
 
void DisplayClear() {
  escreverComando8(0x01);
  delay(5);
 
}
void DisplaySetCursor(byte linha, byte coluna) {
 
  escreverComando8(0x80 | linha << 6 | coluna);
 
}
 
void DisplayPrintChar(char c) {
  escreverDados8(c);
  delayMicroseconds(120);
}
 
void DisplayCursorHome() {
  escreverComando8(0x02);
  delay(5);
}
 
void DisplayEntryMode(byte I_D, byte S) {
  escreverDados8(0x04 | I_D << 1 | S);
  delayMicroseconds(120);
 
}
 
void DisplayPrintString(String s) {
  for (int j = 0; j < s.length(); j++) {
    DisplayPrintChar(s[j]);
  }} 
void DisplayPrintNumero(int n) {
  String numero = String(n);
  DisplayPrintString(numero);
}
 
void GerarTrigger() {
  Ttrig = millis(); //botaum
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);
}
 
/*MÉTODO DE INTERRUPÇÃO (MEDIDOR DE Tpwm)*/
void MetodoDeInterrupcao() {
  switch (estadoPWM) {
 
    case Tascendente:
      Tasc = micros();
      attachInterrupt(0, MetodoDeInterrupcao, FALLING);
      estadoPWM = Tdescendente;
      return;
 
    case Tdescendente:
      Tdes = micros();
      haEco = true;
      T1 = Tdes - Tasc;
      attachInterrupt(0, MetodoDeInterrupcao, RISING);
      estadoPWM = Tascendente;
      return;
  }
}
 


































void MedidorDistancias() {
switch (estado){
    case gerarTrig:
     if (start || millis() - pressed > 500){
      haEco = false;
      now = millis();
      pressed = millis();
      if (now - ago > 100){
        GerarTrigger();
        if (millis() - Ttrig < TriggerGap){
          estado = EsperarTpwm;
        }
        else if (millis() - Ttrig > TriggerGap){
          estado = gerarTrig;
        }
      }
      ago = now;
      break;
 
     case EsperarTpwm:
      if (haEco){
        Distancia = T1/58;
        DisplayClear();
        DisplaySetCursor(0,0);
        if (Distancia >= 2 && Distancia <= 400){
          DisplayPrintString("Dist - "+String(Distancia) + "cm");
          Serial.println("Dist - "+String(Distancia) + "cm");
        }
        else if (Distancia < 2){
          DisplayPrintString("ERRO - < 2cm");
          Serial.println("ERRO - Distancia Menor que 2cm");
        }
        else {
          DisplayPrintString("ERRO - > 4m");
          Serial.println("ERRO - Distancia Maior que 4m");
        }
        estado = gerarTrig;
      }
      else if (millis() - Ttrig > TriggerGap && !haEco){
        DisplayClear();
        DisplayPrintString("Eco Perdido");
        Serial.println("Eco Perdido");
        estado = gerarTrig;
      }
      break;
     }
  }
}

void botao(){
  switch(estadoBtn){
    case Repouso:
      start = false;
      if (millis() - now2 > 20){
        if (!digitalRead(8)){
          now2 = millis();
          estadoBtn = Premido;
        }
      }
      break;
    case Premido:
      start = true;
      if (millis() - now2 > 20){
        if (digitalRead(8)){
          now2 = millis();
          estadoBtn = Repouso;
        }
      }
      break;
  }
}
 
void setup() {
  Serial.begin(9600);
  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEco, INPUT);
  pinMode(8, INPUT);
  estadoBtn = Repouso;
  estadoPWM = Tdescendente;
  estado = gerarTrig;
  DisplayInit();
  DisplayClear();
  haEco = false;
  attachInterrupt(0, MetodoDeInterrupcao, RISING);
}
 
void loop() {
  if (Serial.available()){
    if(Serial.read() == 'L'){
      start = true;
    }
  }
  //botao();
  MedidorDistancias();
  //start = false;
}
