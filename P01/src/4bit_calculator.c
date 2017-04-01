#include<stdio.h>
#include <stdbool.h>

// Pin numbers
#define PIN_A1 0
#define PIN_A0 0
#define PIN_B1 0
#define PIN_B0 0
#define PIN_C2 0
#define PIN_C1 0
#define PIN_C0 0

// Input variables
bool _A1;
bool _A0;
bool _B1;
bool _B0;

// Operation selectors
bool _C2;
bool _C1;
bool _C0;

// Output variables
bool _Y3;
bool _Y2;
bool _Y1;
bool _Y0;

// Functions declarations
void read_inputs();
void calculate();
void write_outputs();
void loop();

int main() {
    printf("Starting Arduino simulation\n");

    while(true) {
        loop();
    }
}

void read_inputs() {

}

void calculate() {

}

void write_outputs() {

}

void loop() {
    read_inputs();
    calculate();
    write_outputs();

}
