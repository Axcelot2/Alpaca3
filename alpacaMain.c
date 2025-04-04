// Xi Ting Yan and Ali Kutay Dastan wrote this program.
/* Ali Kutay Dastan wrote all functions in interaction.c and interaction.h files included in this submission. Currently, due to difficulties with implementation
these files and their functions are INTENTIONALLY EXCLUDED from compiling until further testing can be done. */
// Xi Ting Yan wrote all functions in alpacaLocal.c except for the printFileContent() function.
// Tahmeed Ahmed is responsible for debugging and error handling.
// Ashkan Sharifi implemented the function that permits the printing of text from a selected context file. (printFileContent())
// This program takes user input to create directories with the specified titles.
// Compile alpacaMain.c with alpacaLocal.c in the same directory as alpacaLocal.h and dirStruct.h.
// The final goal of project Alpaca is to implement data streaming from Ollama into this program to create local files that store Ollama context.
// Ollama is an open-source software that allows local running of LLMs.

// DO NOT COMPILE WITH interaction.c, the interaction.c and interaction.h files. These files are simply submitted as a WIP

#include <stdio.h>
#include <stdlib.h>
// Header file alpacaLocal.h contains declaration of all functions that interact with local directories/files.
#include "alpacaLocal.h"
// Header file to include functionalities that communicate with Ollama                                                                                              
#include "alpacaInteraction.h"


// The main program uses functions defined in alpacaLocal.c.
int main() {
    // whatDo stores user request for function.
    // dirArr is an array of structs, with each struct representing a directory.
    int whatDo;
    struct dirStruct dirArr[MAX_DIR];

    // Functions called later will check if dirTitle[0] is NULL as a means of verifying if it has been initialized.
    for (int i = 0; i < MAX_DIR; i++) {
        dirArr[i].dirTitle[0] = '\0';
    }

    // The above comment applies to files too.
    for (int i = 0; i < MAX_DIR; i++) {
        for (int x = 0; x < MAX_FILES; x++) {
            dirArr[i].fileTitle[x][0] = '\0';
        }
    }

    // This function indexes existing directories in the same directory as the program. See alpacaLocal.c.
    // If this function fails, an error message is printed and the program terminates. (See alpacaLocal.c for error message.)
    if (indexDir(dirArr) == 1) {
        return 1;
    }

    if (indexFile(dirArr) == 1) {
        return 1;
    }

    printf("\n--- Alpaca: Ollama Assistant ---\n\n");

    do {
        // Menu options displyed.
        // Currently option 6 is temporarily disabled until it can be properly implemented, the option is simply here for reference purposes.
        printf("Menu Options: \n");
        printf("1. List Available Models\n");
        printf("2. Create Context Category\n");
        printf("3. List Categories\n");
        printf("4. List Context Files\n");
        printf("5. Print Context File\n");
        printf("6. Chat with Model and Save to File\n");
        printf("7. Upload a Text File to Ollama\n");
        printf("0. Exit Alpaca\n\n");
        printf("Enter your selection: ");
        scanf("%d", &whatDo);
        getchar(); // Buffer overflow protection for fgets() in called functions.
        printf("\n");

        switch (whatDo) {
            case 1:
                // This function creates a directory, see alpacaLocal.c for definition and error messages.
                if (system("ollama list") != 0) {
                    printf("No models found.\n\n");
                } else {
                    printf("End of list of models.\n\n");
                }
                printf("Press any button to continue...\n");
                getchar();
                break;

            case 2:
                // This function creates a file, see alpacaLocal.c for definition and error messages.
                createDir(dirArr);
                printf("Press any button to continue...\n");
                getchar();
                break;

            case 3:
                // This function prints all indexed directories, see alpacaLocal.c for definition.
                // Function return 0 after successful execution. Returns 1 if no indexed directories exist.
                listDir(dirArr);
                printf("Press any button to continue...\n");
                getchar();
                break;

            // This allows a user to specify a directory, and a file within said directory to return the title of.
            case 4:
                int dirSelect = 0;
                if (listDir(dirArr) == 0) {
                    printf("Select a directory: ");
                    scanf("%d", &dirSelect);
                    dirSelect--;
                    if (dirSelect < 0 || dirSelect >= MAX_DIR || dirArr[dirSelect].dirTitle[0] == '\0') {
                        printf("Invalid Category Entered.\n\n");
                    } else {
                        listFile(dirArr, dirSelect);
                    }
                }
                printf("Press any button to continue...\n");
                getchar();
                break;

            // This function prints the contents of a user specified context file.
            case 5:
                printFileContent(dirArr);
                printf("Press any button to continue...\n");
                getchar();
                break;

            // This function will prompt the user on which model they want to chat with and also ask which .txt file the user wants to save the conversation
            case 6:
                chatWithModelToFile(dirArr);  
                printf("Press any button to continue...\n");
                getchar();
                break;
            // This function will upload an existing .txt file into a desired model 
            case 7:
                uploadFileToOllama(dirArr);  
                printf("Press any button to continue...\n");
                getchar();
                break;
                

            case 0:
                printf("Exiting Program.\n\n");
                break;
            
            default:
                // Input validation.
                printf("Please enter a valid input according to the menu options.\n\n");
                printf("Press any button to continue...\n");
                getchar();
                break;
        }

    // Program terminates only at user request.
    } while (whatDo != 0);

    return 0;
}