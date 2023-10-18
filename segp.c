#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTION_LENGTH 100  // Adjust as needed for your specific use case

typedef struct {
    char mnemonic[7];
    char opcode[3];
    int operandCount;
    int shiftAmount;
} Instruction;

typedef struct {
    char name[5];
    int value;
} Register;

void readAssemblyFromFile(const char *filename, Instruction instructions[], Register registers[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_INSTRUCTION_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        // printf("Executing assembly code: %s \n", line);

        execute(line, instructions, registers);
    }

    fclose(file);
}
int stackPointer = 20;
int programCounter = 59;
int stack[100]; // Stack memory
int zeroFlag = 0; // Initialize the zero flag to 0
int carryFlag = 0; // Initialize the carry flag to 0

// Function to execute assembly code and update registers
void execute(const char* assembly, Instruction instructions[], Register registers[]) {
    char mnemonic[7];
    char operands[3][5]; // Max three operands with 4 characters each
    

    printf("Executing assembly: %s\n", assembly);

    if (sscanf(assembly, "%6s %4s %4s %4s", mnemonic, operands[0], operands[1], operands[2]) >= 1) {
        // Debugging: Print the parsed mnemonic and operands
        // printf("Mnemonic: %s, Operands: %s %s %s\n", mnemonic, operands[0], operands[1], operands[2]);
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "M") == 0) {
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
                }else if (strcmp(mnemonic, "MSP") == 0) {
                    int destinationRegister = atoi(operands[0] + 1); // Parse destination register number
                    registers[destinationRegister].value = stackPointer;
                }else if (strcmp(mnemonic, "MIL") == 0) {
                    int destinationRegister = atoi(operands[0] + 1); // Parse destination register number
                    int immediateValue = atoi(operands[1]); // Parse immediate value
                    // Clear the lower 16 bits of the destination register and then set the immediate value
                    registers[destinationRegister].value &= 0xFFFF0000;
                    registers[destinationRegister].value |= (immediateValue & 0x0000FFFF);
                }else if (strcmp(mnemonic, "MIH") == 0) {
                    int destinationRegister = atoi(operands[0] + 1); // Parse destination register number
                    int immediateValue = atoi(operands[1]); // Parse immediate value
                    // Clear the higher 16 bits of the destination register and then set the immediate value
                    registers[destinationRegister].value &= 0x0000FFFF;
                    registers[destinationRegister].value |= (immediateValue << 16);
                }else if (strcmp(mnemonic, "MI") == 0) {
                    int destinationRegister = atoi(operands[0] + 1); // Parse destination register number
                    int immediateValue = atoi(operands[1]); // Parse immediate value
                    registers[destinationRegister].value = immediateValue;
                }
                else if (strcmp(mnemonic, "IOM") == 0) {
                    int sourceRegister = atoi(operands[0] + 1); // Parse source register number
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    registers[destinationRegister].value = registers[sourceRegister].value;
                    //implement further 
                    printf("Data moved from RA (R%d) to I/O port at RB (R%d).\n", sourceRegister, destinationRegister);
                }
                
                else if (strcmp(mnemonic, "IMM") == 0) {
                    int sourceIOPort = atoi(operands[0] + 1); // Parse source I/O port address
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    //need to implement further
                    printf("Input data moved from I/O port address RA (R%d) to memory pointed by RB (R%d).\n", sourceIOPort, destinationRegister);
                }else if (strcmp(mnemonic, "CALL") == 0) {
                    // CALL instruction
                    int offset = atoi(operands[0]);
                    // 1. Calculate the jump PC from the offset and the current PC.
                    int jumpPC = programCounter + offset;
                    // 2. Increment the current PC by 1 word to get the next PC.
                    programCounter += 4;
                    // 3. Decrement the current SP by 1 word to get the next SP.
                    if (stackPointer > 0) {
                        stackPointer -= 4;
                        // 4. Save the next PC to the location pointed to by the next SP (PC -> @SP).
                        stack[stackPointer] = jumpPC;

                        // 5. Transfer the program to the jump PC.
                        programCounter = jumpPC;
                    } else {
                        // Handle the case when the stack is empty.
                        // You might want to print an error message or take appropriate action.
                        printf("Error: Stack is empty. Cannot execute CALL.\n");
                    }
                } else if (strcmp(mnemonic, "RET") == 0) {
                    // RET instruction
                    if (stackPointer > 0) {
                        stackPointer--;
                        programCounter = stack[stackPointer];
                    } else {
                        // Handle the case when there's nothing on the stack to return to.
                        // You might want to print an error message or take appropriate action.
                    }
                } else if (strcmp(mnemonic, "PUSH") == 0) {
                    // PUSH instruction
                    int sourceRegister = atoi(operands[0] + 1); // Extract the register number

                    if (stackPointer < sizeof(stack) / sizeof(stack[0])) { // Check if the stack is not full
                        stack[stackPointer] = registers[sourceRegister].value; // Push the value onto the stack
                        stackPointer++; // Increment the stack pointer
                    }
                } else if (strcmp(mnemonic, "POP") == 0) {
                    // POP instruction
                    int destinationRegister = atoi(operands[0] + 1); // Extract the register number
                    if (stackPointer > 0) {
                        stackPointer--; // Decrement the stack pointer
                        registers[destinationRegister].value = stack[stackPointer]; // Pop the value from the stack
                    }
                } else if (strcmp(mnemonic, "J") == 0) {
                    // JUMP instruction
                    int targetOffset = atoi(operands[0]);
                    programCounter += targetOffset - 1; // Jump to PC plus (d23-d0)
                } else if (strcmp(mnemonic, "JEQ") == 0) {
                    // Handle "JEQ" instruction
                    int sourceRegister1 = atoi(operands[0] + 1);
                    int sourceRegister2 = atoi(operands[1] + 1);
                    if (registers[sourceRegister1].value == registers[sourceRegister2].value) {
                        zeroFlag = 1;
                    } else {
                        // Jump to PC plus (d23-d0)
                        programCounter += atoi(operands[2]) - 1;
                    }
                } else if (strcmp(mnemonic, "JLT") == 0) {
                    // Handle "JLT" instruction
                    int sourceRegister1 = atoi(operands[0] + 1);
                    int sourceRegister2 = atoi(operands[1] + 1);
                    if (registers[sourceRegister1].value < registers[sourceRegister2].value) {
                        zeroFlag = 0;
                        carryFlag = 1;
                    } else {
                        // Jump to PC plus (d23-d0)
                        programCounter += atoi(operands[2]) - 1;
                    }
                } else if (strcmp(mnemonic, "JGT") == 0) {
                    // Handle "JGT" instruction
                    int sourceRegister1 = atoi(operands[0] + 1);
                    int sourceRegister2 = atoi(operands[1] + 1);
                    if (registers[sourceRegister1].value > registers[sourceRegister2].value) {
                        zeroFlag = 0;
                        carryFlag = 0;
                    } else {
                        // Jump to PC plus (d23-d0)
                        programCounter += atoi(operands[2]) - 1;
                    }
                } else if (strcmp(mnemonic, "JLE") == 0) {
                    // Handle "JLE" instruction
                    int sourceRegister1 = atoi(operands[0] + 1);
                    int sourceRegister2 = atoi(operands[1] + 1);
                    if (registers[sourceRegister1].value <= registers[sourceRegister2].value) {
                        zeroFlag = 1;
                    } else {
                        // Jump to PC plus (d23-d0)
                        programCounter += atoi(operands[2]) - 1;
                    }
                } else if (strcmp(mnemonic, "JGE") == 0) {
                    // Handle "JGE" instruction
                    int sourceRegister1 = atoi(operands[0] + 1);
                    int sourceRegister2 = atoi(operands[1] + 1);
                    if (registers[sourceRegister1].value >= registers[sourceRegister2].value) {
                        zeroFlag = 1;
                    } else {
                        // Jump to PC plus (d23-d0)
                        programCounter += atoi(operands[2]) - 1;
                    }
                }
                else if (strcmp(mnemonic, "NOP") == 0) {
                    // NOP does nothing, so just continue to the next instruction
                    return;
                } 
                break;
            }
        }
    }
    else if (sscanf(assembly, "%3s %4s @%4s", mnemonic, operands[0], operands[1]) >= 1) {
        // Debugging: Print the parsed mnemonic and operands
        printf("Mnemonic: %s, Operands: %s %s\n", mnemonic, operands[0], operands[1]);
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "MM") == 0) {
                    int sourceRegister = atoi(operands[0] + 1);  // Parse source register number
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    // Assuming the value of sourceRegister is the memory address
                    int memoryValue = registers[sourceRegister].value;
                    registers[destinationRegister].value = memoryValue;
                }
                break;
            }
        }
    }
    
    else if (sscanf(assembly, "%3s @%4s %4s", mnemonic, operands[0], operands[1]) >= 1) {
        // Debugging: Print the parsed mnemonic and operands
        printf("Mnemonic: %s, Operands: %s %s\n", mnemonic, operands[0], operands[1]);
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "MRR") == 0) {
                    int sourceRegister = atoi(operands[0] + 2);  // Parse source register number
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    registers[destinationRegister].value = registers[sourceRegister].value; // Simulated memory to register move
                }
                break;
            }
        }
    }

    else if (sscanf(assembly, "%3s %4s #%4s", mnemonic, operands[0], operands[1]) >= 1) {
        // Debugging: Print the parsed mnemonic and operands
        printf("Mnemonic: %s, Operands: %s %s\n", mnemonic, operands[0], operands[1]);
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "IOM") == 0) {
                    int sourceRegister = atoi(operands[0] + 1); // Parse source register number
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    registers[destinationRegister].value = registers[sourceRegister].value;
                    //implement further
                    printf("Data moved from RA (R%d) to I/O port at RB (R%d).\n", sourceRegister, destinationRegister);
                }
                break;
            }
        }
    }

    else if (sscanf(assembly, "%3s #%4s %4s", mnemonic, operands[0], operands[1]) >= 1) {
        // Debugging: Print the parsed mnemonic and operands
        printf("Mnemonic: %s, Operands: %s %s\n", mnemonic, operands[0], operands[1]);
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "IIM") == 0) {
                    // Parse immediate value (assuming it's in decimal format)
                    int immediateValue = atoi(operands[0] + 1);
                    int destinationRegister = atoi(operands[1] + 1); // Parse destination register number
                    registers[destinationRegister].value = immediateValue;

                    printf("Immediate value %d moved to register R%d.\n", immediateValue, destinationRegister);
                }
                break;
            }
        }
    }
    if (sscanf(assembly, "%3s @%4s #%4s", mnemonic, operands[0], operands[1]) >= 1) {
        // Debugging: Print the parsed mnemonic and operands
        printf("Mnemonic: %s, Operands: %s %s\n", mnemonic, operands[0], operands[1]);
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "MMI") == 0) {
                    int sourceRegister = atoi(operands[0] + 2);  // Parse source register number
                    int immediateValue = atoi(operands[1] + 1);  // Parse immediate value
                    // Assuming the value of sourceRegister is the memory address
                    int memoryValue = registers[sourceRegister].value;
                    // Implement logic to move data from memory to the I/O port at the immediate value
                    printf("Data moved from memory at address R%d to I/O port at address %d.\n", sourceRegister, immediateValue);
                }
                break;
            }
        }
    }
    else if (sscanf(assembly, "%3s #%4s @%4s", mnemonic, operands[0], operands[1]) >= 1) {
        // Debugging: Print the parsed mnemonic and operands
        printf("Mnemonic: %s, Operands: %s %s\n", mnemonic, operands[0], operands[1]);
        for (size_t i = 0; instructions[i].mnemonic[0] != '\0'; i++) {
            if (strcmp(mnemonic, instructions[i].mnemonic) == 0) {
                // Execute the operation based on the mnemonic
                if (strcmp(mnemonic, "IMM") == 0) {
                    int destinationRegister = atoi(operands[0] + 1);  // Parse destination register number
                    int sourceRegister = atoi(operands[1] + 2);  // Parse source register number
                    registers[destinationRegister].value = registers[sourceRegister].value;

                    printf("Value from register R%d moved to register R%d.\n", sourceRegister, destinationRegister);
                }
                break;
            }
        }
    }

}

int main() {
    Instruction instructions[] = {
        {"NOP", "00",0},
        {"MIL","03",2,0},
        {"MIH","04",2,0},
        {"MI","04",2,0},
        {"M", "06", 2,0},
        {"MSP", "07", 2,0},
        {"MM", "08", 2,0},
        {"MRR", "09", 2,0},
        {"IOM", "0A", 2,0},
        {"IIM", "0B", 2,0},
        {"MMI", "0C", 2,0},
        {"IMM", "0D", 2,0},
        {"ADD", "12", 3,0},
        {"SUB", "13", 3,0},
        {"AND", "0E", 3,0},
        {"OR", "0F", 3,0},
        {"XOR", "10", 3,0},
        {"NOT", "11", 2,0},
        {"SL", "14", 3, 0}, 
        {"SR", "15", 3, 0}, 
        {"CALL", "16", 1, 0},
        {"RET", "17", 0, 0},
        {"PUSH", "18", 1, 0},
        {"POP", "19", 1, 0},
        {"J", "1A", 1, 0},
        {"JEQ", "1B", 2, 0},   // Jump if equal
        {"JLT", "1C", 2, 0},   // Jump if less than
        {"JGT", "1D", 2, 0},   // Jump if greater than
        {"JLE", "1E", 2, 0},   // Jump if less than or equal
        {"JGE", "1F", 2, 0},    // Jump if greater than or equal
        {""} 
    };

    const int numRegisters = 256;

    Register registers[numRegisters];
    for (int i = 0; i < numRegisters; i++) {
        snprintf(registers[i].name, sizeof(registers[i].name), "R%d", i);
        registers[i].value = 0;
    }


    // Read assembly code from the file and execute it line by line
    printf("Reading assembly code from file and executing line by line...\n");
    readAssemblyFromFile("example2.txt", instructions, registers);
        
    printf("Stack Pointer (SP): 0x%08X\n", stackPointer);
    printf("Program Counter (PC): 0x%08X\n", programCounter);

    // Print the updated registers in hexadecimal format
    for (int i = 0; i < numRegisters; i++) {
        printf("Register %s: 0x%08X\n", registers[i].name, registers[i].value);
        printf("Register %s: %d\n", registers[i].name, registers[i].value);
    }

    return 0;
}
