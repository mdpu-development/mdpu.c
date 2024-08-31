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
    OP_POP,
    OP_JMP,
    OP_JZ,
    OP_JNZ,
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

// Function to initialize the processing unit
void initialize(ProcessingUnit *pu, int num_registers_x, int num_registers_y, int memory_size_x, int memory_size_y) {
    pu->num_registers_x = num_registers_x;
    pu->num_registers_y = num_registers_y;
    pu->memory_size_x = memory_size_x;
    pu->memory_size_y = memory_size_y;
    
    pu->registers = (int **)malloc(num_registers_x * sizeof(int *));
    if (pu->registers == NULL) {
        printf("Memory allocation failed for registers\n");
        exit(1);
    }
    
    for (int i = 0; i < num_registers_x; i++) {
        pu->registers[i] = (int *)malloc(num_registers_y * sizeof(int));
        if (pu->registers[i] == NULL) {
            printf("Memory allocation failed for registers row %d\n", i);
            exit(1);
        }
    }
    
    pu->memory = (int **)malloc(memory_size_x * sizeof(int *));
    if (pu->memory == NULL) {
        printf("Memory allocation failed for memory\n");
        exit(1);
    }
    
    for (int i = 0; i < memory_size_x; i++) {
        pu->memory[i] = (int *)malloc(memory_size_y * sizeof(int));
        if (pu->memory[i] == NULL) {
            printf("Memory allocation failed for memory row %d\n", i);
            exit(1);
        }
    }

    pu->stack_pointer_x = memory_size_x - 1; // Initialize stack pointer to the top of the memory
    pu->stack_pointer_y = memory_size_y - 1; // Initialize stack pointer to the top of the memory

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

// Function to free the memory allocated for the processing unit
void free_processing_unit(ProcessingUnit *pu) {
    if (pu->registers != NULL) {
        for (int i = 0; i < pu->num_registers_x; i++) {
            if (pu->registers[i] != NULL) {
                free(pu->registers[i]);
            }
        }
        free(pu->registers);
    }
    
    if (pu->memory != NULL) {
        for (int i = 0; i < pu->memory_size_x; i++) {
            if (pu->memory[i] != NULL) {
                free(pu->memory[i]);
            }
        }
        free(pu->memory);
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

void pop(ProcessingUnit *pu, int reg_x, int reg_y) {
    if (pu->stack_pointer_x < pu->memory_size_x - 1 && pu->stack_pointer_y < pu->memory_size_y - 1) {
        pu->stack_pointer_y++;
        if (pu->stack_pointer_y >= pu->memory_size_y) {
            pu->stack_pointer_y = 0;
            pu->stack_pointer_x++;
        }
        pu->registers[reg_x][reg_y] = pu->memory[pu->stack_pointer_x][pu->stack_pointer_y];
    } else {
        printf("Error: Stack underflow\n");
    }
}

// ++++++++++++++++++++++++++++++ Jump operations ++++++++++++++++++++++++++++++ //
void jmp(int *instruction_pointer, int addr) {
    *instruction_pointer = addr;
}

void jz(ProcessingUnit *pu, int *instruction_pointer, int reg_x, int reg_y, int addr) {
    if (pu->registers[reg_x][reg_y] == 0) {
        *instruction_pointer = addr;
    }
}

void jnz(ProcessingUnit *pu, int *instruction_pointer, int reg_x, int reg_y, int addr) {
    if (pu->registers[reg_x][reg_y] != 0) {
        *instruction_pointer = addr;
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
            case OP_POP:
                pop(pu, instr.reg1_x, instr.reg1_y);
                break;
            case OP_JMP:
                jmp(&i, instr.addr_x);
                break;
            case OP_JZ:
                jz(pu, &i, instr.reg1_x, instr.reg1_y, instr.addr_x);
                break;
            case OP_JNZ:
                jnz(pu, &i, instr.reg1_x, instr.reg1_y, instr.addr_x);
                break;
            case OP_HALT:
                return;
        }
    }
}

// Function to free the memory allocated for the state
void free_processing_unit_state(ProcessingUnitState *state) {
    if (state->registers != NULL) {
        for (int i = 0; i < state->stack_size_x; i++) {
            if (state->registers[i] != NULL) {
                free(state->registers[i]);
            }
        }
        free(state->registers);
    }
    
    if (state->stack != NULL) {
        for (int i = 0; i < state->stack_size_x; i++) {
            if (state->stack[i] != NULL) {
                free(state->stack[i]);
            }
        }
        free(state->stack);
    }
}

// Function to run the program and return the state
ProcessingUnitState run(ProcessingUnit *pu, Instruction *program, int program_size) {
    pu->registers[0][0] = 10;
    pu->registers[0][1] = 20;

    execute_program(pu, program, program_size);

    ProcessingUnitState state;
    state.stack_size_x = pu->memory_size_x - pu->stack_pointer_x - 1;
    state.stack_size_y = pu->memory_size_y - pu->stack_pointer_y - 1;

    // Allocate memory for the stack
    state.stack = (int **)malloc(state.stack_size_x * sizeof(int *));
    if (state.stack == NULL) {
        printf("Memory allocation failed for stack\n");
        exit(1);
    }
    
    for (int i = 0; i < state.stack_size_x; i++) {
        state.stack[i] = (int *)malloc(state.stack_size_y * sizeof(int));
        if (state.stack[i] == NULL) {
            printf("Memory allocation failed for stack row %d\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < state.stack_size_x; i++) {
        for (int j = 0; j < state.stack_size_y; j++) {
            state.stack[i][j] = pu->memory[pu->stack_pointer_x + 1 + i][pu->stack_pointer_y + 1 + j];
        }
    }

    // Allocate memory for the registers
    state.registers = (int **)malloc(pu->num_registers_x * sizeof(int *));
    if (state.registers == NULL) {
        printf("Memory allocation failed for state registers\n");
        exit(1);
    }
    
    for (int i = 0; i < pu->num_registers_x; i++) {
        state.registers[i] = (int *)malloc(pu->num_registers_y * sizeof(int));
        if (state.registers[i] == NULL) {
            printf("Memory allocation failed for state registers row %d\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < pu->num_registers_x; i++) {
        for (int j = 0; j < pu->num_registers_y; j++) {
            state.registers[i][j] = pu->registers[i][j];
        }
    }

    return state;
}

int main(int argc, char *argv[]) {
    // Grab MDPU configuration from command line arguments
    if (argc != 5) {
        printf("Usage: %s <register_x> <register_y> <memory_x> <memory_y>\n", argv[0]);
        return 1;
    }

    int num_registers_x = atoi(argv[1]);
    int num_registers_y = atoi(argv[2]);
    int memory_size_x = atoi(argv[3]);
    int memory_size_y = atoi(argv[4]);

    ProcessingUnit pu;
    initialize(&pu, num_registers_x, num_registers_y, memory_size_x, memory_size_y);

    Instruction program[] = {
        {OP_ADD, 0, 0, 0, 1, 1, 0, 0, 0},    // ADD R0_0 and R0_1, store result in R1_0
        {OP_PUSH, 1, 0, 0, 0, 0, 0, 0, 0},   // PUSH R1_0 onto stack
        {OP_HALT, 0, 0, 0, 0, 0, 0, 0, 0}    // HALT
    };
    int program_size = sizeof(program) / sizeof(Instruction);

    ProcessingUnitState state = run(&pu, program, program_size);

    // Print the state of the stack
    printf("Stack State:\n");
    for (int i = 0; i < state.stack_size_x; i++) {
        printf("S%d: ", i);
        for (int j = 0; j < state.stack_size_y; j++) {
            printf("%d ", state.stack[i][j]);
        }
        printf("\n");
    }

    // Print the state of the registers
    printf("Registers State:\n");
    for (int i = 0; i < pu.num_registers_x; i++) {
        printf("R%d: ", i);
        for (int j = 0; j < pu.num_registers_y; j++) {
            printf("%d ", state.registers[i][j]);
        }
        printf("\n");
    }

    // Free the allocated memory
    free_processing_unit(&pu);
    free_processing_unit_state(&state);

    return 0;
}
