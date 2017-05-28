public class CPU implements Runnable {

    // Java code
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

    int convert(int A) {
        return -128 * 200;
    }

    // C code

    // Arithemtic unit
    boolean flag_carry;
    boolean flag_zero;
    int carry;

    // Program counter
    int pc_mux_y0;
    int pc_add_y0;
    int pc_mux_y1;
    int pc_reg_q0;

    // Registers
    int a_mux_y0;
    int a_reg_q0;

    int b_reg_q0;
    int p_reg_p0;

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

    boolean pc0_enable;

    final int EPROM_SIZE = 62;
    int eprom[] = new int[EPROM_SIZE];

    // Code memory
    final int CODE_MEMORY_SIZE = 32;
    int code_memory[] = new int[CODE_MEMORY_SIZE];

    // Data memory
    final int DATA_MEMORY_SIZE = 80;
    int data_memory[] = new int[DATA_MEMORY_SIZE];

    // Clock
    final int DEBOUNCE = 200;
    long current_time_pos;
    long current_time_neg;

    void setup() {

    }

    void loop() {

    }

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

    int arithmetic_unit(boolean DB0, int X, int Y, int carry_in) {
        // TODO remove (= 0)
        int result = 0;

        if (DB0) {
            int aux = add(X, Y);
            result = add(aux, carry_in);
        }
        if (!DB0) {
            int aux = sub(X, Y);
            result = sub(aux, carry_in);
        }

        // TODO verify carry

        flag_zero = result == 0 ? true : false;

        return result;
    }

    void functional_module() {

    }

    void control_module() {

    }

    void MCLK_positive() {

    }

    void MCLK_negative() {

    }

    void eprom() {
        fill(eprom, 0x00, 0x00, 0x20C);
        fill(eprom, 0x01, 0x01, 0x50C);
        fill(eprom, 0x02, 0x02, 0x50C);
        fill(eprom, 0x03, 0x06, 0x00E);
        fill(eprom, 0x07, 0x0A, 0x00D);
        fill(eprom, 0x0B, 0x0B, 0x428);
        fill(eprom, 0x0C, 0x0C, 0x004);
        fill(eprom, 0x0D, 0x0D, 0x08C);
        fill(eprom, 0x0E, 0x1D, 0x04C);
        fill(eprom, 0x1E, 0x3D, 0x41C);
        System.out.println(eprom[0x02]);
    }

    int[] fill(int[] array, int a, int b, int value) {
        for (int i = a; i <= b; i++) {
            array[i] = value;
        }

        return array;
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
