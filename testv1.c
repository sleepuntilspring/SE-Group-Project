#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define an instruction structure with mnemonic, opcode, and operand count
typedef struct {
    char mnemonic[4]; // 3 characters + null terminator
    char opcode[3];   // 2 characters + null terminator
    int operandCount;
    int shiftAmount; // New field for shift amount
} Instruction;


// Define registers and their values
typedef struct {
    char name[5]; // 4 characters + null terminator
    int value;
} Register;

// Function to execute assembly code and update registers
void execute(const char* assembly, Instruction instructions[], Register registers[]) {
    char mnemonic[4];
    char operands[3][5]; // Max three operands with 4 characters each

    if (sscanf(assembly, "%3s %4s %4s %4s", mnemonic, operands[0], operands[1], operands[2]) >= 1) {
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "MOV") == 0) {
                    int destinationRegister = atoi(operands[0] + 1); // Parse register number
                    int valueToMove = atoi(operands[1]);
                    registers[destinationRegister].value = valueToMove;
                } else if (strcmp(mnemonic, "ADD") == 0) {
                    int destinationRegister = atoi(operands[2] + 1); // Parse destination register number
                    int sourceRegister1 = atoi(operands[0] + 1); // Parse source register 1 number
                    int sourceRegister2 = atoi(operands[1] + 1); // Parse source register 2 number
                    registers[destinationRegister].value = registers[sourceRegister1].value + registers[sourceRegister2].value;
                } else if (strcmp(mnemonic, "SUB") == 0) {
                    int destinationRegister = atoi(operands[2] + 1); // Parse destination register number
                    int sourceRegister1 = atoi(operands[0] + 1); // Parse source register 1 number
                    int sourceRegister2 = atoi(operands[1] + 1); // Parse source register 2 number
                    registers[destinationRegister].value = registers[sourceRegister1].value - registers[sourceRegister2].value;
                } else if (strcmp(mnemonic, "AND") == 0) {
                    int destinationRegister = atoi(operands[2] + 1); // Parse destination register number
                    int sourceRegister1 = atoi(operands[0] + 1); // Parse source register 1 number
                    int sourceRegister2 = atoi(operands[1] + 1); // Parse source register 2 number
                    registers[destinationRegister].value = registers[sourceRegister1].value & registers[sourceRegister2].value;
                } else if (strcmp(mnemonic, "OR") == 0) {
                    int destinationRegister = atoi(operands[2] + 1); // Parse destination register number
                    int sourceRegister1 = atoi(operands[0] + 1); // Parse source register 1 number
                    int sourceRegister2 = atoi(operands[1] + 1); // Parse source register 2 number
                    registers[destinationRegister].value = registers[sourceRegister1].value | registers[sourceRegister2].value;
                } else if (strcmp(mnemonic, "XOR") == 0) {
                    int destinationRegister = atoi(operands[2] + 1); // Parse destination register number
                    int sourceRegister1 = atoi(operands[0] + 1); // Parse source register 1 number
                    int sourceRegister2 = atoi(operands[1] + 1); // Parse source register 2 number
                    registers[destinationRegister].value = registers[sourceRegister1].value ^ registers[sourceRegister2].value;
                } else if (strcmp(mnemonic, "NOT") == 0) {
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    int sourceRegister = atoi(operands[0] + 1); // Parse source register number
                    registers[destinationRegister].value = ~registers[sourceRegister].value;
                } else if (strcmp(mnemonic, "SL") == 0) {
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    int sourceRegister = atoi(operands[0] + 1); // Parse source register number
                    int shiftAmount = atoi(operands[2]); // Parse shift amount
                    registers[destinationRegister].value = registers[sourceRegister].value << shiftAmount;
                } else if (strcmp(mnemonic, "SR") == 0) {
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    int sourceRegister = atoi(operands[0] + 1); // Parse source register number
                    int shiftAmount = atoi(operands[2]); // Parse shift amount
                    registers[destinationRegister].value = registers[sourceRegister].value >> shiftAmount;
                } else if (strcmp(mnemonic, "NOP") == 0) {
                    // NOP does nothing, so just continue to the next instruction
                    return;
                }
                break;
            }
        }
    }
}

int main() {
    // Define available instructions
    Instruction instructions[] = {
        {"NOP", "00"},
        {"MOV", "01", 2},
        {"ADD", "02", 3},
        {"SUB", "03", 3},
        {"AND", "04", 3},
        {"OR", "05", 3},
        {"XOR", "06", 3},
        {"NOT", "07", 2},
        {"SL", "08", 3, 0}, 
        {"SR", "09", 3, 0}, 
        
        {""}, // Empty entry to mark the end of the instruction list
};

    // Define the number of registers
    const int numRegisters = 256; // R0 to R255

    // Initialize registers
    Register registers[numRegisters];
    for (int i = 0; i < numRegisters; i++) {
        snprintf(registers[i].name, sizeof(registers[i].name), "R%d", i);
        registers[i].value = 0;
    }

    const char* assemblyCode1 = "MOV R0 5"; // Example assembly code (MOV R0, 10)
    const char* assemblyCode2 = "MOV R1 3"; // Example assembly code (MOV R1, 20)
    const char* assemblyCode3 = "ADD R0 R1 R2"; // Example assembly code (ADD R2, R0, R1)
    const char* assemblyCode4 = "SUB R0 R1 R3"; // Example assembly code (SUB R1, R0, R2)
    const char* assemblyCode5 = "AND R1 R0 R4"; // Example assembly code (AND R2, R0, R1)
    const char* assemblyCode6 = "XOR R1 R0 R5"; // Example assembly code (XOR R2, R0, R1)
    const char* assemblyCode7 = "NOT R3 R6";    // Example assembly code (NOT R3, R2)
    const char* assemblyCode8 = "SL R1 R7 2"; // Example assembly code (SL R4, R1, 2)
    const char* assemblyCode9 = "SR R1 R8 1"; // Example assembly code (SR R5, R3, 3)
    const char* assemblyCode10 = "OR R0 R1 R10";     // Example assembly code (SR R5, R3)

    execute(assemblyCode1, instructions, registers);
    execute(assemblyCode2, instructions, registers);
    execute(assemblyCode3, instructions, registers);
    execute(assemblyCode4, instructions, registers);
    execute(assemblyCode5, instructions, registers);
    execute(assemblyCode6, instructions, registers);
    execute(assemblyCode7, instructions, registers);
    execute(assemblyCode8, instructions, registers);
    execute(assemblyCode9, instructions, registers);
    execute(assemblyCode10, instructions, registers);


    // Print the updated registers in hexadecimal format
    for (int i = 0; i < numRegisters; i++) {
        printf("Register %s: 0x%08X\n", registers[i].name, registers[i].value);
        printf("Register %s: %d\n", registers[i].name, registers[i].value);
    }

    return 0;
}