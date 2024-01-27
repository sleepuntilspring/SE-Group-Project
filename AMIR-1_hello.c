#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define SP_REGISTER_INDEX 15  // Assuming r15 is the stack pointer register
int SP_VALUE= 5;       // Set an initial value for the stack pointer
#define MEMORY_SIZE 10 // Adjust the size as needed

int memory[MEMORY_SIZE];


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

struct Register {
    int value;
    int addressContent;
    // char* content;
    int* content;
};
struct Register registers[256];

// Function to initialize the registers
void initializeRegisters() {
    for (int i = 0; i < 10; ++i) {
        registers[i].value = 0;
        registers[i].addressContent = 0;
        // registers[i].content = (char*)&memory[0]; 
        registers[i].content = (int*)&memory[0]; 
    }
}

void printRegisters() {
    
    for (int i = 0; i < 10; ++i) {
        printf("Register %d: Value=%08X,  Content=%08X\n",
       i, registers[i].value, *registers[i].content);
         
    }
    // for (int i = 0; i < 256; ++i) {
    //     printf("Register %d: Value=%08X, AddressContent=%08X, Content=%02X\n",
    //    i, registers[i].value, registers[i].addressContent, *registers[i].content);
               
    // }
}

int zeroFlag = 0;
int carryFlag=0;

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



void convert_and_print(FILE *inputFile, FILE *outputFile) {
    char assembly_instruction[10000];
    char machine_code[18]; // Increased to 18 to store the 16-character machine code and null terminator
    int increment = 1; //default address counter increment by 1 only
    int counter=0;
    char* variables[10000];
    int varCount = 0;
    int varAddress[10000];


    while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL) {
        size_t len = strlen(assembly_instruction);
        if (len > 0 && assembly_instruction[len - 1] == '\n') {
            assembly_instruction[len - 1] = '\0';
        }

        // Ignore empty lines
        if (strlen(assembly_instruction) == 0) {
            continue;
        }

        //comment --
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

            char* variable=token; //save the variable

            varAddress[varCount]=counter;
            if (varCount < 100) {
                variables[varCount] = strdup(variable);
                varCount++;
            } else {
                printf("Maximum number of variables reached.\n");
            }

            token=strtok(NULL," ");
        }

        //token = strtok(NULL, ":");
        if (strcmp(token, "and") == 0) {
            strcpy(machine_code, "0E"); // Opcode: 0E
        } else if (strcmp(token, "or") == 0) {
            strcpy(machine_code, "0F"); // Opcode: 0F
        } else if (strcmp(token, "xor") == 0) {
            strcpy(machine_code, "10"); // Opcode: 10
        } else if (strcmp(token, "not") == 0) {
            strcpy(machine_code, "11"); // Opcode: 11
        } else if (strcmp(token, "add") == 0) {
            strcpy(machine_code, "12"); // Opcode: 12
        } else if (strcmp(token, "sub") == 0) {
            strcpy(machine_code, "13"); // Opcode: 13
        } else if (strcmp(token, "sl") == 0) {
            strcpy(machine_code, "14"); // Opcode: 14
        } else if (strcmp(token, "sr") == 0) {
            strcpy(machine_code, "15"); // Opcode: 15
        } else if (strcmp(token, "call") == 0) {
            // Process the 'call' token
            strcpy(machine_code, "16"); // Opcode: 16

             // Extract the second operand (hexadecimal value)
            char* operand2 = strtok(NULL, " ");
            if (operand2 != NULL && strncmp(operand2, "0x", 2) == 0) {
                operand2 += 2;
                int op2 = hexToInt(operand2);
                sprintf(machine_code +2, "%06X", op2);

            }
            else {
                int temp;
                bool found=0;

                for (int i = 0; i < varCount; i++) {  // Corrected the loop range
                    if (strcmp(operand2, variables[i]) == 0) {

                    found=1;
                    temp = varAddress[i] - counter;


                    }
                    // Calculate the distance in 6-digit hexadecimal format using two's complement

                    }

                    // Store the current file position

                    char line[1000]; // You can adjust the buffer size as needed

                    strcat(operand2, ":");
                    //printf("operand2 is %s",operand2);
                    //while (fgets(line, sizeof(line), inputFile) != NULL) {
                        // Process the line here
                        //printf("Line: %s", line);
                    //}

                    int linesAway = 1;

                    //printf("%s",operand2);
                    if (found==0) {
                        //printf("%s in the current line.\n", operand2);
                        long currentFilePos = ftell(inputFile);

                        // Calculate the number of lines from the current line
                        // Start at 1 because it's already in the current line

                        // Read and discard lines until we find operand2 again
                        while (fgets(line, sizeof(line), inputFile) != NULL) {

                            if (strstr(line, operand2) > 0) {

                                bool found1=1;

                                //printf("%s found %d lines away.\n", operand2, linesAway);

                                long offset = -(linesAway +1); // Add 1 to account for the current line
                                fseek(inputFile, currentFilePos + offset, SEEK_SET);

                                //printf("%s",str);
                                temp = linesAway;


                                break;
                            }
                            linesAway=linesAway+1;
                            //printf("%d %s",linesAway,str);
                        }

                    }

                    char distanceHex[8]; // Allocate space for 7 characters plus null terminator
                    if (temp < 0) {
                        // Ensure the negative value is within a valid range
                        if (temp >= -0x800000 && temp < 0x800000) {
                            temp += 0x1000000; // Adjust for two's complement
                            snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                        } else {
                            // Handle the case where the value is out of range
                            snprintf(distanceHex, sizeof(distanceHex), "Out of Range");
                        }
                    } else {
                        snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                    }
                    // Update machine_code with the hexadecimal distance
                    sprintf(machine_code + 2, "%06s", distanceHex);
                   
                    fgets(line, sizeof(line), inputFile);

            }

        } 
        
        
        else if (strcmp(token, "mil") == 0) {
            char* operand1 = strtok(NULL, " ,=");
            char* operand2 = strtok(NULL, " ,=");
            // Ensure there's no leading "0x" in the hexadecimal value
            if (operand2 != NULL && strncmp(operand2, "0x", 2) == 0) {
                operand2 += 2;
            }

            // Check if the first operand is a valid register value
            if (registerToHex(operand1) >= 0) {
            // Remove the "r" from operand1
            char* registerValue = operand1 + 1;

            // Convert the immediate value to an integer
            int immediateValue = strtol(operand2, NULL, 16);

            // Perform the mil operation
            int newValue = immediateValue & 0x0000FFFF;

            // Update the register with the new value
            registers[registerToHex(registerValue)].value = newValue;

            // Print the result
            printf("Result for mil: %s = %08X\n", operand1, newValue);

            // Print or store the machine code, depending on your needs
            sprintf(machine_code, "03%02s%04X", registerValue, newValue);
        }
            else {
                fprintf(outputFile, "Invalid register name: %s\n", operand1);
                continue; // Skip to the next line
            }
        }


        else if (strcmp(token, "mih") == 0) {

            char* operand1 = strtok(NULL, " ,=");
            char* operand2 = strtok(NULL, " ,=");
            // Ensure there's no leading "0x" in the hexadecimal value
            if (operand2 != NULL && strncmp(operand2, "0x", 2) == 0) {
                operand2 += 2;
            }

            // Check if the first operand is a valid register value
            if (registerToHex(operand1) >= 0) {
                // Remove the "r" from operand1
                char* registerValue = operand1 + 1;

                // Convert the immediate value to an integer
                int immediateValue = strtol(operand2, NULL, 16);

                // Perform the mih operation
                int currentValue = registers[registerToHex(registerValue)].value;
                int newValue = (currentValue & 0x0000FFFF) | (immediateValue << 16);

                // Update the register with the new value
                registers[registerToHex(registerValue)].value = newValue;

                // Print the result
                printf("Result for mih: %s = %08X\n", operand1, newValue);

                // Print or store the machine code, depending on your needs
                sprintf(machine_code, "04%02s%04s", registerValue, operand2);
            } else {
                fprintf(outputFile, "Invalid register name: %s\n", operand1);
                continue; // Skip to the next line
            }

        }
        else if (strcmp(token, "mi") == 0) {

            char* operand1 = strtok(NULL, " ,=");
            char* operand2 = strtok(NULL, " ,=");
            // Ensure there's no leading "0x" in the hexadecimal value
            if (operand2 != NULL && strncmp(operand2, "0x", 2) == 0) {
                operand2 += 2;
            }

            // Check if the first operand is a valid register value
            if (registerToHex(operand1) >= 0) {
                // Remove the "r" from operand1
                char* registerValue = operand1 + 1;
                // Generate the machine code
                sprintf(machine_code, "05%02s0000 %08s", registerValue, operand2);
            } else {
                fprintf(outputFile, "Invalid register name: %s\n", operand1);
                continue; // Skip to the next line
            }
            increment=2;

        }
        //move
        else if (strcmp(token, "m") == 0) {
            // Process the 'm' instruction
            token = strtok(NULL, " ,");
            char* operand1 = token;
            token = strtok(NULL, " ,");
            char* operand2 = token;

            //m r1, sp
            if (operand1[0] == 'r' && strcmp(operand2, "sp") == 0) {
                // Generate the machine code
                int sourceRegister = atoi(&operand1[1]);
                SP_VALUE = registers[sourceRegister].value;

                // Move the value from sourceRegister to the stack pointer
                registers[SP_REGISTER_INDEX].value = SP_VALUE;

                sprintf(machine_code, "06%02s%02d00", &operand1[1], SP_VALUE); // Use the appropriate SP_VALUE
            }
            //m sp, r0
            else if (operand1[0] == 's' && operand1[1] == 'p' && operand1[2] == '\0' && operand2[0] == 'r' && isdigit(operand2[1])) {
                int registerIndex = atoi(&operand2[1]);

                // Move the current stack pointer value to the specified register
                registers[registerIndex].value = SP_VALUE;

                // Update the machine code accordingly
                sprintf(machine_code, "07%02d%02s00", SP_VALUE, &operand2[1]);
            }


            // //m ra, @rb
            else if (operand1[0] == 'r' && isdigit(operand1[1]) && operand2[0] == '@' && operand2[1] == 'r' && isdigit(operand2[2])) {
                
                int sourceRegister = atoi(&operand1[1]);
                int destinationRegister = atoi(&operand2[2]);

                // Get the values from the registers
                int valueToMove = registers[sourceRegister].value;
                int memoryAddress = registers[destinationRegister].value;

                // Move the data from the register to the memory location pointed to by the destination register
                *registers[destinationRegister].content = valueToMove;

                // Update the machine code accordingly (if needed)
                sprintf(machine_code, "08%02s%02s00", &operand1[1], &operand2[2]);
            }


            // m @ra, rb
            else if (operand1[0] == '@' && operand1[1] == 'r' && isdigit(operand1[2]) && operand2[0] == 'r' && isdigit(operand2[1])) {
                // Extract register indices from operands
                int sourceRegisterIndex = atoi(&operand1[2]);
                int destinationRegisterIndex = atoi(&operand2[1]);

                // Get the memory address pointed to by the source register
                int memoryAddress = registers[sourceRegisterIndex].value;

                // Load the value from memory into the destination register
                int loadedValue = *(char *)registers[destinationRegisterIndex].content;

                // Update the register with the loaded value
                registers[destinationRegisterIndex].value = loadedValue;

                // Update the machine code accordingly
                sprintf(machine_code, "09%02d%02d00", sourceRegisterIndex, destinationRegisterIndex);
            }


            else if (operand1[0] == 'r' && operand2[0] == '#' && operand2[1] == 'r' && isdigit(operand2[2])) {
                sprintf(machine_code, "0A%02s%02s00", &operand1[1], &operand2[2]);
            }
            else if (operand1[0] == '#' && operand1[1] == 'r'&& isdigit(operand1[2]) && operand2[0] == 'r' && isdigit(operand2[1])) {
                sprintf(machine_code, "0B%02s%02s00", &operand1[2], &operand2[1]); // Example values, adjust as needed
            }
            else if (operand1[0] == '@' && operand1[1] == 'r'&& isdigit(operand1[2]) && operand2[0] == '#'&& operand2[1] == 'r' && isdigit(operand2[2])) {
                sprintf(machine_code, "0C%02s%02s00", &operand1[2], &operand2[2]); // Example values, adjust as needed
            }
            else if (operand1[0] == '#' && operand1[1] == 'r'&& isdigit(operand1[2]) && operand2[0] == '@'&& operand2[1] == 'r' && isdigit(operand2[2])) {
                sprintf(machine_code, "0D%02s%02s00", &operand1[2], &operand2[2]);
            }

            else {
                // Invalid operands for 'm' instruction
                fprintf(outputFile, "Invalid instruction: %s\n", assembly_instruction);
                continue; // Skip to the next line
            }
        }
        
        
        
        
        
        
        else if (strcmp(token, "ret") == 0) {
            // Process the 'ret' token
            strcpy(machine_code, "17000000"); // Opcode: 17
        } else if (strcmp(token, "push") == 0) {
            // Process the 'push' token
            strcpy(machine_code, "18"); // Opcode: 18

        } else if (strcmp(token, "pop") == 0) {
            // Process the 'pop' token
            strcpy(machine_code, "19"); // Opcode: 19

        } else if (strcmp(token, "j") == 0) {
            // Process the 'j' token
            strcpy(machine_code, "1A"); // Opcode: 1A
             // Extract the second operand (hexadecimal value)
            char* operand2 = strtok(NULL, " ");
            if (operand2 != NULL && strncmp(operand2, "0x", 2) == 0) {
                operand2 += 2;
                int op2 = hexToInt(operand2);
                sprintf(machine_code +2, "%06X", op2);

            }
            else {
                int temp;
                bool found=0;


                for (int i = 0; i < varCount; i++) {  // Corrected the loop range
                    if (strcmp(operand2, variables[i]) == 0) {
                    found=1;
                    temp = varAddress[i] - counter;


                    }
                    // Calculate the distance in 6-digit hexadecimal format using two's complement

                    }

                    // Store the current file position

                    char line[1000]; // You can adjust the buffer size as needed

                    int linesAway = 1;
                    strcat(operand2, ":");

                    //printf("%s",operand2);
                    if (found==0) {
                        //printf("%s in the current line.\n", operand2);
                        long currentFilePos = ftell(inputFile);

                        // Calculate the number of lines from the current line
                         // Start at 1 because it's already in the current line

                        // Read and discard lines until we find operand2 again
                        while (fgets(line, sizeof(line), inputFile) != NULL) {

                            if (strstr(line, operand2) > 0) {

                                bool found1=1;

                                //printf("%s found %d lines away.\n", operand2, linesAway);

                                long offset = -(linesAway +1); // Add 1 to account for the current line
                                fseek(inputFile, currentFilePos + offset, SEEK_SET);

                                //printf("%s",str);
                                temp = linesAway;


                                break;
                            }
                            linesAway=linesAway+1;
                            //printf("%d %s",linesAway,str);
                        }

                    }

                    char distanceHex[8]; // Allocate space for 7 characters plus null terminator
                    if (temp < 0) {
                        // Ensure the negative value is within a valid range
                        if (temp >= -0x800000 && temp < 0x800000) {
                            temp += 0x1000000; // Adjust for two's complement
                            snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                        } else {
                            // Handle the case where the value is out of range
                            snprintf(distanceHex, sizeof(distanceHex), "Out of Range");
                        }
                    } else {
                        snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                    }
                    // Update machine_code with the hexadecimal distance
                    sprintf(machine_code + 2, "%06s", distanceHex);
                    // fgets(line, sizeof(line), inputFile);
            }
        }
        
        else if (strcmp(token, "x=") == 0) {
            token = strtok(NULL, " ");
            char *operand2 = token;

            int jumpOffset;

            if (operand2 != NULL) {

                if (is_valid_hexadecimal(operand2)) {
                    // If valid hexadecimal, treat as a compare instruction
                    int sourceRegister1 = atoi(operand1 + 1);
                    int sourceRegister2 = atoi(operand2 + 1);

                    // Perform subtraction
                    int result = registers[sourceRegister1].value - registers[sourceRegister2].value;

                    if (result == 0) {
                        zeroFlag = 1;
                        // Set machine_code to '1B' followed by zeros (no jump needed)
                        sprintf(machine_code, "1B000000");
                    }
                    else {
                        // Jump to PC plus (d23-d0)
                        jumpOffset = hexToInt(operand2) - 1;
                        // Set machine_code to '1B' followed by the hexadecimal jump offset
                        sprintf(machine_code, "1B%06X", jumpOffset);
                    }
                }
                else {

                    int temp;
                    bool found = 0;
                    strcpy(machine_code, "1B");

                    for (int i = 0; i < varCount; i++) {  
                        if (strcmp(operand2, variables[i]) == 0) {
                            found = 1;
                            temp = varAddress[i] - counter;
                            break;
                        }
                    }

                    if (!found) {
                        // Handle the case where the variable is not found
                        fprintf(outputFile, "Variable not found: %s\n", operand2);
                        continue;
                    }

                    // Calculate jumpOffset
                    jumpOffset = (temp < 0) ? temp : (temp - 1);
                    char distanceHex[8]; 

                    if (temp < 0) {
                        // Ensure the negative value is within a valid range
                        if (temp >= -0x800000 && temp < 0x800000) {
                            temp += 0x1000000; // Adjust for two's complement
                            snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                        } else {
                            // Handle the case where the value is out of range
                            snprintf(distanceHex, sizeof(distanceHex), "Out of Range");
                        }
                    } else {
                        snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                    }

                    // Update machine_code with the hexadecimal distance
                    sprintf(machine_code + 2, "%06s", distanceHex);
                }
            } else {
                // Handle cases where the syntax is incorrect
                fprintf(outputFile, "Invalid 'x' instruction: %s\n", assembly_instruction);
                continue;
            }   
        }


        else if (strcmp(token, "x<=") == 0) {
            token = strtok(NULL, " ");
            char *operand2 = token;

            int jumpOffset;

            if (operand2 != NULL) {
                int sourceRegister1 = atoi(&operand1[1]);
                int sourceRegister2 = atoi(&operand2[1]);

                if (registers[sourceRegister1].value <= registers[sourceRegister2].value) {
                    zeroFlag = 1;
                    // Set machine_code to '1E' followed by zeros (no jump needed)
                    sprintf(machine_code, "1E000000");
                } else {
                    // Jump to PC plus (d23-d0)
                    if (is_valid_hexadecimal(operand2)) {
                        jumpOffset = hexToInt(operand2) - 1;
                    } else {
                        int temp;
                        bool found = 0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand2, variables[i]) == 0) {
                                found = 1;
                                temp = varAddress[i] - counter;
                                break;
                            }
                        }

                        if (!found) {
                            // Handle the case where the variable is not found
                            fprintf(outputFile, "Variable not found: %s\n", operand2);
                            continue;
                        }

                        // Calculate jumpOffset
                        jumpOffset = (temp < 0) ? temp : (temp - 1);
                        char distanceHex[8]; // Allocate space for 7 characters plus null terminator

                        if (temp < 0) {
                            // Ensure the negative value is within a valid range
                            if (temp >= -0x800000 && temp < 0x800000) {
                                temp += 0x1000000; // Adjust for two's complement
                                snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                            } else {
                                // Handle the case where the value is out of range
                                snprintf(distanceHex, sizeof(distanceHex), "Out of Range");
                            }
                        } else {
                            snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                        }

                        // Update machine_code with the hexadecimal distance
                        sprintf(machine_code + 2, "%06s", distanceHex);
                    }
                }
            } else {
                // Handle cases where the syntax is incorrect
                fprintf(outputFile, "Invalid 'x' instruction: %s\n", assembly_instruction);
                continue;
            }   
        }


        else if (strcmp(token, "x>=") == 0) {
            token = strtok(NULL, " ");
            char *operand2 = token;

            int jumpOffset;

            if (operand2 != NULL) {
                int sourceRegister1 = atoi(&operand1[1]);
                int sourceRegister2 = atoi(&operand2[1]);

                if (registers[sourceRegister1].value >= registers[sourceRegister2].value) {
                    zeroFlag = 1;
                    // Set machine_code to '1F' followed by zeros (no jump needed)
                    sprintf(machine_code, "1F000000");
                } else {
                    // Jump to PC plus (d23-d0)
                    if (is_valid_hexadecimal(operand2)) {
                        jumpOffset = hexToInt(operand2) - 1;
                    } else {
                        int temp;
                        bool found = 0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand2, variables[i]) == 0) {
                                found = 1;
                                temp = varAddress[i] - counter;
                                break;
                            }
                        }

                        if (!found) {
                            // Handle the case where the variable is not found
                            fprintf(outputFile, "Variable not found: %s\n", operand2);
                            continue;
                        }

                        // Calculate jumpOffset
                        jumpOffset = (temp < 0) ? temp : (temp - 1);
                        char distanceHex[8]; // Allocate space for 7 characters plus null terminator

                        if (temp < 0) {
                            // Ensure the negative value is within a valid range
                            if (temp >= -0x800000 && temp < 0x800000) {
                                temp += 0x1000000; // Adjust for two's complement
                                snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                            } else {
                                // Handle the case where the value is out of range
                                snprintf(distanceHex, sizeof(distanceHex), "Out of Range");
                            }
                        } else {
                            snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                        }

                        // Update machine_code with the hexadecimal distance
                        sprintf(machine_code + 2, "%06s", distanceHex);
                    }
                }
            } else {
                // Handle cases where the syntax is incorrect
                fprintf(outputFile, "Invalid 'x' instruction: %s\n", assembly_instruction);
                continue;
            }   
        }


        else if (strcmp(token, "x<") == 0) {
            token = strtok(NULL, " ");
            char *operand2 = token;

            int jumpOffset;

            if (operand2 != NULL) {

                if (is_valid_hexadecimal(operand2)) {
                    int sourceRegister1 = atoi(&operand1[1]);
                    int sourceRegister2 = atoi(&operand2[1]);

                    if (registers[sourceRegister1].value < registers[sourceRegister2].value) {
                        zeroFlag = 0;
                        carryFlag = 0;
                        // Set machine_code to '1C' followed by zeros (no jump needed)
                        sprintf(machine_code, "1C000000");
                    } else {
                        // Jump to PC plus (d23-d0)
                        jumpOffset = hexToInt(operand2) - 1;
                        // Set machine_code to '1C' followed by the hexadecimal jump offset
                        sprintf(machine_code, "1C%06X", jumpOffset);
                    }
                }
                else {

                    int temp;
                    bool found = 0;
                    strcpy(machine_code, "1C");

                    for (int i = 0; i < varCount; i++) {  
                        if (strcmp(operand2, variables[i]) == 0) {
                            found = 1;
                            temp = varAddress[i] - counter;
                            break;
                        }
                    }

                    if (!found) {
                        // Handle the case where the variable is not found
                        fprintf(outputFile, "Variable not found: %s\n", operand2);
                        continue;
                    }

                    // Calculate jumpOffset
                    jumpOffset = (temp < 0) ? temp : (temp - 1);
                    char distanceHex[8]; 

                    if (temp < 0) {
                        // Ensure the negative value is within a valid range
                        if (temp >= -0x800000 && temp < 0x800000) {
                            temp += 0x1000000; // Adjust for two's complement
                            snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                        } else {
                            // Handle the case where the value is out of range
                            snprintf(distanceHex, sizeof(distanceHex), "Out of Range");
                        }
                    } else {
                        snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                    }

                    // Update machine_code with the hexadecimal distance
                    sprintf(machine_code + 2, "%06s", distanceHex);
                }
            } else {
                // Handle cases where the syntax is incorrect
                fprintf(outputFile, "Invalid 'x' instruction: %s\n", assembly_instruction);
                continue;
            }   
        }

        else if (strcmp(token, "x>") == 0) {
            token = strtok(NULL, " ");
            char *operand2 = token;

            int jumpOffset;

            if (operand2 != NULL) {

                if (is_valid_hexadecimal(operand2)) {
                    int sourceRegister1 = atoi(&operand1[1]);
                    int sourceRegister2 = atoi(&operand2[1]);

                    if (registers[sourceRegister1].value > registers[sourceRegister2].value) {
                        zeroFlag = 0;
                        carryFlag = 0;
                        // Set machine_code to '1D' followed by zeros (no jump needed)
                        sprintf(machine_code, "1D000000");
                    } else {
                        // Jump to PC plus (d23-d0)
                        jumpOffset = hexToInt(operand2) - 1;
                        // Set machine_code to '1D' followed by the hexadecimal jump offset
                        sprintf(machine_code, "1D%06X", jumpOffset);
                    }
                }
                else {

                    int temp;
                    bool found = 0;
                    strcpy(machine_code, "1D");

                    for (int i = 0; i < varCount; i++) {  
                        if (strcmp(operand2, variables[i]) == 0) {
                            found = 1;
                            temp = varAddress[i] - counter;
                            break;
                        }
                    }

                    if (!found) {
                        // Handle the case where the variable is not found
                        fprintf(outputFile, "Variable not found: %s\n", operand2);
                        continue;
                    }

                    // Calculate jumpOffset
                    jumpOffset = (temp < 0) ? temp : (temp - 1);
                    char distanceHex[8]; 

                    if (temp < 0) {
                        // Ensure the negative value is within a valid range
                        if (temp >= -0x800000 && temp < 0x800000) {
                            temp += 0x1000000; // Adjust for two's complement
                            snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                        } else {
                            // Handle the case where the value is out of range
                            snprintf(distanceHex, sizeof(distanceHex), "Out of Range");
                        }
                    } else {
                        snprintf(distanceHex, sizeof(distanceHex), "%06X", temp);
                    }

                    // Update machine_code with the hexadecimal distance
                    sprintf(machine_code + 2, "%06s", distanceHex);
                }
            } else {
                // Handle cases where the syntax is incorrect
                fprintf(outputFile, "Invalid 'x' instruction: %s\n", assembly_instruction);
                continue;
            }   
        }

        

        if (strcmp(token, "sl") == 0 || strcmp(token, "sr") == 0) {
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;

            int register_number1;
            int register_number2;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1) {
            sprintf(machine_code + 2, "%02X", register_number1);
            sprintf(machine_code + 4, "%02X", register_number2);
            }
            token = strtok(NULL, " , ");
            //int offset = operand3[1] - '0';
            int shiftAmount = atoi(token);
            char shiftAmountHex[3];
            sprintf(shiftAmountHex, "%02X", shiftAmount);
            strcpy(machine_code + 6, shiftAmountHex);
            //sprintf(machine_code +6 , "%02X", shiftAmount);

        } 
        
        else if (strcmp(token, "and") == 0 || strcmp(token, "or") == 0 || strcmp(token, "xor") == 0 || strcmp(token, "add") == 0 || strcmp(token, "sub") == 0){
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
            sprintf(machine_code + 2, "%02X", register_number1);
            sprintf(machine_code + 4, "%02X", register_number2);
            sprintf(machine_code + 6, "%02X", register_number3);
            }
        } 

        
        else if (strcmp(token, "not") == 0) {
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;

            int register_number1;
            int register_number2;
            if (sscanf(operand1, "r%X", &register_number1) == 1 &&
            sscanf(operand2, "r%X", &register_number2) == 1) {
            sprintf(machine_code + 2, "%02X", register_number1);
            sprintf(machine_code + 4, "%02X", register_number2);
            }

            sprintf(machine_code + 6 , "00");
        } else if (strcmp(token, "push") == 0 || strcmp(token, "pop") == 0){
            token = strtok(NULL, " , ");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
            sprintf(machine_code + 2, "%02X", register_number1);
            }
            sprintf(machine_code + 4 , "0000");
        }

        // Output machine code to the output file
        fprintf(outputFile, "%3X  %20s  | %s\n",counter, machine_code, str);


        counter += increment;  // Increment the counter
        increment=1;
    }

    fprintf(outputFile, "\n\n%s", "Symbol Table");
    fprintf(outputFile, "\n%s", "------------");
    fprintf(outputFile, "\n%s%11s\t\t\t%5s","No","Label","Address");
    for(int i=1;i<=varCount;i++){
        fprintf(outputFile, "\n%X\t",i);
        fprintf(outputFile, "%12s",variables[i-1]);
        fprintf(outputFile, "%8X\t\t",varAddress[i-1]);
    }
}

int main() {
    initializeRegisters();

    registers[0].value = 10;
    registers[1].value = 10;
    registers[3].value = 42;  // Set value in r3
    registers[4].value = 100; // Set memory address in r4
    registers[5].value = 66;
    registers[9].value = 70;


    // Specify the input and output file names
    const char *inputFileName = "example.txt";
    const char *outputFileName = "output.txt";

    // Open the input file for reading
    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // Open the output file for writing the binary representations
    FILE *outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }
    fprintf(outputFile, "%7s %18s %20s\n","Counter","Machine Code","Assembly Language");

    // Call the conversion and printing function
    convert_and_print(inputFile, outputFile);

    printRegisters();

    fclose(inputFile);
    fclose(outputFile);

    printf("the latest sp_value: %d\n",SP_VALUE);
    printf("Instructions have been translated and saved to %s.\n", outputFileName);

    return 0;
}

