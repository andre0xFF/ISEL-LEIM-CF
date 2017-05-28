public class CPU implements Runnable {

    /** * * * * * * * * * * * * * * * * * * * * * * * *
     * Java code
     ** * * * * * * * * * * * * * * * * * * * * * * * */

    public static void main(String[] args) {
        new CPU();
    }

    public CPU() {
        this.setup();
        new Thread(new CPU()).start();
    }

    @Override
    public void run() {
        this.loop();
    }

    static class Serial {
        static void begin(int i) {
           return;
        }

        public static void print(String s) {
            System.out.print(s);
        }

        public static void print(int s, int hex) {
            System.out.print((byte) s);
        }
    }

    int MCLK;
    int RISING;
    int FALLING;
    int HEX;

    void attachInterrupt(int i, int j, int k) {
        return;
    }

    void interrupts() {
        return;
    }

    long millis() {
        return System.currentTimeMillis();
    }

    /** * * * * * * * * * * * * * * * * * * * * * * * *
     * C code
     ** * * * * * * * * * * * * * * * * * * * * * * * */

    // Arithemtic unit
    boolean flag_carry;
    boolean flag_zero;
    int carry;

    // Program counter
    int pc_mux_y0;
    int pc_add_y0;
    int pc_mux_y1;
    int pc_reg_d0;
    int pc_reg_q0;

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

    boolean pc0_enable;

    // EPROM: There are a total of 0x03D addresses so the
    // proper size is 2^6 = 64
    final int EPROM_SIZE = 64;
    int eprom[] = new int[EPROM_SIZE];

    // Code memory: The JUMP instruction can have a max jump of
    // 0 1111 1111 = 0x0FF = 255 so the proper size is 2^8 = 256
    final int CODE_MEMORY_SIZE = 256;
    int code_memory[] = new int[CODE_MEMORY_SIZE];

    // Data memory: Arbitrary value (at the current moment)
    final int DATA_MEMORY_SIZE = 128;
    int data_memory[] = new int[DATA_MEMORY_SIZE];

    // Clock
    final int DEBOUNCE_TIME = 200;
    long time_pos;
    long time_neg;

    void setup() {
        Serial.begin(9600);
        initialize();
        attachInterrupt(0, MCLK, RISING);
        interrupts();
    }

    void initialize() {
        data_memory();
    }

    void loop() {

    }

    /** * * * * * * * * * * * * * * * * * * * * * * * *
     * Modules
     ** * * * * * * * * * * * * * * * * * * * * * * * */

    int MUX_2x1(boolean S, int A, int B) {
        return S ? A : B;
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

    void functional_module() {
        // TODO
    }

    void control_module() {
        // Control module input are the bits:
        // D8, D7, D6, D5, D1, D0 = 011100011 = 0x0E3
        int input = code_memory[pc_reg_q0] & 0x0E3;
        // TODO
    }

    void eprom() {
        fill(eprom, 0x00, 0x00, 0x20C); // MOV B, A
        fill(eprom, 0x01, 0x01, 0x50C); // ADDC A, B
        fill(eprom, 0x02, 0x02, 0x50C); // SUBB A, B
        fill(eprom, 0x03, 0x06, 0x00E); // JC rel5
        fill(eprom, 0x07, 0x0A, 0x00D); // JZ rel5
        fill(eprom, 0x0B, 0x0B, 0x428); // MOV A, @P
        fill(eprom, 0x0C, 0x0C, 0x004); // MOV @P, A
        fill(eprom, 0x0D, 0x0D, 0x08C); // MOV P, A
        fill(eprom, 0x0E, 0x1D, 0x04C); // JMP end7
        fill(eprom, 0x1E, 0x3D, 0x41C); // MOV A, #CONST
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

        time_pos = time;

        if (true) {
            pc_reg_q0 = pc_reg_d0;
        }

        Serial.print("0x0");
        Serial.print(pc_reg_q0, HEX);
    }

    void MCLK_negative() {
        long time = millis();

        if (time - time_neg < DEBOUNCE_TIME) {
            return;
        }

        time_neg = time;

        a_reg_q0 = register(a_enable, a_mux_y0, a_reg_q0);
        b_reg_q0 = register(b_enable, a_reg_q0, b_reg_q0);
        p_reg_q0 = register(p_enable, a_reg_q0, p_reg_q0);

        z_reg_q0 = register(f_enable, flag_zero ? 1 : 0, z_reg_q0);
        cy_reg_q0 = register(f_enable, flag_carry ? 1 : 0, cy_reg_q0);
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
