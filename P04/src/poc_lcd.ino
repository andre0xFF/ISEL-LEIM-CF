#include <Wire.h>

// Addresses
#define RS 0x01
#define RW 0x02
#define EN 0x04
#define LIGHT 0x08
#define LCD_ADDRESS 0x3F

void display_write_data_4(byte data) {     
      Wire.beginTransmission(LCD_ADDRESS);
      Wire.write((data << 4) | LIGHT | RS);
      Wire.endTransmission();
      
      Wire.beginTransmission(LCD_ADDRESS);
      Wire.write((data << 4) | LIGHT | RS | EN );
      Wire.endTransmission();
      
      delayMicroseconds(1); // enable ativo >450ns
      
      Wire.beginTransmission(LCD_ADDRESS);
      Wire.write((data << 4) | LIGHT | RS);
      Wire.endTransmission();
      
      delayMicroseconds(40);
}

void display_write_command_4(byte data) {
      Wire.beginTransmission(LCD_ADDRESS);
      Wire.write((data << 4) | LIGHT );
      Wire.endTransmission();
      Wire.beginTransmission(LCD_ADDRESS);
      Wire.write((data << 4) | LIGHT | EN );
      Wire.endTransmission();
      delayMicroseconds(1); // enable ativo >450ns
      Wire.beginTransmission(LCD_ADDRESS);
      Wire.write((data << 4) | LIGHT);
      Wire.endTransmission();
      delayMicroseconds(40);
}


void display_write_command_8(byte data) {
      display_write_command_4(data >> 4);
      display_write_command_4(data);
}

void display_write_data_8(byte data) {
      display_write_data_4(data >> 4);
      display_write_data_4(data);
}

void display_initialize() {
      Wire.begin();
      delay(50);

      display_write_command_4(0x03);
      delay(5);
      display_write_command_4(0x03);

      delayMicroseconds(200);

      display_write_command_4(0x03);
      display_write_command_4(0x02);
      display_write_command_4(0x02);
      display_write_command_4(0x08);
   
      delayMicroseconds(120);
 
      display_write_command_4(0x00);
      display_write_command_4(0x0F);
 
      delayMicroseconds(120);
 
      display_write_command_4(0x08);
      display_write_command_4(0x00);
 
      delayMicroseconds(120);
}

void display_clear() {
      display_write_command_8(0x01);
      delay(5);
}

void display_set_cursor(byte line, byte column) {
      display_write_command_8(0x80 | line << 6 | column);
}

void display_print_char(char data) {
      display_write_data_8(data);
      delayMicroseconds(120);
}

void display_print_string(String data) {
      display_clear();
      display_set_cursor(0,0);
      for (int i = 0; i < data.length(); i++) { 
        if(i==16){
          display_set_cursor(1,0);
        }
          display_print_char(data[i]);
      }
  }
  
  void display_print_int(int data) {
      String s = String(data);
      display_print_string(s);
}

void setup() {
      Serial.begin(9600);
      display_initialize();
      display_clear();
      display_print_string("Hello world!    Skynet lives");
      delay(2000);
      display_print_string("something");

}

void loop() {
    

}

