#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the structure of the multi-dimensional processing unit
typedef struct {
    int *registers;
    int *memory;
    int num_registers;
    int memory_size;
    int stack_pointer;
} ProcessingUnit;

// Define the structure to hold the state after execution
typedef struct {
    int *registers;
    int *stack;
    int stack_size;
} ProcessingUnitState;

// Define opcodes
typedef enum {
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_STORE,
    OP_LOAD,
    OP_LOAD_IMMEDIATE,
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
    int reg1;   // First register index
    int reg2;   // Second register index
    int reg3;   // Destination register index
    int addr;   // Memory address or jump address
    int immediate; // Immediate value
} Instruction;

// Function to initialize the processing unit
void initialize(ProcessingUnit *pu, int num_registers, int memory_size) {
    pu->num_registers = num_registers;
    pu->memory_size = memory_size;
    
    pu->registers = (int *)malloc(num_registers * sizeof(int));
    if (pu->registers == NULL) {
        printf("Memory allocation failed for registers\n");
        exit(1);
    }
    
    pu->memory = (int *)malloc(memory_size * sizeof(int));
    if (pu->memory == NULL) {
        printf("Memory allocation failed for memory\n");
        exit(1);
    }

    pu->stack_pointer = memory_size - 1; // Initialize stack pointer to the top of the memory

    for (int i = 0; i < num_registers; i++) {
        pu->registers[i] = 0;
    }
    
    for (int i = 0; i < memory_size; i++) {
        pu->memory[i] = 0;
    }
}

// Function to free the memory allocated for the processing unit
void free_processing_unit(ProcessingUnit *pu) {
    if (pu->registers != NULL) {
        free(pu->registers);
    }
    
    if (pu->memory != NULL) {
        free(pu->memory);
    }
}

// ++++++++++++++++++++++++++++++ Arithmetic operations ++++++++++++++++++++++++++++++ //
void add(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    pu->registers[reg3] = pu->registers[reg1] + pu->registers[reg2];
}

void subtract(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    pu->registers[reg3] = pu->registers[reg1] - pu->registers[reg2];
}

void multiply(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    pu->registers[reg3] = pu->registers[reg1] * pu->registers[reg2];
}

void divide(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    if (pu->registers[reg2] != 0) {
        pu->registers[reg3] = pu->registers[reg1] / pu->registers[reg2];
    } else {
        printf("Error: Division by zero\n");
        exit(1);
    }
}

// ++++++++++++++++++++++++++++++ Memory operations ++++++++++++++++++++++++++++++ //
void store(ProcessingUnit *pu, int reg, int addr) {
    if (addr >= 0 && addr < pu->memory_size) {
        pu->memory[addr] = pu->registers[reg];
    } else {
        printf("Error: Memory address out of bounds: %d\n", addr);
        exit(1);
    }
}

void load(ProcessingUnit *pu, int addr, int reg) {
    if (addr >= 0 && addr < pu->memory_size) {
        pu->registers[reg] = pu->memory[addr];
    } else {
        printf("Error: Memory address out of bounds: %d\n", addr);
        exit(1);
    }
}

// ++++++++++++++++++++++++++++++ Stack operations ++++++++++++++++++++++++++++++ //
void push(ProcessingUnit *pu, int reg) {
    if (pu->stack_pointer >= 0) {
        pu->memory[pu->stack_pointer] = pu->registers[reg];
        pu->stack_pointer--;
    } else {
        printf("Error: Stack overflow on R%d\n", reg);
        exit(1);
    }
}

void pop(ProcessingUnit *pu, int reg) {
    if (pu->stack_pointer < pu->memory_size - 1) {
        pu->stack_pointer++;
        pu->registers[reg] = pu->memory[pu->stack_pointer];
    } else {
        printf("Error: Stack underflow on R%d\n", reg);
        exit(1);
    }
}

// ++++++++++++++++++++++++++++++ Jump operations ++++++++++++++++++++++++++++++ //
void jmp(int *instruction_pointer, int addr) {
    *instruction_pointer = addr;
}

void jz(ProcessingUnit *pu, int *instruction_pointer, int reg, int addr) {
    if (pu->registers[reg] == 0) {
        *instruction_pointer = addr;
    }
}

void jnz(ProcessingUnit *pu, int *instruction_pointer, int reg, int addr) {
    if (pu->registers[reg] != 0) {
        *instruction_pointer = addr;
    }
}

// ++++++++++++++++++++++++++++++ Program execution ++++++++++++++++++++++++++++++ //
void execute_program(ProcessingUnit *pu, Instruction *program, int program_size, int mic) {
    const int MAX_INSTRUCTION_COUNT = mic;
    int instruction_count = 0;
    int instruction_pointer = 0;

    while (instruction_pointer < program_size) {
        if (instruction_count >= MAX_INSTRUCTION_COUNT) {
            printf("Error: Maximum instruction count exceeded, possible infinite loop\n");
            exit(1);
        }

        Instruction instr = program[instruction_pointer];
        switch (instr.opcode) {
            case OP_ADD:
                add(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case OP_SUBTRACT:
                subtract(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case OP_MULTIPLY:
                multiply(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case OP_DIVIDE:
                divide(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case OP_STORE:
                store(pu, instr.reg1, instr.addr);
                break;
            case OP_LOAD:
                load(pu, instr.addr, instr.reg1);
                break;
            case OP_LOAD_IMMEDIATE:
                pu->registers[instr.reg1] = instr.immediate;
                break;
            case OP_PUSH:
                push(pu, instr.reg1);
                break;
            case OP_POP:
                pop(pu, instr.reg1);
                break;
            case OP_JMP:
                jmp(&instruction_pointer, instr.addr);
                continue;
            case OP_JZ:
                jz(pu, &instruction_pointer, instr.reg1, instr.addr);
                continue;
            case OP_JNZ:
                jnz(pu, &instruction_pointer, instr.reg1, instr.addr);
                continue;
            case OP_HALT:
                return;
        }
        instruction_pointer++;
        instruction_count++;
    }
}

// Function to free the memory allocated for the state
void free_processing_unit_state(ProcessingUnitState *state) {
    if (state->registers != NULL) {
        free(state->registers);
    }
    if (state->stack != NULL) {
        free(state->stack);
    }
}

// Function to run the program and return the state
ProcessingUnitState run(ProcessingUnit *pu, Instruction *program, int program_size, int mic) {
    execute_program(pu, program, program_size, mic);

    ProcessingUnitState state;
    state.stack_size = pu->memory_size - pu->stack_pointer - 1;

    // Allocate memory for the stack
    state.stack = (int *)malloc(state.stack_size * sizeof(int));
    if (state.stack == NULL) {
        printf("Memory allocation failed for stack\n");
        exit(1);
    }

    for (int i = 0; i < state.stack_size; i++) {
        state.stack[i] = pu->memory[pu->stack_pointer + 1 + i];
    }

    // Allocate memory for the registers
    state.registers = (int *)malloc(pu->num_registers * sizeof(int));
    if (state.registers == NULL) {
        printf("Memory allocation failed for state registers\n");
        exit(1);
    }

    for (int i = 0; i < pu->num_registers; i++) {
        state.registers[i] = pu->registers[i];
    }

    return state;
}

void post_run(ProcessingUnitState *state, ProcessingUnit *pu) {
    int num_registers = pu->num_registers;

    printf("Registers:\n");
    for (int i = 0; i < pu->num_registers; i++) {
        printf("R%d: %d\n", i, state->registers[i]);
    }

    printf("Stack:\n");
    for (int i = 0; i < state->stack_size; i++) {
        printf("S%d: %d\n", i, state->stack[i]);
    }

    free_processing_unit_state(state);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <register_size_x>x<register_size_y> <memory_size_x>x<memory_size_y>\n", argv[0]);
        exit(1);
    }

    // Split the args by `x` to get register_x, register_y, memory_x, memory_y
    char *register_size = argv[1];
    char *memory_size = argv[2];

    // Split the register size by `x` and multiply x and y to get the total number of registers
    char *register_x = strtok(register_size, "x");
    char *register_y = strtok(NULL, "x");
    char *memory_x = strtok(memory_size, "x");
    char *memory_y = strtok(NULL, "x");

    int total_registers = atoi(register_x) * atoi(register_y);
    int total_memory = atoi(memory_x) * atoi(memory_y);

    ProcessingUnit pu;
    initialize(&pu, total_registers, total_memory);

    // Example program (OP codes with some dummy values)
    Instruction program[] = {
        {OP_LOAD_IMMEDIATE, 0, 0, 0, 0, 10},
        {OP_LOAD_IMMEDIATE, 1, 0, 0, 0, 20},
        {OP_ADD, 0, 1, 2, 0, 0},
        {OP_STORE, 2, 0, 0, 0, 0},
        {OP_HALT, 0, 0, 0, 0, 0}
    };
    int program_size = sizeof(program) / sizeof(program[0]);

    // Run the program
    ProcessingUnitState state = run(&pu, program, program_size, 1000);

    // Clean up
    post_run(&state, &pu);

    return 0;
}
