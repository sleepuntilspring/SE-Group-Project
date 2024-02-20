#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

bool is_valid_hexadecimal(const char *str) {
    if (str == NULL) {
        return false;
    }

    // Skip "0x" prefix if present
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isxdigit((unsigned char)str[i])) {
            return false;
        }
    }
    return true;
}


// Function to convert a hexadecimal string to an integer
int hexToInt(char* hexStr) {
    int result = 0;
    sscanf(hexStr, "%X", &result);
    return result;
}

int registerToHex(char* reg) {
    if (strncmp(reg, "r", 1) == 0) {
        int registerNumber = atoi(reg + 1); // Remove the "r" prefix and convert to an integer
        if (registerNumber >= 0 && registerNumber <= 255) {
            return registerNumber;
        }
    }
    return -1; // Invalid register name
}

// void convert_and_print(FILE *inputFile, FILE *outputFile) {
void convert_and_print(FILE *inputFile, FILE *outputFile, const char *outputFileName, int targetLine, int *currentLine){
    
    // Array to store register values from 0 to 255
    int registers[256] = {0};
    int content[256] = {0};
    int IOcontent[256] = {0};

    char assembly_instruction[10000];
    char machine_code[18]; // Increased to 18 to store the 16-character machine code and null terminator
    int increment = 1; //default address counter increment by 1 only
    int counter=0;
    char* variables[10000];
    int varCount = 0;
    int varAddress[10000];

    int stackPointer = 0;
    int programCounter = 0;
    int stack[100]; // Stack memory
    int zeroFlag = 0; // Initialize the zero flag to 0
    int carryFlag = 0; // Initialize the carry flag to 0

    while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL) {
        size_t len = strlen(assembly_instruction);
        
        if (len > 0 && assembly_instruction[len - 1] == '\n') {
            assembly_instruction[len - 1] = '\0';
        }

        // Ignore empty lines
        if (strlen(assembly_instruction) == 0) {
            continue;
        }

        char* comment = strstr(assembly_instruction, "--");
        if (comment != NULL) {
            // Find the last non-space character before the comment
            char* lastNonSpace = comment - 1;
            while (lastNonSpace >= assembly_instruction && isspace(*lastNonSpace)) {
                lastNonSpace--;
            }

            // Terminate the string at the last non-space character before the comment
            *(lastNonSpace + 1) = '\0';

            // Check if the instruction is empty after removing the comment
            if (strlen(assembly_instruction) == 0) {
                // If the instruction is empty, skip this line
                continue;
            }
        }

        char mnemonic[256];
        char operand1[256];
        char operand2[256];
        char operand3[256];

        char* str=strdup(assembly_instruction);

        // Tokenize the token
        char* token = strtok(assembly_instruction, " ,\n");
        // char* token = strtok(assembly_instruction, " ,\n");
        memset(machine_code, 0, sizeof(machine_code));

        // Check if the token contains a colon
        char* colon = strchr(token, ':');
        if (colon != NULL) {
            *colon = '\0'; // Terminate the string at the colon
            char* mnemonic = colon + 1;
            //printf("%s ",token);
            char* variable=token; //save the variable
            //printf("%d %s ",counter,variable);

            varAddress[varCount]=counter;
            if (varCount < 100) {
                variables[varCount] = strdup(variable);
                varCount++;
            } else {
                printf("Maximum number of variables reached.\n");
            }
            token=strtok(NULL," ");
        }

        int register_number1;
        int register_number2;
        
        //token = strtok(NULL, ":");
        if (strcmp(token, "m") == 0) {
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;


            // Check if operand1 is a register
            if (sscanf(operand1, "r%X", &register_number1) == 1) {
                // Check if operand2 is a register
                 
                if (strcmp(operand2, "sp") == 0) {
                    stack[stackPointer] = registers[register_number1];    
                }
                else if(strncmp (operand2, "@r", 2) == 0 && sscanf(operand2, "@r%X", &register_number2) == 1){
                    content[register_number2] = registers[register_number1];
                }
                else if(strncmp (operand2, "#r", 2) == 0 && sscanf(operand2, "#r%X", &register_number2) == 1){
                    IOcontent[register_number2] = registers[register_number1];
                }
                else {
                        // Handle invalid operand2
                        printf("Invalid operand2 for mov operation.\n");
                    }
            }
            else if (strcmp(operand1, "sp") == 0 && sscanf(operand2, "r%X", &register_number2) == 1){
                // Move the value from stackPointer to register_number2
                registers[register_number2] = stackPointer;
            }
            else if(strncmp (operand1, "@r", 2) == 0 && sscanf(operand1, "@r%X", &register_number1) == 1 &&
                sscanf(operand2, "r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    registers[register_number2] = content[register_number1] ;
                } else {
                    printf("Invalid memory address in @r%d.\n", register_number1);
                }
            }
            else if(strncmp (operand1, "#r", 2) == 0 && sscanf(operand1, "#r%X", &register_number1) == 1 &&
                sscanf(operand2, "r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    registers[register_number2] = IOcontent[register_number1] ;
                } else {
                    printf("Invalid memory address in @r%d.\n", register_number1);
                }
            }

            else if(strncmp (operand1, "&r", 2) == 0 && sscanf(operand1, "&r%X", &register_number1) == 1 &&
                sscanf (operand2, "#r", 2) == 0 && sscanf(operand2, "#r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    IOcontent[register_number2] = content[register_number1] ;
                } else {
                    printf("Invalid memory address in @r%d.\n", register_number1);
                }
            }

            else if(strncmp (operand1, "#r", 2) == 0 && sscanf(operand1, "#r%X", &register_number1) == 1 &&
                sscanf (operand2, "&r", 2) == 0 && sscanf(operand2, "&r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    content[register_number2] = IOcontent[register_number1] ;
                } else {
                    printf("Invalid memory address in @r%d.\n", register_number1);
                }
            }


        }

        else if (strcmp(token, "mil") == 0) {
            token = strtok(NULL, " ,=");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
                token = strtok(NULL, " ,=");
                char* operand2 = token;

                int constant_value;
                if (sscanf(operand2, "0x%X", &constant_value) == 1) {
                    // Load the low 16 bits of the constant value into the specified register
                    registers[register_number1] = (registers[register_number1] & 0xFFFF0000) | (constant_value & 0xFFFF);
                } else {
                    // Handle invalid operand2
                    printf("Invalid operand2 for mil operation.\n");
                }
            }
        }

        else if (strcmp(token, "mih") == 0) {
            token = strtok(NULL, " ,=");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
                token = strtok(NULL, " ,=");
                char* operand2 = token;

                int constant_value;
                if (sscanf(operand2, "0x%X", &constant_value) == 1) {
                    // Load the high 16 bits of the constant value into the specified register
                    registers[register_number1] = (registers[register_number1] & 0x0000FFFF) | ((constant_value << 16) & 0xFFFF0000);
                } else {
                    // Handle invalid operand2
                    printf("Invalid operand2 for mih operation.\n");
                }
            }
        }

        else if (strcmp(token, "mi") == 0) {
            token = strtok(NULL, " ,=");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
                token = strtok(NULL, " ,=");
                char* operand2 = token;

                int constant_value;
                if (sscanf(operand2, "0x%X", &constant_value) == 1) {
                    // Load the 32-bit constant value into the specified register
                    registers[register_number1] = constant_value;
                } else {
                    // Handle invalid operand2
                    printf("Invalid operand2 for mi operation.\n");
                }
            }
        }

        else if (strcmp(token, "x=") == 0) {
            token = strtok(NULL, " ,=");
            char* operand1 = token;

            printf("HELLO X=\n");
        }
        else if (strcmp(token, "x>=") == 0) {
            token = strtok(NULL, " ,=");
        
            printf("HELLO X>=\n");
        }

        else if (strcmp(token, "x<=") == 0) {
            token = strtok(NULL, " ,=");
            
            printf("HELLO X<=\n");
        }

        else if (strcmp(token, "x<") == 0) {
            token = strtok(NULL, " ,=");

            printf("HELLO X< \n");
        }

        else if (strcmp(token, "x>") == 0) {
            token = strtok(NULL, " ,=");

            printf("HELLO X> \n");
        }

        else if (strcmp(token, "and") == 0) {
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;
            token = strtok(NULL, " , ");
            char* operand3 = token;
            int register_number1;
            int register_number2;
            int register_number3;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1 &&
            sscanf(operand3, "r%X", &register_number3) == 1) {
            // Perform the AND operation and store the result in the destination register
            registers[register_number3] = registers[register_number2] & registers[register_number1];
            }
        } else if (strcmp(token, "or") == 0) {
            strcpy(machine_code, "0F"); // Opcode: 0F
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;
            token = strtok(NULL, " , ");
            char* operand3 = token;
            int register_number1;
            int register_number2;
            int register_number3;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1 &&
            sscanf(operand3, "r%X", &register_number3) == 1) {
            registers[register_number3] = registers[register_number2] | registers[register_number1];
            }
        } else if (strcmp(token, "xor") == 0) {
            strcpy(machine_code, "10"); // Opcode: 10
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;
            token = strtok(NULL, " , ");
            char* operand3 = token;
            int register_number1;
            int register_number2;
            int register_number3;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1 &&
            sscanf(operand3, "r%X", &register_number3) == 1) {
            registers[register_number3] = registers[register_number2] ^ registers[register_number1];
            }
        } else if (strcmp(token, "not") == 0) {
            strcpy(machine_code, "11"); // Opcode: 11
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;

            int register_number1;
            int register_number2;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1) {
            registers[register_number2] = ~registers[register_number1];
            }
        } else if (strcmp(token, "add") == 0) {
            strcpy(machine_code, "12"); // Opcode: 12
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;
            token = strtok(NULL, " , ");
            char* operand3 = token;
            int register_number1;
            int register_number2;
            int register_number3;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1 &&
            sscanf(operand3, "r%X", &register_number3) == 1) {
            registers[register_number3] = registers[register_number2] + registers[register_number1];
            }
        } else if (strcmp(token, "sub") == 0) {
            strcpy(machine_code, "13"); // Opcode: 13
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;
            token = strtok(NULL, " , ");
            char* operand3 = token;
            int register_number1;
            int register_number2;
            int register_number3;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1 &&
            sscanf(operand3, "r%X", &register_number3) == 1) {
            registers[register_number3] = registers[register_number2] - registers[register_number1];
            }
        } else if (strcmp(token, "sl") == 0) {
            strcpy(machine_code, "14"); // Opcode: 14
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;

            int register_number1;
            int register_number2;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1) {
            token = strtok(NULL, " , ");
            char* operand3 = token;
            int shiftAmount;
            if (sscanf(operand3, "%X", &shiftAmount) == 1) {
                registers[register_number2] = registers[register_number1] << shiftAmount;
                } else {
                    printf("Invalid operand2 for mov operation.\n");
                }
            }
        } else if (strcmp(token, "sr") == 0) {
            strcpy(machine_code, "15"); // Opcode: 15
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;

            int register_number1;
            int register_number2;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1) {
            token = strtok(NULL, " , ");
            char* operand3 = token;
            int shiftAmount;
            if (sscanf(operand3, "%X", &shiftAmount) == 1) {
                registers[register_number2] = registers[register_number1] >> shiftAmount;
                } else {
                    printf("Invalid operand2 for mov operation.\n");
                }
            }
        } else if (strcmp(token, "call") == 0) {
        } else if (strcmp(token, "ret") == 0) {
        } else if (strcmp(token, "push") == 0) {
            // Process the 'push' token
            strcpy(machine_code, "18"); // Opcode: 18
            token = strtok(NULL, " , ");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
            // Check if the stack is not full
                if (stackPointer < sizeof(stack) / sizeof(stack[0])) {
                    // Push the value onto the stack
                    stack[stackPointer] = registers[register_number1];
                    stackPointer++; // Increment the stack pointer
                    
                } else {
                    printf("Stack overflow.\n");
                }
            }

        } else if (strcmp(token, "pop") == 0) {
            // Process the 'pop' token
            token = strtok(NULL, " , ");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
            // Check if the stack is not empty
                if (stackPointer > 0) {
                    // Pop the value from the stack
                    stackPointer--; // Decrement the stack pointer
                    registers[register_number1] = stack[stackPointer];
                } else {
                    printf("Stack underflow.\n");
                }
            }
        }
        (*currentLine)++;

        // Check if the target line is reached
        if (*currentLine >= targetLine) {
            break; // Exit the loop if the target line is reached
        }
        
        counter += increment;  // Increment the counter
        increment=1; 
    
    }
    for (int i = 0; i < 256; i++) {
            // printf("Value in register %d: %08X --- %08X\n", i, registers[i], content[i]);
            
            fprintf(outputFile,"Value in register %X: %08X --- %08X ---IO: %08X\n", i, registers[i], content[i], IOcontent[i]);
    }
    fprintf(outputFile,"Value in Stack pointer: %08X\n",stackPointer); 
    fclose(outputFile);
    
}

int main() {
    // Specify the input and output file names
    const char *inputFileName = "example2.txt";
    const char *outputFileName = "output.txt";  // Declare outputFileName here

    // Open the input file for reading
    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // Open the output file for writing (replaces existing content)
    FILE *outputFile = NULL;
    outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        perror("Error opening output file");

        fclose(inputFile);
        return 1;
    }

    int targetLine;
    char assembly_instruction[10000];
    printf("Enter the line number until which you want to display: ");
    scanf("%d", &targetLine);

    // Read and process lines until the specified target line
    int currentLine = 0;
    while (currentLine < targetLine && fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL) {
        size_t len = strlen(assembly_instruction);
        if (len > 0 && assembly_instruction[len - 1] == '\n') {
            assembly_instruction[len - 1] = '\0';
        }

        convert_and_print(inputFile, outputFile, outputFileName, targetLine, &currentLine);

        currentLine++;
    }

    // Close the files
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
