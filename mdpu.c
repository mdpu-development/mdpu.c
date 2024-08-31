#include <stdio.h>
#include <stdlib.h>

// Define the structure of the multi-dimensional processing unit
typedef struct {
    int **registers;
    int **memory;
    int num_registers_x;
    int num_registers_y;
    int memory_size_x;
    int memory_size_y;
    int stack_pointer_x;
    int stack_pointer_y;
} ProcessingUnit;

// Define the structure to hold the state after execution
typedef struct {
    int **registers;
    int **stack;
    int stack_size_x;
    int stack_size_y;
} ProcessingUnitState;

// Define opcodes
typedef enum {
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_STORE,
    OP_LOAD,
    OP_PUSH,
    OP_HALT
} Opcode;

// Define the structure of an instruction
typedef struct {
    Opcode opcode;
    int reg1_x;
    int reg1_y;
    int reg2_x;
    int reg2_y;
    int reg3_x;
    int reg3_y;
    int addr_x;
    int addr_y;
} Instruction;

// ++++++++++++++++++++++++++++++ Initialization ++++++++++++++++++++++++++++++ //
void initialize(ProcessingUnit *pu, int num_registers_x, int num_registers_y, int memory_size_x, int memory_size_y) {
    pu->num_registers_x = num_registers_x;
    pu->num_registers_y = num_registers_y;
    pu->memory_size_x = memory_size_x;
    pu->memory_size_y = memory_size_y;
    pu->registers = (int **)malloc(num_registers_x * sizeof(int *));
    pu->memory = (int **)malloc(memory_size_x * sizeof(int *));
    for (int i = 0; i < num_registers_x; i++) {
        pu->registers[i] = (int *)malloc(num_registers_y * sizeof(int));
    }
    for (int i = 0; i < memory_size_x; i++) {
        pu->memory[i] = (int *)malloc(memory_size_y * sizeof(int));
    }
    pu->stack_pointer_x = memory_size_x - 1; // Initialize stack pointer to the top of the memory
    pu->stack_pointer_y = memory_size_y - 1; // Initialize stack pointer to the top of the memory
    if (pu->registers == NULL || pu->memory == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < num_registers_x; i++) {
        for (int j = 0; j < num_registers_y; j++) {
            pu->registers[i][j] = 0;
        }
    }
    for (int i = 0; i < memory_size_x; i++) {
        for (int j = 0; j < memory_size_y; j++) {
            pu->memory[i][j] = 0;
        }
    }
}

// ++++++++++++++++++++++++++++++ Arithmetic operations ++++++++++++++++++++++++++++++ //
void add(ProcessingUnit *pu, int reg1_x, int reg1_y, int reg2_x, int reg2_y, int reg3_x, int reg3_y) {
    pu->registers[reg3_x][reg3_y] = pu->registers[reg1_x][reg1_y] + pu->registers[reg2_x][reg2_y];
}

void subtract(ProcessingUnit *pu, int reg1_x, int reg1_y, int reg2_x, int reg2_y, int reg3_x, int reg3_y) {
    pu->registers[reg3_x][reg3_y] = pu->registers[reg1_x][reg1_y] - pu->registers[reg2_x][reg2_y];
}

void multiply(ProcessingUnit *pu, int reg1_x, int reg1_y, int reg2_x, int reg2_y, int reg3_x, int reg3_y) {
    pu->registers[reg3_x][reg3_y] = pu->registers[reg1_x][reg1_y] * pu->registers[reg2_x][reg2_y];
}

void divide(ProcessingUnit *pu, int reg1_x, int reg1_y, int reg2_x, int reg2_y, int reg3_x, int reg3_y) {
    if (pu->registers[reg2_x][reg2_y] != 0) {
        pu->registers[reg3_x][reg3_y] = pu->registers[reg1_x][reg1_y] / pu->registers[reg2_x][reg2_y];
    } else {
        printf("Error: Division by zero\n");
    }
}

// ++++++++++++++++++++++++++++++ Memory operations ++++++++++++++++++++++++++++++ //
void store(ProcessingUnit *pu, int reg_x, int reg_y, int addr_x, int addr_y) {
    pu->memory[addr_x][addr_y] = pu->registers[reg_x][reg_y];
}

void load(ProcessingUnit *pu, int addr_x, int addr_y, int reg_x, int reg_y) {
    pu->registers[reg_x][reg_y] = pu->memory[addr_x][addr_y];
}

// ++++++++++++++++++++++++++++++ Stack operations ++++++++++++++++++++++++++++++ //
void push(ProcessingUnit *pu, int reg_x, int reg_y) {
    if (pu->stack_pointer_x >= 0 && pu->stack_pointer_y >= 0) {
        pu->memory[pu->stack_pointer_x][pu->stack_pointer_y] = pu->registers[reg_x][reg_y];
        pu->stack_pointer_y--;
        if (pu->stack_pointer_y < 0) {
            pu->stack_pointer_y = pu->memory_size_y - 1;
            pu->stack_pointer_x--;
        }
    } else {
        printf("Error: Stack overflow\n");
    }
}

// ++++++++++++++++++++++++++++++ Program execution ++++++++++++++++++++++++++++++ //
void execute_program(ProcessingUnit *pu, Instruction *program, int program_size) {
    for (int i = 0; i < program_size; i++) {
        Instruction instr = program[i];
        switch (instr.opcode) {
            case OP_ADD:
                add(pu, instr.reg1_x, instr.reg1_y, instr.reg2_x, instr.reg2_y, instr.reg3_x, instr.reg3_y);
                break;
            case OP_SUBTRACT:
                subtract(pu, instr.reg1_x, instr.reg1_y, instr.reg2_x, instr.reg2_y, instr.reg3_x, instr.reg3_y);
                break;
            case OP_MULTIPLY:
                multiply(pu, instr.reg1_x, instr.reg1_y, instr.reg2_x, instr.reg2_y, instr.reg3_x, instr.reg3_y);
                break;
            case OP_DIVIDE:
                divide(pu, instr.reg1_x, instr.reg1_y, instr.reg2_x, instr.reg2_y, instr.reg3_x, instr.reg3_y);
                break;
            case OP_STORE:
                store(pu, instr.reg1_x, instr.reg1_y, instr.addr_x, instr.addr_y);
                break;
            case OP_LOAD:
                load(pu, instr.addr_x, instr.addr_y, instr.reg1_x, instr.reg1_y);
                break;
            case OP_PUSH:
                push(pu, instr.reg1_x, instr.reg1_y);
                break;
            case OP_HALT:
                return;
            default:
                printf("Unknown opcode: %d\n", instr.opcode);
                return;
        }
    }
}

// ++++++++++++++++++++++++++++++ Example usage ++++++++++++++++++++++++++++++ //
ProcessingUnitState run(ProcessingUnit *pu, Instruction *program, int program_size) {
    // Initialize registers for the example
    pu->registers[0][0] = 10;
    pu->registers[0][1] = 20;

    // Execute the program
    execute_program(pu, program, program_size);

    // Prepare the state to return
    ProcessingUnitState state;
    state.registers = (int **)malloc(pu->num_registers_x * sizeof(int *));
    for (int i = 0; i < pu->num_registers_x; i++) {
        state.registers[i] = (int *)malloc(pu->num_registers_y * sizeof(int));
    }
    state.stack_size_x = pu->memory_size_x - pu->stack_pointer_x - 1;
    state.stack_size_y = pu->memory_size_y - pu->stack_pointer_y - 1;
    state.stack = (int **)malloc(state.stack_size_x * sizeof(int *));
    for (int i = 0; i < state.stack_size_x; i++) {
        state.stack[i] = (int *)malloc(state.stack_size_y * sizeof(int));
    }

    for (int i = 0; i < pu->num_registers_x; i++) {
        for (int j = 0; j < pu->num_registers_y; j++) {
            state.registers[i][j] = pu->registers[i][j];
        }
    }
    for (int i = 0; i < state.stack_size_x; i++) {
        for (int j = 0; j < state.stack_size_y; j++) {
            state.stack[i][j] = pu->memory[pu->stack_pointer_x + 1 + i][pu->stack_pointer_y + 1 + j];
        }
    }

    return state;
}

int main() {
    ProcessingUnit pu;
    initialize(&pu, 4, 4, 10, 10);

    Instruction program[] = {
        {OP_ADD, 0, 0, 0, 1, 1, 0, 0, 0},
        {OP_SUBTRACT, 0, 0, 0, 1, 1, 1, 0, 1},
        {OP_MULTIPLY, 0, 0, 0, 1, 1, 2, 0, 2},
        {OP_DIVIDE, 0, 0, 0, 1, 1, 3, 0, 3},
        {OP_STORE, 0, 0, 0, 0, 0, 0, 0, 4},
        {OP_LOAD, 0, 0, 0, 0, 1, 0, 0, 4},
        {OP_PUSH, 0, 0, 0, 0, 0, 0, 0, 0},
        {OP_PUSH, 0, 1, 0, 1, 0, 0, 0, 0},
        {OP_PUSH, 0, 2, 0, 2, 0, 0, 0, 0},
        {OP_PUSH, 0, 3, 0, 3, 0, 0, 0, 0},
        {OP_HALT, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    int program_size = sizeof(program) / sizeof(program[0]);

    ProcessingUnitState state = run(&pu, program, program_size);

    printf("MDPU shape:\n");
    printf("-- Registers: %dx%d\n", pu.num_registers_x, pu.num_registers_y);
    printf("-- Memory: %dx%d\n\n", pu.memory_size_x, pu.memory_size_y);

    printf("Registers:\n");

    for (int i = 0; i < pu.num_registers_x; i++) {
        printf("-- ");
        for (int j = 0; j < pu.num_registers_y; j++) {
            printf("%d ", state.registers[i][j]);
        }
        printf("\n\n");
    }

    printf("Stack:\n");

    for (int i = 0; i < state.stack_size_x; i++) {
        printf("-- ");
        for (int j = 0; j < state.stack_size_y; j++) {
            printf("%d ", state.stack[i][j]);
        }
        printf("\n");
    }

    return 0;
}