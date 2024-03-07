#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define PROGRAM_START_ADDRESS 0x00000000
#define DATA_START_ADDRESS    0x00000100
#define TOP_OF_STACK_ADDRESS  0x00001F00
#define BOTTOM_OF_STACK_ADDRESS 0x00001FF0
#define TOP_OF_MAIN_MEMORY 0x00001FFF
#define STACK_SIZE (BOTTOM_OF_STACK_ADDRESS-TOP_OF_STACK_ADDRESS) 
#define MAX_INSTRUCTION_SIZE 20
#define NUM_INSTRUCTIONS 256
#define MEMORY_SIZE 4096
#define IO_SIZE 8192
#define MAX_LINES 10000

// Memory
unsigned int content[MEMORY_SIZE];

// IO Ports
unsigned int IOcontent[IO_SIZE];

unsigned int Z_flag = 0;
unsigned int C_flag = 0;
unsigned int N_flag = 0;
long varFilePos = -1; // Variable to store the file position where the variable is found
bool exect = false;

char stack[STACK_SIZE];
    

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
bool reset_1_command = false;
bool reset_2_command = false;

// void convert_and_print(FILE *inputFile, FILE *outputFile) {
int convert_and_print(FILE *inputFile, FILE *outputFile, const char *outputFileName, FILE *get_linesway_file){
        
    // Array to store register values from 0 to 255
    int registers[256] = {0};
    int content[256] = {0};
    int IOcontent[256] = {0};
    char INScontent[256][9];
    char registerStr[15];
    char memoryAddressStr[15];
    char ioPortStr[15];
    char insAddressStr[15];

    char assembly_instruction[10000];
    char machine_code[18]; // Increased to 18 to store the 16-character machine code and null terminator
    int counter=0;
    char* variables[10000];
    
    int varAddress[10000];
    int stack[STACK_SIZE]; // Stack memory
    int stackPointer = TOP_OF_STACK_ADDRESS;
    int increment = 1;
    int currentPointer = PROGRAM_START_ADDRESS;
    int programCounter = PROGRAM_START_ADDRESS + increment;
    int varCount = 0;

    int lineNumber = 0;

    FILE *errorFile;
    errorFile = fopen("error.log", "w"); // Opens the file in write mode

    if (errorFile == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL) {
        lineNumber++;
        
        FILE *get_linesway_file = fopen("getLines.txt", "a");
        // fprintf(get_linesway_file,"found 1");
        // printf("%s",assembly_instruction);
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

        //printf("\ntoken: %s",token);
        currentPointer= insAddressStr;

        #define MAX_VARIABLES 100
        #define MAX_LINE_LENGTH 1000
        // Variables to store variable names and their corresponding file positions
        char* variables[MAX_VARIABLES];
        long varFilePositions[MAX_VARIABLES];
        int variablesPC[MAX_VARIABLES];

        // Buffer to store each line read from the file
        char line[MAX_LINE_LENGTH];

        long currentFilePos;
        // Check if the token contains a colon
        char* colon = strchr(token, ':');
        if (colon != NULL) {
            *colon = '\0'; // Terminate the string at the colon
            char* variable = token; // Save the variable before the colon
            
            //printf("%d %s ", counter, variable);
            
            varAddress[varCount] = counter;
            if (varCount < 100) {
                // Save the current file position and the variable name
                varFilePositions[varCount] = ftell(inputFile) -len-1;
                variables[varCount] = strdup(variable);
                variablesPC[varCount] = programCounter;
                //printf("Variable saved: %s\n", variables[varCount]);
                varCount++;
            } else {
                printf("Maximum number of variables reached.\n");
            }
            token = strtok(NULL, " ");
        }


        int register_number1;
        int register_number2;
        
        if (strcmp(token, "m") == 0) {
            token = strtok(NULL, " , ");
            char* operand1 = token;
            token = strtok(NULL, " , ");
            char* operand2 = token;

            // Check if operand1 is a register
            if (sscanf(operand1, "r%X", &register_number1) == 1) {              
                 
                if (strcmp(operand2, "sp") == 0) {
                    stackPointer= registers[register_number1];
                } 
                else if(strncmp (operand2, "@r", 2) == 0 && sscanf(operand2, "@r%X", &register_number2) == 1){
                    int memoryAddress = registers[register_number2];
                    content[memoryAddress] = registers[register_number1];
                }

                else if (strncmp(operand2, "#r", 2) == 0 && sscanf(operand2 + 1, "r%X", &register_number2) == 1) {
                    int memoryAddress = registers[register_number2];
                    IOcontent[memoryAddress] = registers[register_number1];
                }
                else {
                        // Handle invalid operand2
                        fprintf(errorFile, "Line %d: Invalid operand2 for m operation.\n", lineNumber);
                    }
            }
            else if (strcmp(operand1, "sp") == 0 && sscanf(operand2, "r%X", &register_number2) == 1){
                // Move the value from stackPointer to register_number2
                registers[register_number2] = stackPointer;
            }
            else if(strncmp (operand1, "@r", 2) == 0 && sscanf(operand1, "@r%X", &register_number1) == 1 &&
                sscanf(operand2, "r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    int memoryAddress = registers[register_number1];
                    registers[register_number2] = content[memoryAddress] ;
                } else {
                    fprintf(errorFile,"Line %d: Invalid memory address in @r%d for m operation.\n", lineNumber, register_number1);
                }
            }
            else if(strncmp (operand1, "#r", 2) == 0 && sscanf(operand1, "#r%X", &register_number1) == 1 &&
                sscanf(operand2, "r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    int memoryAddress = registers[register_number1];
                    registers[register_number2] = IOcontent[memoryAddress] ;
                } else {
                    fprintf(errorFile,"Line %d: Invalid memory address in @r%d for m operation.\n", lineNumber, register_number1);
                }
            }

            else if(strncmp (operand1, "@r", 2) == 0 && sscanf(operand1, "@r%X", &register_number1) == 1 &&
                sscanf (operand2, "#r", 2) == 0 && sscanf(operand2, "#r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    int memoryAddress = registers[register_number1];
                    IOcontent[register_number2] = content[memoryAddress] ;
                } else {
                    fprintf(errorFile,"Line %d: Invalid memory address in @r%d for m operation.\n", lineNumber, register_number1);
                }
            }

            else if(strncmp (operand1, "#r", 2) == 0 && sscanf(operand1, "#r%X", &register_number1) == 1 &&
                sscanf (operand2, "@r", 2) == 0 && sscanf(operand2, "@r%X", &register_number2) == 1) {
                if (register_number1 >= 0 && register_number1 < 256) {
                    int memoryAddress = registers[register_number1];
                    content[register_number2] = IOcontent[memoryAddress] ;
                } else {
                    fprintf(errorFile,"Line %d: Invalid memory address in @r%d for m operation.\n", lineNumber, register_number1);
                }
            } else {
                fprintf(errorFile,"Line %d: Invalid operand1 for m operation.\n", lineNumber);
            }
        }

        else if (strcmp(token, "mil") == 0) {
            // Assuming the instruction is a mil, update the instruction memory with the corresponding machine code
            int opcode = 0x03;  // Mil opcode
            int register_number1;

            token = strtok(NULL, " ,=");
            char* operand1 = token;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
                token = strtok(NULL, " ,=");
                char* operand2 = token;

                int constant_value;
                if (sscanf(operand2, "0x%X", &constant_value) == 1) {
                    // Generate the machine code
                    int machine_code = (opcode << 24) | (register_number1 << 16) | (constant_value & 0xFFFF);

                    //Do operation
                    registers[register_number1] = (registers[register_number1] & 0xFFFF0000) | (constant_value & 0xFFFF);
                    
                    // Store the machine code in the instruction memory
                    sprintf(INScontent[counter], "%08X", machine_code);
                    

                } else {
                    // Handle invalid operand2
                    fprintf(errorFile,"Line %d: Invalid operand2 for mil operation.\n", lineNumber);
                }
            } else {
                fprintf(errorFile,"Line %d: Invalid operand1 for mil operation.\n", lineNumber);
            }
        }

        else if (strcmp(token, "mih") == 0) {
            // Assuming the instruction is a mih, update the instruction memory with the corresponding machine code
            int opcode = 0x04;  // Mih opcode
            int register_number1;

            token = strtok(NULL, " ,=");
            char* operand1 = token;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
                token = strtok(NULL, " ,=");
                char* operand2 = token;

                int constant_value;
                if (sscanf(operand2, "0x%X", &constant_value) == 1) {
                    // Generate the machine code
                    int machine_code = (opcode << 24) | (register_number1 << 16) | (constant_value & 0xFFFF);

                    //Do operation
                    registers[register_number1] = (registers[register_number1] & 0x0000FFFF) | ((constant_value << 16) & 0xFFFF0000);

                    // Store the machine code in the instruction memory
                    sprintf(INScontent[counter], "%08X", machine_code);
                } else {
                    // Handle invalid operand2
                    fprintf(errorFile,"Line %d: Invalid operand2 for mih operation.\n", lineNumber);
                }
            } else {
                fprintf(errorFile,"Line %d: Invalid operand1 for mih operation.\n", lineNumber);
            }
        }

        else if (strcmp(token, "mi") == 0) {
            // Assuming the instruction is a mi, update the instruction memory with the corresponding machine code
            int opcode = 0x05;  // Mi opcode
            int register_number1;

            token = strtok(NULL, " ,=");
            char* operand1 = token;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
                // Generate the machine code
                int machine_code = (opcode << 24) | (register_number1 << 16);

                // Store the machine code in the instruction memory
                sprintf(INScontent[counter], "%08X", machine_code);

                // Increment the counter to the next address
                counter++;
                
                // Assuming the next address should store the constant value
                token = strtok(NULL, " ,=");
                int constant_value;
                if (sscanf(token, "0x%X", &constant_value) == 1) {
                    
                    //Load 32 bit const value into the register
                    registers[register_number1] = constant_value;
                    
                    // Store the constant value in the next address
                    sprintf(INScontent[counter], "%08X", constant_value);
                    increment=2;
                } else {
                    // Handle invalid operand2
                    fprintf(errorFile,"Line %d: Invalid operand2 for mi operation.\n", lineNumber);
                }
            } else {
                // Handle invalid operand1
                fprintf(errorFile,"Line %d: Invalid operand1 for mi operation.\n", lineNumber);
            }
        }
        
        else if (strcmp(token, "x=") == 0) {

            // Check if Z_flag or C_flag is 0
            if (Z_flag == 0) {
                
                long currentFilePos2;
                token = strtok(NULL, " ");
                char *operand1 = token;

                int address;
                
                int linesAway =1;
                int result = sscanf(operand1, "0x%X", &address);
                
                // Check if sscanf successfully read a value
                if (result == 1) {

                    // Save the current position in the file                
                    result = 0;
                    rewind(inputFile);
                    lineNumber = 0;

                    programCounter = 0;
                        
                    // Scan through lines and decrement the address accordingly
                    while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL && address - 1 != programCounter) {
                        lineNumber++;
                        // printf("Address: %d PC:%d\n",address-1,programCounter);
                        size_t len = strlen(assembly_instruction);

                        if (len > 0 && assembly_instruction[len - 1] == '\n') {
                            assembly_instruction[len - 1] = '\0';
                        }

                        // Ignore empty lines
                        if (strlen(assembly_instruction) == 0) {
                            continue;
                        }

                        // Check if the instruction is 'mi' 
                        if (strstr(assembly_instruction, "mi") != NULL) {
                            programCounter += 2;
                            linesAway -= 2;
                        } else {
                            // Decrement the address by 1 for other instructions
                            programCounter += 1;
                            linesAway-=1;
                        }

                        // Increment the program counter
                        programCounter++;
                        fprintf(get_linesway_file,"found %d\n", linesAway);
                        //printf("pc is %d",programCounter);
                        currentFilePos2 = ftell(inputFile);
                    }

                    // Jump to the calculated position in the file
                    fseek(inputFile, currentFilePos2, SEEK_SET);

                } else {
                    // Handle the case where operand1 is a variable name
                    if (operand1 != NULL) {
                        // Now, you can proceed with the rest of your code to handle the valid 'x=' case.

                        int temp;
                        bool found = 0;
                        int virtualLinesAway=0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand1, variables[i]) == 0) {
                                found = 1;
                                fseek(inputFile, varFilePositions[i], SEEK_SET);
                                programCounter = variablesPC[i]-1;
                                break;
                            }
                        }

                        char line[1000];
                        int linesAway = 1;
                            
                        strcat(operand1, ":");
                        if (found == 0) {
                            // Read and discard lines until we find operand1 again
                            while (fgets(line, sizeof(line), inputFile) != NULL) {
                                lineNumber++;
                                size_t len2 = strlen(line);

                                if (strstr(line, operand1) > 0) {
                                    printf("%s found %d lines away.\n\n", operand1, linesAway);
                                    fprintf(get_linesway_file,"found %d\n", linesAway);
                                    virtualLinesAway += linesAway;
                                    long currentFilePos = ftell(inputFile) - len2 - 1;
                                    long offset = -(linesAway + 1);
                                    fseek(inputFile, currentFilePos, SEEK_SET);
                                    temp = linesAway;
                                    break;
                                }

                                // Count virtual lines for 'mi' commands
                                if (strstr(line, "mi") != NULL) {
                                    virtualLinesAway++;
                                }
                                linesAway = linesAway + 1;
                            }
                        }

                        // Adjust programCounter based on virtualLinesAway
                    programCounter += virtualLinesAway;
                    } else {
                        // Handle cases where the syntax is incorrect
                        fprintf(errorFile, "Line %d: Invalid 'x' instruction: %s\n", lineNumber, assembly_instruction);
                        continue;
                    }
                }
            } else {
                // Handle the case where Z_flag and C_flag are not 0
                return exect = true;
            }
        } else if (strcmp(token, "x>=") == 0) {
            
            // Check if Z_flag or C_flag is 0
            if (Z_flag == 1 || C_flag == 1) {
                long currentFilePos2;
            token = strtok(NULL, " ");
            char *operand1 = token;

            int address;
            
            int linesAway =1;
            int result = sscanf(operand1, "0x%X", &address);
            
                // Check if sscanf successfully read a value
                if (result == 1) {
                // Save the current position in the file         
                            
                result = 0;
                rewind(inputFile);
                lineNumber = 0;

                programCounter = 0;
                
                // Scan through lines and decrement the address accordingly
                while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL && address - 1 != programCounter) {
                    lineNumber++;
                    // printf("Address: %d PC:%d\n",address-1,programCounter);
                    size_t len = strlen(assembly_instruction);

                    if (len > 0 && assembly_instruction[len - 1] == '\n') {
                        assembly_instruction[len - 1] = '\0';
                    }

                    // Ignore empty lines
                    if (strlen(assembly_instruction) == 0) {
                        continue;
                    }

                    // Check if the instruction is 'mi' 
                    if (strstr(assembly_instruction, "mi") != NULL) {
                        programCounter += 2;
                        linesAway -= 2;
                    } else {
                        // Decrement the address by 1 for other instructions
                        programCounter += 1;
                        linesAway-=1;
                    }

                    // Increment the program counter
                    programCounter++;
                    fprintf(get_linesway_file,"found %d\n", linesAway);
                    //printf("pc is %d",programCounter);
                    currentFilePos2 = ftell(inputFile);
                }

                // Jump to the calculated position in the file
                fseek(inputFile, currentFilePos2, SEEK_SET);

            } else {
                    // Handle the case where operand1 is a variable name
                    if (operand1 != NULL) {
                        // Now, you can proceed with the rest of your code to handle the valid 'x=' case.

                        int temp;
                        bool found = 0;
                        int virtualLinesAway=0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand1, variables[i]) == 0) {
                                found = 1;
                                fseek(inputFile, varFilePositions[i], SEEK_SET);
                                programCounter = variablesPC[i]-1;
                                break;
                            }
                        }

                        char line[1000];
                        int linesAway = 1;
                        
                        strcat(operand1, ":");
                        if (found == 0) {
                            // Read and discard lines until we find operand1 again
                            while (fgets(line, sizeof(line), inputFile) != NULL) {
                                lineNumber++;
                                size_t len2 = strlen(line);

                                if (strstr(line, operand1) > 0) {
                                    printf("%s found %d lines away.\n\n", operand1, linesAway);
                                    fprintf(get_linesway_file,"found %d\n", linesAway);
                                    virtualLinesAway += linesAway;
                                    long currentFilePos = ftell(inputFile) - len2 - 1;
                                    long offset = -(linesAway + 1);
                                    fseek(inputFile, currentFilePos, SEEK_SET);
                                    temp = linesAway;
                                    break;
                                }
                                // Count virtual lines for 'mi' commands
                                if (strstr(line, "mi") != NULL) {
                                    virtualLinesAway++;
                                }
                                linesAway = linesAway + 1;
                            }
                        }

                        // Adjust programCounter based on virtualLinesAway
                        programCounter += virtualLinesAway;
                    } else {
                        // Handle cases where the syntax is incorrect
                        fprintf(errorFile, "Line %d: Invalid 'x' instruction: %s\n", lineNumber, assembly_instruction);
                        continue;
                    }
                }
            }
            else {
                // Handle the case where Z_flag and C_flag are not 0
                return exect = true;
            }
        }

        else if (strcmp(token, "x<=") == 0) {

            // Check if Z_flag or C_flag is 0
            if (Z_flag == 0 || C_flag == 0) {
                long currentFilePos2;
            token = strtok(NULL, " ");
            char *operand1 = token;

            int address;
            
            int linesAway =1;
            int result = sscanf(operand1, "0x%X", &address);
            
                // Check if sscanf successfully read a value
                if (result == 1) {
                // Save the current position in the file         
                            
                result = 0;
                rewind(inputFile);
                lineNumber = 0;

                programCounter = 0;
                
                // Scan through lines and decrement the address accordingly
                while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL && address - 1 != programCounter) {
                    lineNumber++;
                    // printf("Address: %d PC:%d\n",address-1,programCounter);
                    size_t len = strlen(assembly_instruction);

                    if (len > 0 && assembly_instruction[len - 1] == '\n') {
                        assembly_instruction[len - 1] = '\0';
                    }

                    // Ignore empty lines
                    if (strlen(assembly_instruction) == 0) {
                        continue;
                    }

                    // Check if the instruction is 'mi' 
                    if (strstr(assembly_instruction, "mi") != NULL) {
                        programCounter += 2;
                        linesAway -= 2;
                    } else {
                        // Decrement the address by 1 for other instructions
                        programCounter += 1;
                        linesAway-=1;
                    }

                    // Increment the program counter
                    programCounter++;
                    fprintf(get_linesway_file,"found %d\n", linesAway);
                    //printf("pc is %d",programCounter);
                    currentFilePos2 = ftell(inputFile);
                }

                // Jump to the calculated position in the file
                fseek(inputFile, currentFilePos2, SEEK_SET);

            } else {
                    // Handle the case where operand1 is a variable name
                    if (operand1 != NULL) {
                        // Now, you can proceed with the rest of your code to handle the valid 'x=' case.

                        int temp;
                        bool found = 0;
                        int virtualLinesAway=0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand1, variables[i]) == 0) {
                                found = 1;
                                fseek(inputFile, varFilePositions[i], SEEK_SET);
                                programCounter = variablesPC[i]-1;
                                break;
                            }
                        }

                        char line[1000];
                        int linesAway = 1;
                        
                        strcat(operand1, ":");
                        if (found == 0) {
                            // Read and discard lines until we find operand1 again
                            while (fgets(line, sizeof(line), inputFile) != NULL) {
                                lineNumber++;
                                size_t len2 = strlen(line);

                                if (strstr(line, operand1) > 0) {
                                    printf("%s found %d lines away.\n\n", operand1, linesAway);
                                    fprintf(get_linesway_file,"found %d\n", linesAway);
                                    virtualLinesAway += linesAway;
                                    long currentFilePos = ftell(inputFile) - len2 - 1;
                                    long offset = -(linesAway + 1);
                                    fseek(inputFile, currentFilePos, SEEK_SET);
                                    temp = linesAway;
                                    break;
                                }
                                // Count virtual lines for 'mi' commands
                                if (strstr(line, "mi") != NULL) {
                                    virtualLinesAway++;
                                }
                                linesAway = linesAway + 1;
                            }
                        }

                        // Adjust programCounter based on virtualLinesAway
                        programCounter += virtualLinesAway;
                    } else {
                        // Handle cases where the syntax is incorrect
                        fprintf(errorFile, "Line %d: Invalid 'x' instruction: %s\n", lineNumber, assembly_instruction);
                        continue;
                    }
                }
            }
            else {
                // Handle the case where Z_flag and C_flag are not 0
                return exect = true;
            }
        }

        else if (strcmp(token, "x<") == 0) {
        
            // Check if Z_flag or C_flag is 0
            if (C_flag == 0) {
                long currentFilePos2;
            token = strtok(NULL, " ");
            char *operand1 = token;

            int address;
            
            int linesAway =1;
            int result = sscanf(operand1, "0x%X", &address);
            
                // Check if sscanf successfully read a value
                if (result == 1) {
                // Save the current position in the file         
                            
                result = 0;
                rewind(inputFile);
                lineNumber = 0;

                programCounter = 0;
                
                // Scan through lines and decrement the address accordingly
                while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL && address - 1 != programCounter) {
                    lineNumber++;
                    // printf("Address: %d PC:%d\n",address-1,programCounter);
                    size_t len = strlen(assembly_instruction);

                    if (len > 0 && assembly_instruction[len - 1] == '\n') {
                        assembly_instruction[len - 1] = '\0';
                    }

                    // Ignore empty lines
                    if (strlen(assembly_instruction) == 0) {
                        continue;
                    }

                    // Check if the instruction is 'mi' 
                    if (strstr(assembly_instruction, "mi") != NULL) {
                        programCounter += 2;
                        linesAway -= 2;
                    } else {
                        // Decrement the address by 1 for other instructions
                        programCounter += 1;
                        linesAway-=1;
                    }

                    // Increment the program counter
                    programCounter++;
                    fprintf(get_linesway_file,"found %d\n", linesAway);
                    //printf("pc is %d",programCounter);
                    currentFilePos2 = ftell(inputFile);
                }

                // Jump to the calculated position in the file
                fseek(inputFile, currentFilePos2, SEEK_SET);

            } else {
                    // Handle the case where operand1 is a variable name
                    if (operand1 != NULL) {
                        // Now, you can proceed with the rest of your code to handle the valid 'x=' case.

                        int temp;
                        bool found = 0;
                        int virtualLinesAway=0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand1, variables[i]) == 0) {
                                found = 1;
                                fseek(inputFile, varFilePositions[i], SEEK_SET);
                                programCounter = variablesPC[i]-1;
                                break;
                            }
                        }

                        char line[1000];
                        int linesAway = 1;
                        
                        strcat(operand1, ":");
                        if (found == 0) {
                            // Read and discard lines until we find operand1 again
                            while (fgets(line, sizeof(line), inputFile) != NULL) {
                                lineNumber++;
                                size_t len2 = strlen(line);

                                if (strstr(line, operand1) > 0) {
                                    printf("%s found %d lines away.\n\n", operand1, linesAway);
                                    fprintf(get_linesway_file,"found %d\n", linesAway);
                                    virtualLinesAway += linesAway;
                                    long currentFilePos = ftell(inputFile) - len2 - 1;
                                    long offset = -(linesAway + 1);
                                    fseek(inputFile, currentFilePos, SEEK_SET);
                                    temp = linesAway;
                                    break;
                                }
                                // Count virtual lines for 'mi' commands
                                if (strstr(line, "mi") != NULL) {
                                    virtualLinesAway++;
                                }
                                linesAway = linesAway + 1;
                            }
                        }

                        // Adjust programCounter based on virtualLinesAway
                        programCounter += virtualLinesAway;
                    } else {
                        // Handle cases where the syntax is incorrect
                        fprintf(errorFile, "Line %d: Invalid 'x' instruction: %s\n", lineNumber, assembly_instruction);
                        continue;
                    }
                }
            }
            else {
                // Handle the case where Z_flag and C_flag are not 0
                return exect = true;
            }
        }

        else if (strcmp(token, "x>") == 0) {

            // Check if Z_flag or C_flag is 0
            if (Z_flag == 1 && C_flag == 1) {
                long currentFilePos2;
            token = strtok(NULL, " ");
            char *operand1 = token;

            int address;
            
            int linesAway =1;
            int result = sscanf(operand1, "0x%X", &address);
            
                // Check if sscanf successfully read a value
                if (result == 1) {
                // Save the current position in the file         
                            
                result = 0;
                rewind(inputFile);
                lineNumber = 0;

                programCounter = 0;
                
                // Scan through lines and decrement the address accordingly
                while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL && address - 1 != programCounter) {
                    lineNumber++;
                    // printf("Address: %d PC:%d\n",address-1,programCounter);
                    size_t len = strlen(assembly_instruction);

                    if (len > 0 && assembly_instruction[len - 1] == '\n') {
                        assembly_instruction[len - 1] = '\0';
                    }

                    // Ignore empty lines
                    if (strlen(assembly_instruction) == 0) {
                        continue;
                    }

                    // Check if the instruction is 'mi' 
                    if (strstr(assembly_instruction, "mi") != NULL) {
                        programCounter += 2;
                        linesAway -= 2;
                    } else {
                        // Decrement the address by 1 for other instructions
                        programCounter += 1;
                        linesAway-=1;
                    }

                    // Increment the program counter
                    programCounter++;
                    fprintf(get_linesway_file,"found %d\n", linesAway);
                    //printf("pc is %d",programCounter);
                    currentFilePos2 = ftell(inputFile);
                }

                // Jump to the calculated position in the file
                fseek(inputFile, currentFilePos2, SEEK_SET);

            } else {
                    // Handle the case where operand1 is a variable name
                    if (operand1 != NULL) {
                        // Now, you can proceed with the rest of your code to handle the valid 'x=' case.

                        int temp;
                        bool found = 0;
                        int virtualLinesAway=0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand1, variables[i]) == 0) {
                                found = 1;
                                fseek(inputFile, varFilePositions[i], SEEK_SET);
                                programCounter = variablesPC[i]-1;
                                break;
                            }
                        }

                        char line[1000];
                        int linesAway = 1;
                        
                        strcat(operand1, ":");
                        if (found == 0) {
                            // Read and discard lines until we find operand1 again
                            while (fgets(line, sizeof(line), inputFile) != NULL) {
                                lineNumber++;
                                size_t len2 = strlen(line);

                                if (strstr(line, operand1) > 0) {
                                    printf("%s found %d lines away.\n\n", operand1, linesAway);
                                    fprintf(get_linesway_file,"found %d\n", linesAway);
                                    virtualLinesAway += linesAway;
                                    long currentFilePos = ftell(inputFile) - len2 - 1;
                                    long offset = -(linesAway + 1);
                                    fseek(inputFile, currentFilePos, SEEK_SET);
                                    temp = linesAway;
                                    break;
                                }
                                // Count virtual lines for 'mi' commands
                                if (strstr(line, "mi") != NULL) {
                                    virtualLinesAway++;
                                }
                                linesAway = linesAway + 1;
                            }
                        }

                        // Adjust programCounter based on virtualLinesAway
                        programCounter += virtualLinesAway;
                    } else {
                        // Handle cases where the syntax is incorrect
                        fprintf(errorFile, "Line %d: Invalid 'x' instruction: %s\n", lineNumber, assembly_instruction);
                        continue;
                    }
                }
            }
            else {
                // Handle the case where Z_flag and C_flag are not 0
                return exect = true;
            }
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
            // Convert the result to binary for reading bit 31 and bit 32
            unsigned int result = registers[register_number3];
            C_flag = (result >> 30) & 0x01;
            Z_flag = (result >> 31) & 0x01;
            } else {
                fprintf(errorFile,"Line %d: Invalid 'and' operation.\n", lineNumber);
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

            // Convert the result to binary for reading bit 31 and bit 32
            unsigned int result = registers[register_number3];
            C_flag = (result >> 30) & 0x01;
            Z_flag = (result >> 31) & 0x01;

            } else {
                fprintf(errorFile,"Line %d: Invalid 'or' operation.\n", lineNumber);
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
                registers[register_number3] = registers[register_number1] ^ registers[register_number2];
                // Convert the result to binary for reading bit 31 and bit 32
                unsigned int result = registers[register_number3];
                C_flag = (result >> 30) & 0x01;
                Z_flag = (result >> 31) & 0x01;
            } else {
                fprintf(errorFile,"Line %d: Invalid 'xor' operation.\n", lineNumber);
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
            // Convert the result to binary for reading bit 31 and bit 32
            unsigned int result = registers[register_number2];
            C_flag = (result >> 30) & 0x01;
            Z_flag = (result >> 31) & 0x01;
            } else {
                fprintf(errorFile,"Line %d: Invalid 'not' operation.\n", lineNumber);
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
            // Convert the result to binary for reading bit 31 and bit 32
            unsigned int result = registers[register_number3];
            C_flag = (result >> 30) & 0x01;
            Z_flag = (result >> 31) & 0x01;
            } else {
                fprintf(errorFile,"Line %d: Invalid 'add' operation.\n", lineNumber);
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
            registers[register_number3] = registers[register_number1] - registers[register_number2];
            // Convert the result to binary for reading bit 31 and bit 32
            unsigned int result = registers[register_number3];
            C_flag = (result >> 30) & 0x01;
            Z_flag = (result >> 31) & 0x01;
            } else {
                fprintf(errorFile,"Line %d: Invalid 'sub' operation.\n", lineNumber);
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
                // Convert the result to binary for reading bit 31 and bit 32
                unsigned int result = registers[register_number2];
                C_flag = (result >> 30) & 0x01;
                Z_flag = (result >> 31) & 0x01;

                } else {
                    fprintf(errorFile,"Line %d: Invalid operand2 for sl operation.\n", lineNumber);
                }
            } else {
                fprintf(errorFile,"Line %d: Invalid 'sl' operation.\n"), lineNumber;
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
                    // Perform logical shift right
                    unsigned int shifted_value = (unsigned int)registers[register_number1] >> shiftAmount;
                    registers[register_number2] = shifted_value;
                    // Convert the result to binary for reading bit 31 and bit 32
                    unsigned int result = registers[register_number2];
                    C_flag = (result >> 30) & 0x01;
                    Z_flag = (result >> 31) & 0x01;

                } else {
                    fprintf(errorFile,"Line %d: Invalid operand2 for sr operation.\n", lineNumber);
                }
            } else {
                fprintf(errorFile,"Line %d: Invalid 'sr' operation.\n", lineNumber);
            }

        }
        else if (strcmp(token, "nop") == 0) {
            continue;
        }

         else if (strcmp(token, "call") == 0) {
            // printf("pc to be saved is %X",programCounter);
            int memoryAddress = stackPointer - TOP_OF_STACK_ADDRESS; //stack index
            stackPointer--;
            stack[memoryAddress] = programCounter;
            
            token = strtok(NULL, " ");
            char *operand1 = token;

            int address;
            int linesAway =1;
            
            int result = sscanf(operand1, "0x%X", &address);
                // Check if sscanf successfully read a value
                if (result == 1) {
                // Save the current position in the file
                long currentFilePos = ftell(inputFile);

                // Scan through lines and decrement the address accordingly
                while (address > 0 && fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL) {
                    lineNumber++;
                    size_t len = strlen(assembly_instruction);

                    if (len > 0 && assembly_instruction[len - 1] == '\n') {
                        assembly_instruction[len - 1] = '\0';
                    }

                    // Ignore empty lines
                    if (strlen(assembly_instruction) == 0) {
                        continue;
                    }

                    // Check if the instruction is 'mi' and decrement the address by 2
                    if (strstr(assembly_instruction, "mi") != NULL) {
                        address -= 2;
                        linesAway -=2;
                    } else {
                        // Decrement the address by 1 for other instructions
                        address -= 1;
                        linesAway -= 1;

                    }

                    // Increment the program counter
                    programCounter++;
                    fprintf(get_linesway_file,"found %d\n", linesAway);
                }

                size_t len2 = strlen(line);
                currentFilePos = ftell(inputFile) - len2 - 1;
                // Jump to the calculated position in the file
                fseek(inputFile, currentFilePos, SEEK_SET);

                } else {
                    // Handle the case where operand1 is a variable name
                    if (operand1 != NULL) {
                        // Now, you can proceed with the rest of your code to handle the valid 'x=' case.

                        int temp;
                        bool found = 0;
                        int virtualLinesAway=0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand1, variables[i]) == 0) {
                                found = 1;
                                fseek(inputFile, varFilePositions[i], SEEK_SET);
                                programCounter = variablesPC[i]-1;
                                break;
                            }
                        }

                        char line[1000];
                        int linesAway = 1;
                        
                        strcat(operand1, ":");
                        if (found == 0) {
                            // Read and discard lines until we find operand1 again
                            while (fgets(line, sizeof(line), inputFile) != NULL) {
                                lineNumber++;
                                size_t len2 = strlen(line);

                                if (strstr(line, operand1) > 0) {
                                    printf("%s found %d lines away.\n\n", operand1, linesAway);
                                    fprintf(get_linesway_file,"found %d\n", linesAway);
                                    virtualLinesAway += linesAway;
                                    long currentFilePos = ftell(inputFile) - len2 - 1;
                                    long offset = -(linesAway + 1);
                                    fseek(inputFile, currentFilePos, SEEK_SET);
                                    temp = linesAway;
                                    break;
                                }
                                // Count virtual lines for 'mi' commands
                                if (strstr(line, "mi") != NULL) {
                                    virtualLinesAway++;
                                }
                                linesAway = linesAway + 1;
                            }
                        }

                        // Adjust programCounter based on virtualLinesAway
                        programCounter += virtualLinesAway;
                    } else {
                        // Handle cases where the syntax is incorrect
                        fprintf(outputFile, "Invalid 'call' instruction: %s\n", assembly_instruction);
                        continue;
                    }
                }
            
        } else if (strcmp(token, "ret") == 0) {
            if (stackPointer > 0) {
                    for (int i = PROGRAM_START_ADDRESS; i <= TOP_OF_MAIN_MEMORY; i ++) {
                        sprintf(memoryAddressStr, "%08X", i);
                        
                        // Check if the current address is within the stack range
                        if (i <= BOTTOM_OF_STACK_ADDRESS && i >= TOP_OF_STACK_ADDRESS) {
                            // Handle stack memory differently
                            int memoryAddress = i - TOP_OF_STACK_ADDRESS + 1; 
                            if (i == stackPointer){
                                programCounter = stack[memoryAddress];
                                stack[memoryAddress] = 0;
                                // printf("PC is %X",programCounter);
                            }
                        } 
                    }
                    stackPointer++;
                } else {
                    // printf("Stack underflow.\n");
                }

                rewind(inputFile);
                lineNumber = 0;

                int programCounterTemp = 0;
                
                // Scan through lines and decrement the address accordingly
                while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL && programCounterTemp != programCounter) {
                    lineNumber++;
                    //printf("Address: %d PC:%d\n",address-1,programCounter);
                    size_t len = strlen(assembly_instruction);

                    if (len > 0 && assembly_instruction[len - 1] == '\n') {
                        assembly_instruction[len - 1] = '\0';
                    }

                    // Ignore empty lines
                    if (strlen(assembly_instruction) == 0) {
                        continue;
                    }

                    // Check if the instruction is 'mi' 
                    if (strstr(assembly_instruction, "mi") != NULL) {
                        programCounterTemp += 2;
                    } else {
                        // Decrement the address by 1 for other instructions
                        programCounterTemp += 1;
                    }
                    // Increment the program counter
                    programCounterTemp++;
                }
            

        } else if (strcmp(token, "j") == 0 ){
            long currentFilePos2;
            token = strtok(NULL, " ");
            char *operand1 = token;

            int address;
            
            int linesAway =1;
            int result = sscanf(operand1, "0x%X", &address);
            
                // Check if sscanf successfully read a value
                if (result == 1) {
                // Save the current position in the file         
                            
                result = 0;
                rewind(inputFile);
                lineNumber = 0;

                programCounter = 0;
                
                // Scan through lines and decrement the address accordingly
                while (fgets(assembly_instruction, sizeof(assembly_instruction), inputFile) != NULL && address - 1 != programCounter) {
                    lineNumber++;
                    // printf("Address: %d PC:%d\n",address-1,programCounter);
                    size_t len = strlen(assembly_instruction);

                    if (len > 0 && assembly_instruction[len - 1] == '\n') {
                        assembly_instruction[len - 1] = '\0';
                    }

                    // Ignore empty lines
                    if (strlen(assembly_instruction) == 0) {
                        continue;
                    }

                    // Check if the instruction is 'mi' 
                    if (strstr(assembly_instruction, "mi") != NULL) {
                        programCounter += 2;
                        linesAway -= 2;
                    } else {
                        // Decrement the address by 1 for other instructions
                        programCounter += 1;
                        linesAway-=1;
                    }

                    // Increment the program counter
                    programCounter++;
                    fprintf(get_linesway_file,"found %d\n", linesAway);
                    //printf("pc is %d",programCounter);
                    currentFilePos2 = ftell(inputFile);
                }

                // Jump to the calculated position in the file
                fseek(inputFile, currentFilePos2, SEEK_SET);

                } else {
                    // Handle the case where operand1 is a variable name
                    if (operand1 != NULL) {
                        // Now, you can proceed with the rest of your code to handle the valid 'x=' case.

                        int temp;
                        bool found = 0;
                        int virtualLinesAway=0;

                        for (int i = 0; i < varCount; i++) {
                            if (strcmp(operand1, variables[i]) == 0) {
                                found = 1;
                                fseek(inputFile, varFilePositions[i], SEEK_SET);
                                programCounter = variablesPC[i]-1;
                                break;
                            }
                        }

                        char line[1000];
                        int linesAway = 1;
                        
                        strcat(operand1, ":");
                        if (found == 0) {
                            // Read and discard lines until we find operand1 again
                            while (fgets(line, sizeof(line), inputFile) != NULL) {
                                lineNumber++;
                                size_t len2 = strlen(line);

                                if (strstr(line, operand1) > 0) {
                                    printf("%s found %d lines away.\n\n", operand1, linesAway);
                                    fprintf(get_linesway_file,"found %d\n", linesAway);
                                    virtualLinesAway += linesAway;
                                    long currentFilePos = ftell(inputFile) - len2 - 1;
                                    long offset = -(linesAway + 1);
                                    fseek(inputFile, currentFilePos, SEEK_SET);
                                    temp = linesAway;
                                    break;
                                }
                                // Count virtual lines for 'mi' commands
                                if (strstr(line, "mi") != NULL) {
                                    virtualLinesAway++;
                                }
                                linesAway = linesAway + 1;
                            }
                        }

                        // Adjust programCounter based on virtualLinesAway
                        programCounter += virtualLinesAway;
                    } else {
                        // Handle cases where the syntax is incorrect
                        fprintf(outputFile, "Invalid 'j' instruction: %s\n", assembly_instruction);
                        continue;
                    }
                }
        } else if (strcmp(token, "push") == 0) {
            // Process the 'push' token
            strcpy(machine_code, "18"); // Opcode: 18
            token = strtok(NULL, " , ");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
            
            // Check if the stack is not full
                if (stackPointer >= TOP_OF_STACK_ADDRESS && stackPointer <= BOTTOM_OF_STACK_ADDRESS){

                    // Push the value onto the stack
                    int memoryAddress = stackPointer - TOP_OF_STACK_ADDRESS; //stack index
                    stackPointer--;
                    // content[memoryAddress] = registers[register_number1];
                    stack[memoryAddress] = registers[register_number1];
                } 
                else {
                        // printf("Stack overflow.\n");
                }
            } else {
                fprintf(errorFile,"Line %d: Invalid 'push' operation.\n", lineNumber);
            }
        }   
        
        else if (strcmp(token, "pop") == 0) {
            // Process the 'pop' token
            token = strtok(NULL, " , ");
            char* operand1 = token;

            int register_number1;

            if (sscanf(operand1, "r%X", &register_number1) == 1) {
            // Check if the stack is not empty
                if (stackPointer > 0) {
                    for (int i = PROGRAM_START_ADDRESS; i <= TOP_OF_MAIN_MEMORY; i ++) {
                        sprintf(memoryAddressStr, "%08X", i);
                        
                        // Check if the current address is within the stack range
                        if (i <= BOTTOM_OF_STACK_ADDRESS && i >= TOP_OF_STACK_ADDRESS) {
                            // Handle stack memory differently
                            int memoryAddress = i - TOP_OF_STACK_ADDRESS + 1; 
                            if (i == stackPointer){
                                registers[register_number1] = stack[memoryAddress];
                                stack[memoryAddress] = 0;
                            }
                        } 
                    }
                    stackPointer++;
                } else {
                    // printf("Stack underflow.\n");
                }
            } else {
                fprintf(errorFile,"Line %d: Invalid 'pop' operation.\n", lineNumber);
            }
            
        }
        else if (strcmp(token, "reset-1") == 0) {
            reset_1_command = true;
            continue; // Skip processing this line further
        } else if (strcmp(token, "reset-2") == 0) {
            reset_2_command = true;
            continue; // Skip processing this line further
        } else {
            fprintf(errorFile,"Line %d: Invalid instruction.\n", lineNumber);
        }

        if (reset_1_command) {
        // Reset the program counter to 0
            programCounter = PROGRAM_START_ADDRESS;

        } else if (reset_2_command) {
            // Handle reset-2 command (leave it empty for now)
            continue;
        }    
        reset_1_command = false;
        reset_2_command = false;    
        
        counter += increment;  // Increment the counter
        
        currentPointer = programCounter;
        programCounter += increment;
        increment=1; //reset back increment to 1
        
        
        // printf("CurrentPtr: 0x%08X    NxPtr: %08X\n\n",currentPointer, programCounter);
        
    } 
    fprintf(outputFile,"==============================================================================================================\n");
    fprintf(outputFile,"                        Instruction Memory");
    fprintf(outputFile,"\n==============================================================================================================\n");


    for (int i = 0; i < 256; i++) {
                
        sprintf(insAddressStr, "0x%08X", i);  // Format the address

        if (INScontent[i][0] != '\0') {
            // Print the instruction memory entry if it is not empty
            fprintf(outputFile, "%10s\t\t0x%-10s\n", insAddressStr, INScontent[i]);

        } else {
            // Print the address with a placeholder if it is empty
            fprintf(outputFile, "%10s\t\t%10s\n", insAddressStr, "0x00000000");
        }

    }

    //fprintf(outputFile, "\nN flag: %d", N_flag);
         
    fclose(outputFile);

    FILE *registersFile = fopen("registers_output.txt", "w");
    FILE *memoryFile = fopen("memory_output.txt", "w");
    FILE *ioFile = fopen("io_output.txt", "w");

    if (registersFile == NULL || memoryFile == NULL || ioFile == NULL) {
        perror("Error opening output files");

        fclose(inputFile);
        fclose(outputFile);

        // Close the files if they were opened
        if (registersFile != NULL) fclose(registersFile);
        if (memoryFile != NULL) fclose(memoryFile);
        if (ioFile != NULL) fclose(ioFile);

        return 1;
    }
   
    
    // Iterate through the registers and print to the registers file
    for (int i = 0; i < 256; i++) {
        fprintf(registersFile, "r%X\t\t0x%08X\n", i, registers[i]);
    }
    fprintf(registersFile, "SP\t 0x%08X\n", stackPointer);  
    fprintf(registersFile, "ZFlag\t %d\n", Z_flag);
    fprintf(registersFile, "CFlag\t %d\n", C_flag);
    fprintf(registersFile, "PC\t0x%08X\n",programCounter);

    // Iterate through the memory addresses and print to the memory file
    // fprintf(memoryFile, "%-20s%-10s\n", "Memory Address", "Content");
    for (int i = PROGRAM_START_ADDRESS; i <= TOP_OF_MAIN_MEMORY; i ++) {
        sprintf(memoryAddressStr, "%08X", i);
        
        // Check if the current address is within the stack range
        if (i <= BOTTOM_OF_STACK_ADDRESS && i >= TOP_OF_STACK_ADDRESS) {
            // Handle stack memory differently
            int memoryAddress = i - TOP_OF_STACK_ADDRESS + 1; 
            fprintf(memoryFile, "%10s\t\t\t0x%08X (Stack)\n", memoryAddressStr, stack[memoryAddress]);
        } else {
            // Handle non-stack memory
            fprintf(memoryFile, "%10s\t\t\t0x%08X\n", memoryAddressStr, content[i]);
        }
    }

    // Iterate through the IO addresses and print to the io file
    // fprintf(ioFile, "%-13s%15s\n", "IO Port", "Content");
    for (int i = 0x00000000; i <= 0x00001FFF; i++) {
        sprintf(ioPortStr, "0x%08X", i);
        fprintf(ioFile, "%10s\t\t\t0x%08X\n", ioPortStr, IOcontent[i]);
    }

    // Close the files
    fclose(registersFile);
    fclose(memoryFile);
    fclose(ioFile);
    fclose(errorFile);
    
}

int main() {
    
    char INScontent[NUM_INSTRUCTIONS][MAX_INSTRUCTION_SIZE];

    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        for (int j = 0; j < MAX_INSTRUCTION_SIZE; j++) {
            INScontent[i][j] = '\0';  // Set each character to null character
        }
    }
    
    // Initialize the stack memory to a default value (e.g., 0)
    for (int i = 0; i < STACK_SIZE; ++i) {
        stack[i] = 0;
    }

    for (int i = 0; i < IO_SIZE; i++) {
        IOcontent[i] = 0x00000000;
    }
    for (int i = 0; i < MEMORY_SIZE; i++) {
        content[i] = 0x00000000;
    }


    // Specify the input and output file names
    // const char *inputFileName = "example.txt";
    const char *inputFileName = "temp_input2.txt";
    const char *outputFileName = "output.txt";  // Declare outputFileName here
    const char *get_lines_away_filename = "getLines.txt";

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

    FILE *get_lines_away_file = NULL;
    get_lines_away_file = fopen(get_lines_away_filename, "w");
    if (get_lines_away_file == NULL) {
        perror("Error opening output file");

        fclose(inputFile);
        return 1;
    }

    // Call the conversion and printing function
    convert_and_print(inputFile, outputFile, outputFileName, get_lines_away_file);

    // Close the files
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
