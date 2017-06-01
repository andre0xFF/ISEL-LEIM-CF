import java.util.Scanner;

public class CPU implements Runnable {

    /** * * * * * * * * * * * * * * * * * * * * * * * *
     * Java code
     ** * * * * * * * * * * * * * * * * * * * * * * * */

    public static void main(String[] args) {
        CPU cpu = new CPU();

        Scanner scanner = new Scanner(System.in);
        boolean clock = false;

        while (true) {
            String input = scanner.nextLine();

            System.out.println("Clock: " + clock);

            if (input.equals("") && !clock) {
                cpu.MCLK_negative();
                clock = true;
                continue;
            }

            if (input.equals("") && clock) {
                cpu.MCLK_positive();
                clock = false;
                continue;
            }

        }
    }

    public CPU() {
        this.setup();
        new Thread(this).start();
    }

    @Override
    public void run() {
        while(true) {
            this.loop();
        }
    }

    static class Serial {
        static void begin(int i) {
           return;
        }

        static void print(String s) {
            System.out.print(s);
        }

        static void print(int s, int hex) {
            System.out.print((byte) s);
        }

        static boolean available() {
            return false;
        }
    }

    int MCLK_positive;
    int MCLK_negative;
    boolean RISING = true;
    boolean FALLING = false;
    int HEX;

    void attachInterrupt(int i, int j, boolean k) {
        if (k) {
            this.MCLK_positive();
        }

        if (!k) {
            this.MCLK_negative();
        }
    }

    void interrupts() {
        return;
    }

    long millis() {
        return System.currentTimeMillis();
    }

    void read_input() { }

    int digitalPinToInterrupt(int i) { return 0; }

    /** * * * * * * * * * * * * * * * * * * * * * * * *
     * C code
     ** * * * * * * * * * * * * * * * * * * * * * * * */

    // Arithemtic unit
    boolean flag_carry;
    boolean flag_zero;
    int alu_r;
    int alu_c;
    int alu_z;

    // Program counter
    int pc_mux_y0;
    int pc_add_y0;
    int pc_mux_y1;
    int pc_reg_d0;
    volatile int pc_reg_q0;
    boolean pc0_enable;

    // Registers
    int a_mux_y0;
    int a_reg_d0;
    int a_reg_q0;
    int b_reg_d0;
    int b_reg_q0;
    int p_reg_d0;
    int p_reg_q0;
    int z_reg_d0;
    int z_reg_q0;
    int cy_reg_d0;
    int cy_reg_q0;

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

    // EPROM: There are a total of 0x054 = 84 addresses so the
    // proper size is 2^8 = 256
    final int EPROM_SIZE = 256;
    int eprom[] = new int[EPROM_SIZE];

    // Code memory: The JUMP instruction can have a max jump of
    // 0 1111 1111 = 0x0FF = 255 so the proper size is 2^8 = 256
    final int CODE_MEMORY_SIZE = 256;
    int code_memory[] = new int[CODE_MEMORY_SIZE];
    int data_memory_db;

    // Data memory: Max bit in Address Bus is 9 so 2^9 = 512
    final int DATA_MEMORY_SIZE = 512;
    int data_memory[] = new int[DATA_MEMORY_SIZE];
    int code_memory_db;

    // Clock
    final int DEBOUNCE_TIME = 200;
    long time_pos;
    long time_neg;

    void setup() {
        Serial.begin(9600);
        initialize();
        attachInterrupt(0, MCLK_negative, FALLING);
        interrupts();
    }

    void initialize() {
        eprom();
        data_memory();
        code_memory_test();
    }

    void loop() {
        functional_module();
        control_module();

        if (Serial.available()) {
            read_input();
        }
    }

    /** * * * * * * * * * * * * * * * * * * * * * * * *
     * Modules
     ** * * * * * * * * * * * * * * * * * * * * * * * */

    int MUX_2x1(boolean S, int A, int B) {
        return S ? B : A;
    }

    int MUX_4x1(boolean S1, boolean S0, int M3, int M2, int M1, int M0) {
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

        // TODO remove this return when we convert the code to C
        return 0;
    }

    int add(int A, int B) {
        // TODO remove cast
        return (int)(A + B);
    }

    int sub(int A, int B) {
        // TODO remove cast
        return (int)(A - B);
    }

    int register(boolean E, int D, int Q) {
        return E ? D : Q;
    }

    // Arithmetic and Logic Unit
    int alu(boolean Sel, int X, int Y, int carry_in) {
        // TODO remove (= 0)
        int result = 0;

        if (Sel) {
            int aux = add(X, Y);
            result = add(aux, carry_in);
        }
        else {
            int aux = sub(X, Y);
            result = sub(aux, carry_in);
        }

        flag_zero = result == 0;
        flag_carry = result > 255;

        return result;
    }

    boolean x_module(boolean jc, boolean c, boolean jz, boolean z) {
        return jc & c | jz & z;
    }

    void code_memory(int address_bus) {
        code_memory_db = code_memory[address_bus];
    }

    void data_memory(int address_bus, boolean output_enable, boolean write_enable) {
        if (!write_enable) {
            data_memory_db = a_reg_q0;
        }

        if (!output_enable) {
            data_memory_db = data_memory[address_bus];
        }
    }

    void functional_module() {
        // Code memory
        code_memory(pc_reg_q0);

        // Data memory
        data_memory(p_reg_q0, read, write);

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
        alu_r = alu((code_memory_db & 0x01) == 1 ? true : false, a_reg_q0, b_reg_q0, alu_c);

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

    boolean read_bit(int bits, int n) {
        // Shift n positions to the right
        bits = bits >> n;
        // Filter the last bit of the right
        bits = bits & 0x01;
        return bits == 0x01 ? true : false;
    }

    void eprom() {
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

    void fill(int[] array, int from, int to, int value) {
        for (int i = from; i <= to; i++) {
            array[i] = value;
        }
    }

    void data_memory() {
        data_memory[0x10] = 0x004;
        data_memory[0x20] = 0x008;
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

        Serial.print("0x0");
        Serial.print(pc_reg_q0, HEX);

//        attachInterrupt(digitalPinToInterrupt(2), MCLK_negative, FALLING);
        time_pos = time;
    }

    void MCLK_negative() {
        long time = millis();

        if (time - time_neg < DEBOUNCE_TIME) {
            return;
        }

        a_reg_q0 = register(a_enable, a_mux_y0, a_reg_q0);
        b_reg_q0 = register(b_enable, a_reg_q0, b_reg_q0);
        p_reg_q0 = register(p_enable, a_reg_q0, p_reg_q0);

        z_reg_q0 = register(f_enable, flag_zero ? 1 : 0, z_reg_q0);
        cy_reg_q0 = register(f_enable, flag_carry ? 1 : 0, cy_reg_q0);

//        attachInterrupt(digitalPinToInterrupt(2), MCLK_positive, RISING);
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

        // Instructions codification with parameters values:
        // MOV A, #CONST    > 1 1110 1011 = 0x1EB
        // JC rel5          > 0 0010 0101 = 0x025
        // JZ rel5          > 0 0100 0101 = 0x045
        // JMP end7         > 0 1000 1001 = 0x089

        // The last instruction is a JMP to the same address: HALT

        // Instructions:
        code_memory[0x00] = 0x1EB; // MOV A, #CONST
        code_memory[0x01] = 0x060; // MOV A, @P
        code_memory[0x02] = 0x061; // MOV @P, A
        code_memory[0x03] = 0x062; // MOV P, A
        code_memory[0x04] = 0x000; // MOV B, A
        code_memory[0x05] = 0x001; // ADDC A, B
        code_memory[0x06] = 0x002; // SUBB A, B
        code_memory[0x07] = 0x025; // JC rel5
        code_memory[0x08] = 0x045; // JZ rel5
        code_memory[0x09] = 0x089; // JMP end7
    }

    void program1() {
//        code_memory[0x00] = ;
//        code_memory[0x01] = ;
//        code_memory[0x02] = ;
//        code_memory[0x03] = ;
//        code_memory[0x04] = ;
//        code_memory[0x05] = 0x05;
    }
}
