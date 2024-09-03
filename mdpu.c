#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    NOP,
    ADD,
    SUB,
    MUL,
    DIV,
    STORE,
    LOAD,
    LOAD_IMMEDIATE,
    PUSH,
    POP,
    JMP,
    JZ,
    JNZ,
    MOV,
    JE,
    JNE,
    AND,
    OR,
    XOR,
    NOT,
    SHL,
    SHR,
    CMP,
    TEST,
    B,
    BZ,
    BNZ,
    NEG,
    ABS,
    HALT
} Opcode;

// Define the structure of an instruction
typedef struct {
    Opcode opcode;
    int reg1;      // First register index
    int reg2;      // Second register index
    int reg3;      // Destination register index
    int addr;      // Memory address or jump address
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

// Helper function to check register bounds
void check_register_bounds(ProcessingUnit *pu, int reg) {
    if (reg < 0 || reg >= pu->num_registers) {
        printf("Error: Register index out of bounds: R%d\n", reg);
        exit(1);
    }
}

// ++++++++++++++++++++++++++++++ Arithmetic operations ++++++++++++++++++++++++++++++ //
void add(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] + pu->registers[reg2];
}

void subtract(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] - pu->registers[reg2];
}

void multiply(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] * pu->registers[reg2];
}

void divide(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    if (pu->registers[reg2] != 0) {
        pu->registers[reg3] = pu->registers[reg1] / pu->registers[reg2];
    } else {
        printf("Error: Division by zero on R%d of value %d\n", reg2, pu->registers[reg2]);
        exit(1);
    }
}

void neg(ProcessingUnit *pu, int reg1, int reg2) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    pu->registers[reg2] = -pu->registers[reg1];
}

void absolute(ProcessingUnit *pu, int reg1, int reg2) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    pu->registers[reg2] = abs(pu->registers[reg1]);
}

// ++++++++++++++++++++++++++++++ Memory operations ++++++++++++++++++++++++++++++ //
void store(ProcessingUnit *pu, int reg, int addr) {
    check_register_bounds(pu, reg);
    if (addr >= 0 && addr < pu->memory_size) {
        pu->memory[addr] = pu->registers[reg];
    } else {
        printf("Error: Memory address out of bounds: %d\n", addr);
        exit(1);
    }
}

void load(ProcessingUnit *pu, int addr, int reg) {
    check_register_bounds(pu, reg);
    if (addr >= 0 && addr < pu->memory_size) {
        pu->registers[reg] = pu->memory[addr];
    } else {
        printf("Error: Memory address out of bounds: %d\n", addr);
        exit(1);
    }
}

// ++++++++++++++++++++++++++++++ Stack operations ++++++++++++++++++++++++++++++ //
void push(ProcessingUnit *pu, int reg) {
    check_register_bounds(pu, reg);
    if (pu->stack_pointer >= 0) {
        pu->memory[pu->stack_pointer] = pu->registers[reg];
        pu->stack_pointer--;
    } else {
        printf("Error: Stack overflow on R%d\n", reg);
        exit(1);
    }
}

void pop(ProcessingUnit *pu, int reg) {
    check_register_bounds(pu, reg);
    if (pu->stack_pointer < pu->memory_size - 1) {
        pu->stack_pointer++;
        pu->registers[reg] = pu->memory[pu->stack_pointer];
    } else {
        printf("Error: Stack underflow on R%d\n", reg);
        exit(1);
    }
}

void mov(ProcessingUnit *pu, int reg1, int reg2) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    pu->registers[reg1] = pu->registers[reg2];
}

// ++++++++++++++++++++++++++++++ Jump operations ++++++++++++++++++++++++++++++ //
void jmp(int *instruction_pointer, int addr) {
    *instruction_pointer = addr;
}

void jz(ProcessingUnit *pu, int *instruction_pointer, int reg, int addr) {
    check_register_bounds(pu, reg);
    if (pu->registers[reg] == 0) {
        *instruction_pointer = addr;
    }
}

void jnz(ProcessingUnit *pu, int *instruction_pointer, int reg, int addr) {
    check_register_bounds(pu, reg);
    if (pu->registers[reg] != 0) {
        *instruction_pointer = addr;
    }
}

void je(ProcessingUnit *pu, int *instruction_pointer, int reg1, int reg2, int addr) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    if (pu->registers[reg1] == pu->registers[reg2]) {
        *instruction_pointer = addr;
    }
}

void jne(ProcessingUnit *pu, int *instruction_pointer, int reg1, int reg2, int addr) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    if (pu->registers[reg1] != pu->registers[reg2]) {
        *instruction_pointer = addr;
    }
}

// ++++++++++++++++++++++++++++++ Bitwise operations ++++++++++++++++++++++++++++++ //
void and(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] & pu->registers[reg2];
}

void or(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] | pu->registers[reg2];
}

void xor(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] ^ pu->registers[reg2];
}

void not(ProcessingUnit *pu, int reg1, int reg2) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    pu->registers[reg2] = ~pu->registers[reg1];
}

void shl(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] << pu->registers[reg2];
}

void shr(ProcessingUnit *pu, int reg1, int reg2, int reg3) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    check_register_bounds(pu, reg3);
    pu->registers[reg3] = pu->registers[reg1] >> pu->registers[reg2];
}

// ++++++++++++++++++++++++++++++ Comparison operations ++++++++++++++++++++++++++++++ //
void cmp(ProcessingUnit *pu, int reg1, int reg2) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    if (pu->registers[reg1] == pu->registers[reg2]) {
        pu->registers[0] = 0;
    } else if (pu->registers[reg1] < pu->registers[reg2]) {
        pu->registers[0] = -1;
    } else {
        pu->registers[0] = 1;
    }
}

void test(ProcessingUnit *pu, int reg1, int reg2) {
    check_register_bounds(pu, reg1);
    check_register_bounds(pu, reg2);
    pu->registers[0] = pu->registers[reg1] & pu->registers[reg2];
}

// ++++++++++++++++++++++++++++++ Branch operations ++++++++++++++++++++++++++++++ //
void b(int *instruction_pointer, int addr) {
    *instruction_pointer = addr;
}

void bz(ProcessingUnit *pu, int *instruction_pointer, int reg, int addr) {
    check_register_bounds(pu, reg);
    if (pu->registers[reg] == 0) {
        *instruction_pointer = addr;
    }
}

void bnz(ProcessingUnit *pu, int *instruction_pointer, int reg, int addr) {
    check_register_bounds(pu, reg);
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
            case ADD:
                add(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case SUB:
                subtract(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case MUL:
                multiply(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case DIV:
                divide(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case STORE:
                store(pu, instr.reg1, instr.addr);
                break;
            case LOAD:
                load(pu, instr.addr, instr.reg1);
                break;
            case LOAD_IMMEDIATE:
                check_register_bounds(pu, instr.reg1);
                pu->registers[instr.reg1] = instr.immediate;
                break;
            case PUSH:
                push(pu, instr.reg1);
                break;
            case POP:
                pop(pu, instr.reg1);
                break;
            case JMP:
                jmp(&instruction_pointer, instr.addr);
                continue;
            case JZ:
                jz(pu, &instruction_pointer, instr.reg1, instr.addr);
                continue;
            case JNZ:
                jnz(pu, &instruction_pointer, instr.reg1, instr.addr);
                continue;
            case MOV:
                mov(pu, instr.reg1, instr.reg2);
                break;
            case JE:
                je(pu, &instruction_pointer, instr.reg1, instr.reg2, instr.addr);
                break;
            case JNE:
                jne(pu, &instruction_pointer, instr.reg1, instr.reg2, instr.addr);
                break;
            case AND:
                and(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case OR:
                or(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case XOR:
                xor(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case NOT:
                not(pu, instr.reg1, instr.reg2);
                break;
            case SHL:
                shl(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case SHR:
                shr(pu, instr.reg1, instr.reg2, instr.reg3);
                break;
            case CMP:
                cmp(pu, instr.reg1, instr.reg2);
                break;
            case TEST:
                test(pu, instr.reg1, instr.reg2);
                break;
            case B:
                b(&instruction_pointer, instr.addr);
                continue;
            case BZ:
                bz(pu, &instruction_pointer, instr.reg1, instr.addr);
                continue;
            case BNZ:
                bnz(pu, &instruction_pointer, instr.reg1, instr.addr);
                continue;
            case NEG:
                neg(pu, instr.reg1, instr.reg2);
                break;
            case ABS:
                absolute(pu, instr.reg1, instr.reg2);
                break;
            case HALT:
                return;
            default:
                printf("Error: Unknown opcode %d\n", instr.opcode);
                exit(1);
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

int parse_dimensions(char *size_str) {
    int total_size = 1;
    char *token = strtok(size_str, "x");
    while (token != NULL) {
        total_size *= atoi(token);
        token = strtok(NULL, "x");
    }
    return total_size;
}

// Function to convert string to uppercase
void str_to_upper(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Function to parse opcode from string
Opcode parse_opcode(const char *str) {
    char opcode_str[20];
    sscanf(str, "%s", opcode_str);  // Extract the first word

    if (strcmp(opcode_str, "") == 0) return NOP;
    if (strcmp(opcode_str, "//") == 0) return NOP;
    if (strcmp(opcode_str, "NOP") == 0) return NOP;
    if (strcmp(opcode_str, "ADD") == 0) return ADD;
    if (strcmp(opcode_str, "SUB") == 0) return SUB;
    if (strcmp(opcode_str, "MUL") == 0) return MUL;
    if (strcmp(opcode_str, "DIV") == 0) return DIV;
    if (strcmp(opcode_str, "STORE") == 0) return STORE;
    if (strcmp(opcode_str, "LOAD") == 0) return LOAD;
    if (strcmp(opcode_str, "LOAD_IMMEDIATE") == 0) return LOAD_IMMEDIATE;
    if (strcmp(opcode_str, "PUSH") == 0) return PUSH;
    if (strcmp(opcode_str, "POP") == 0) return POP;
    if (strcmp(opcode_str, "JMP") == 0) return JMP;
    if (strcmp(opcode_str, "JZ") == 0) return JZ;
    if (strcmp(opcode_str, "JNZ") == 0) return JNZ;
    if (strcmp(opcode_str, "MOV") == 0) return MOV;
    if (strcmp(opcode_str, "JE") == 0) return JE;
    if (strcmp(opcode_str, "JNE") == 0) return JNE;
    if (strcmp(opcode_str, "AND") == 0) return AND;
    if (strcmp(opcode_str, "OR") == 0) return OR;
    if (strcmp(opcode_str, "XOR") == 0) return XOR;
    if (strcmp(opcode_str, "NOT") == 0) return NOT;
    if (strcmp(opcode_str, "SHL") == 0) return SHL;
    if (strcmp(opcode_str, "SHR") == 0) return SHR;
    if (strcmp(opcode_str, "CMP") == 0) return CMP;
    if (strcmp(opcode_str, "TEST") == 0) return TEST;
    if (strcmp(opcode_str, "B") == 0) return B;
    if (strcmp(opcode_str, "BZ") == 0) return BZ;
    if (strcmp(opcode_str, "BNZ") == 0) return BNZ;
    if (strcmp(opcode_str, "NEG") == 0) return NEG;
    if (strcmp(opcode_str, "ABS") == 0) return ABS;
    if (strcmp(opcode_str, "HALT") == 0) return HALT;
    
    printf("Error: Unknown opcode %s\n", opcode_str);
    exit(1);
}

// Function to parse instruction file
Instruction* parse_instruction_file(const char* filename, int* program_size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    Instruction* program = NULL;
    int capacity = 10;
    *program_size = 0;
    program = malloc(capacity * sizeof(Instruction));

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (*program_size >= capacity) {
            capacity *= 2;
            program = realloc(program, capacity * sizeof(Instruction));
        }

        char opcode_str[20];
        int reg1, reg2, reg3, addr, immediate;
        sscanf(line, "%s %d %d %d %d %d", opcode_str, &reg1, &reg2, &reg3, &addr, &immediate);
        
        str_to_upper(opcode_str);
        Opcode opcode = parse_opcode(opcode_str);

        if (opcode == NOP) {
            continue;
        }

        program[*program_size] = (Instruction){opcode, reg1, reg2, reg3, addr, immediate};
        (*program_size)++;
    }

    fclose(file);
    return program;
}

// Modify the main function to use the new parser
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <register_size_dimensions> <memory_size_dimensions> <instruction_file>\n", argv[0]);
        exit(1);
    }

    // Parse the dimensions for registers and memory
    int total_registers = parse_dimensions(argv[1]);
    int total_memory = parse_dimensions(argv[2]);

    ProcessingUnit pu;
    initialize(&pu, total_registers, total_memory);

    // Parse the instruction file
    int program_size;
    Instruction* program = parse_instruction_file(argv[3], &program_size);

    // Run the program
    ProcessingUnitState state = run(&pu, program, program_size, 1000);

    // Clean up
    post_run(&state, &pu);
    free(program);

    return 0;
}
