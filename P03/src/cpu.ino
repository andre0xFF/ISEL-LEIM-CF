// EPROM: There are a total of 0x3F addresses so the
// proper size is 2^4 = 64
#define EPROM_SIZE 64

// Code memory: The JUMP instruction can have a max jump of
// 0 1111 1111 = 0x0FF = 255 so the proper size is 2^8 = 256
#define CODE_MEMORY_SIZE 256

// Data memory: Max bit in Address Bus is 9 so 2^9 = 512
#define DATA_MEMORY_SIZE 512

#define DEBOUNCE_TIME 200

// Arithemtic unit
boolean flag_carry;
boolean flag_zero;
byte alu_r;
byte alu_c;
byte alu_z;

// Program counter
byte pc_mux_y0;
byte pc_add_y0;
byte pc_mux_y1;
byte pc_reg_d0;
volatile byte pc_reg_q0;
boolean pc0_enable;

// Registers
byte a_mux_y0;
byte a_reg_d0;
byte a_reg_q0;
byte b_reg_d0;
volatile byte b_reg_q0;
byte p_reg_d0;
volatile byte p_reg_q0;
byte z_reg_d0;
byte z_reg_q0;
byte cy_reg_d0;
volatile byte cy_reg_q0;

// Control module
boolean a_enable;
boolean b_enable;
boolean f_enable;
boolean p_enable;

boolean pc1_enable;
boolean pc2_enable;
boolean pc3_enable;
boolean write;
boolean read;
boolean jump_carry;
boolean jump_zero;

// Blocks
word eprom[EPROM_SIZE];

word code_memory[CODE_MEMORY_SIZE];
word code_memory_db;

byte data_memory[DATA_MEMORY_SIZE];
byte data_memory_db;

// Clock
long time_pos;
long time_neg;

void setup() {
    Serial.begin(9600);
    initialize();
    attachInterrupt(digitalPinToInterrupt(2), MCLK_negative, FALLING);
    interrupts();
}

void initialize() {
    fill_eprom();
    fill_data_memory();
    code_memory_test();
    print_instruction();
}

void loop() {
    functional_module();
    control_module();

    if (Serial.available()) {
        read_input();
    }
}

void read_input() {
    switch (Serial.read()) {
        case 'C':
            return print_control_module();
        case 'F':
            return print_functional_module();
        case 'M':
            return print_memory();
    }
}

void print_control_module() {
    Serial.print(" > C: ");
    Serial.print(" EnA ");
    Serial.print(a_enable, HEX);
    Serial.print(" EnB ");
    Serial.print(b_enable, HEX);
    Serial.print(" EnF ");
    Serial.print(f_enable, HEX);
    Serial.print(" EnP ");
    Serial.print(p_enable, HEX);
    Serial.print(" PC1 ");
    Serial.print(pc1_enable, HEX);
    Serial.print(" PC2 ");
    Serial.print(pc2_enable, HEX);
    Serial.print(" PC3 ");
    Serial.print(pc3_enable, HEX);
    Serial.print(" /WR ");
    Serial.print(write, HEX);
    Serial.print(" /RD ");
    Serial.print(read, HEX);
    Serial.print(" JC ");
    Serial.print(jump_carry, HEX);
    Serial.print(" JZ ");
    Serial.print(jump_zero, HEX);
    Serial.println();
}

void print_functional_module() {

}

void print_memory() {
    Serial.print(" > Reg: ");
    Serial.print(" PC_Q ");
    Serial.print(pc_reg_q0, HEX);
    Serial.print(" A_Q ");
    Serial.print(a_reg_q0, HEX);
    Serial.print(" B_Q ");
    Serial.print(b_reg_q0, HEX);
    Serial.print(" P_Q ");
    Serial.print(p_reg_q0, HEX);
    Serial.print(" Cy_Q ");
    Serial.print(flag_carry, HEX);
    Serial.print(" Z_Q ");
    Serial.print(flag_zero, HEX);
    Serial.println();
}

void print_instruction() {
    word instruction = code_memory[pc_reg_q0];

    Serial.print("0x0");
    Serial.print(pc_reg_q0);
    Serial.print(": ");

    boolean D8 = read_bit(instruction, 8);
    boolean D7 = read_bit(instruction, 7);
    boolean D6 = read_bit(instruction, 6);
    boolean D5 = read_bit(instruction, 5);
    boolean D1 = read_bit(instruction, 1);
    boolean D0 = read_bit(instruction, 0);

    if (D8) {
        Serial.print("MOV A, #CONST");
    }
    else if (D7) {
        Serial.print("JMP end7");
    }
    // else if (D6 & D5 & D1) {
    //     Serial.print("MOV P, A");
    // }
    // else if (D6 & D5 & D0) {
    //     Serial.print("MOV @P, A");
    // }
    // else if (D6 & D5) {
    //     Serial.print("MOV A, @P");
    // }
    // else if (D6) {
    //     Serial.print("JZ rel5");
    // }
    // else if (D5) {
    //     Serial.print("JC rel5");
    // }
    // else if (D1) {
    //     Serial.print("SUBB A, B");
    // }
    // else if (D0) {
    //     Serial.print("ADDC A, B");
    // }
    // else {
    //     Serial.print("MOV B, A");
    // }

    Serial.print(" > ");
    Serial.print(instruction, BIN);
    Serial.println();
}

/** * * * * * * * * * * * * * * * * * * * * * * * *
 * Modules
 ** * * * * * * * * * * * * * * * * * * * * * * * */

byte MUX_2x1(boolean S, byte A, byte B) {
    return S ? B : A;
}

byte MUX_4x1(boolean S1, boolean S0, byte M3, byte M2, byte M1, byte M0) {
    if (!S1 & !S0) {
        return M0;
    }
    if (!S1 & S0) {
        return M1;
    }
    if (S1 & !S0) {
        return M2;
    }
    if (S1 & S0) {
        return M3;
    }
}

byte add(byte A, byte B) {
    return A + B;
}

byte sub(byte A, byte B) {
    return A - B;
}

byte register_memory(boolean E, byte D, byte Q) {
    return E ? D : Q;
}

// Arithmetic and Logic Unit
byte alu(boolean Sel, byte X, byte Y, byte carry_in) {
    byte result;

    if (Sel) {
        byte aux = add(X, Y);
        result = add(aux, carry_in);
        flag_zero = (byte) (X - Y - carry_in) == 0;
        flag_carry = (int) (X - Y - carry_in) > 255;
    }
    else {
        byte aux = sub(X, Y);
        result = sub(aux, carry_in);
        flag_zero = (byte) (X + Y + carry_in) == 0;
        flag_carry = (int) (X + Y + carry_in) > 255;
    }

    return result;
}

boolean x_module(boolean jc, boolean c, boolean jz, boolean z) {
    return jc & c | jz & z;
}

void code_memory_block(byte address_bus) {
    code_memory_db = code_memory[address_bus];
}

void data_memory_block(byte address_bus, boolean output_enable, boolean write_enable) {
    if (!write_enable) {
        data_memory_db = a_reg_q0;
    }

    if (!output_enable) {
        data_memory_db = data_memory[address_bus];
    }
}

void functional_module() {
    // Code memory
    code_memory_block(pc_reg_q0);

    // Data memory
    data_memory_block(p_reg_q0, read, write);

    // Program counter
    // Mask bits D4, D3, D2, D1, D0
    pc_mux_y0 = MUX_2x1(pc0_enable, 1, code_memory_db & 0x01F);
    pc_add_y0 = add(pc_mux_y0, pc_reg_q0);
    pc_mux_y1 = MUX_2x1(pc1_enable, pc_add_y0, code_memory_db);
    pc_reg_d0 = pc_mux_y1;

    // A, B, P registers
    // Mask bits D7, D6, D5, D4, D3, D2, D1, D0
    a_mux_y0 = MUX_4x1(pc3_enable, pc2_enable, 0, code_memory_db & 0x0FF, data_memory_db, alu_r);
    a_reg_d0 = a_mux_y0;
    b_reg_d0 = a_reg_q0;
    p_reg_d0 = a_reg_q0;

    // ALU
    alu_r = alu((code_memory_db & 0x01) == 1, a_reg_q0, b_reg_q0, alu_c);

    // X module
    pc0_enable = x_module(jump_carry, flag_carry, jump_zero, flag_zero);
}

void control_module() {
    // Control module input are the bits:
    // D8 D7 D6 D5 !D4 !D3 !D2 D1 D0 = 1 1110 0011 = 0x1E3
    int input_address = code_memory_db & 0x1E3;

    // Form a new address with just
    // D8 D7 D6 D5 D1 D0 bits
    // just like in EPROM table.
    input_address = (
            (input_address & 0x100) >> 3 |
            (input_address & 0x080) >> 3 |
            (input_address & 0x040) >> 3 |
            (input_address & 0x020) >> 3|
            input_address & 0x002 |
            input_address & 0x001
    );

    // input_address = (
    //         input_address & 0x1E0 >> 3 |
    //         input_address & 0x002 |
    //         input_address & 0x001
    // );

    // Read the data at the eprom input address
    int data = eprom[input_address];

    // Read individual bits of data
    a_enable = read_bit(data, 10);
    b_enable = read_bit(data, 9);
    f_enable = read_bit(data, 8);
    p_enable = read_bit(data, 7);
    pc1_enable = read_bit(data, 6);
    pc2_enable = read_bit(data, 5);
    pc3_enable = read_bit(data, 4);
    write = read_bit(data, 3);
    read = read_bit(data, 2);
    jump_carry = read_bit(data, 1);
    jump_zero = read_bit(data, 0);
}

boolean read_bit(byte bits, byte n) {
    // Shift n positions to the right
    bits = bits >> n;
    // Filter the last bit of the right
    bits = bits & 0x01;
    return bits == 0x01;
}

void fill_eprom() {
    fill(eprom, 0x00, 0x00, 0x20C); // MOV B, A
    fill(eprom, 0x01, 0x01, 0x50C); // ADDC A, B
    fill(eprom, 0x02, 0x02, 0x50C); // SUBB A, B
    fill(eprom, 0x04, 0x07, 0x00E); // JC rel5
    fill(eprom, 0x08, 0x0B, 0x00D); // JZ rel5
    fill(eprom, 0x0C, 0x0C, 0x428); // MOV A, @P
    fill(eprom, 0x0D, 0x0D, 0x004); // MOV @P, A
    fill(eprom, 0x0E, 0x0E, 0x08C); // MOV P, A
    fill(eprom, 0x10, 0x1F, 0x04C); // JMP end7
    fill(eprom, 0x20, 0x3F, 0x41C); // MOV A, #CONST
}

void fill(word array[], word from, word to, word value) {
    for (int i = from; i <= to; i++) {
        array[i] = value;
    }
}

void fill_data_memory() {
    for (int i = 0x00; i < DATA_MEMORY_SIZE; i++) {
        data_memory[i] = random(0x00, 0x0FF);
    }
}

/** * * * * * * * * * * * * * * * * * * * * * * * *
 * Clock
 ** * * * * * * * * * * * * * * * * * * * * * * * */

void MCLK_positive() {
    long time = millis();

    if (time - time_pos < DEBOUNCE_TIME) {
        return;
    }

    if (true) {
        pc_reg_q0 = pc_reg_d0;
    }

    print_instruction();

    attachInterrupt(digitalPinToInterrupt(2), MCLK_negative, FALLING);
    time_pos = time;
}

void MCLK_negative() {
    long time = millis();

    if (time - time_neg < DEBOUNCE_TIME) {
        return;
    }

    a_reg_q0 = register_memory(a_enable, a_mux_y0, a_reg_q0);
    b_reg_q0 = register_memory(b_enable, a_reg_q0, b_reg_q0);
    p_reg_q0 = register_memory(p_enable, a_reg_q0, p_reg_q0);

    z_reg_q0 = register_memory(f_enable, flag_zero, z_reg_q0);
    cy_reg_q0 = register_memory(f_enable, flag_carry, cy_reg_q0);

    attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
    time_neg = time;
}

/** * * * * * * * * * * * * * * * * * * * * * * * *
 * Programs
 ** * * * * * * * * * * * * * * * * * * * * * * * */

void code_memory_test() {
    // Instructions from codification table

    // Arbitrary parameters values for test purposes:
    // Dashes (-) are the instructions bits
    // #CONST           = - 1110 1011 = 0x0EB = 235
    // rel5             = - --00 0101 = 0x005 = 005
    // end7             = - -000 1001 = 0x009 = 009

    // end7             = - -000 1000 = 0x008 = 008

    // Instructions codification with parameters values:
    // MOV A, #CONST    > 1 1110 1011 = 0x1EB
    // JC rel5          > 0 0010 0101 = 0x025
    // JZ rel5          > 0 0100 0101 = 0x045
    // JMP end7         > 0 1000 1001 = 0x089

    // JMP end7         > 0 1000 1000 = 0x088

    // The last instruction is a JMP to the same address: HALT

    // Instructions:
    code_memory[0x00] = 0x1EB; // MOV A, #CONST
    code_memory[0x01] = 0x061; // MOV @P, A
    code_memory[0x02] = 0x060; // MOV A, @P
    code_memory[0x03] = 0x062; // MOV P, A
    code_memory[0x04] = 0x000; // MOV B, A
    code_memory[0x05] = 0x001; // ADDC A, B
    code_memory[0x06] = 0x002; // SUBB A, B
    code_memory[0x07] = 0x025; // JC rel5
    code_memory[0x08] = 0x045; // JZ rel5
    code_memory[0x09] = 0x089; // JMP end7
    // Jumps to go back
    code_memory[0x12] = 0x088; // JMP end7
    code_memory[0x13] = 0x089; // JMP end7
}
