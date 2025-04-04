// alpacaInteraction.c - Modular Ollama interaction logic for Alpaca
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alpacaLocal.h"

#define MAX_CMD 1024

// This function allows a user to choose a model, chat, and append the prompt/response to a specified text file.
void chatWithModelToFile() {
    char modelName[64];
    char historyFile[256];
    char userPrompt[MAX_CONTENT_CHAR];
    char command[MAX_CMD];
    char buffer[512];
    char fullResponse[MAX_RESPONSE] = {0};

    printf("Enter the name of the model to use (e.g., mistral, llama3): ");
    fgets(modelName, sizeof(modelName), stdin);
    modelName[strcspn(modelName, "\n")] = 0;

    printf("Enter the name of the .txt file to use as chat history: ");
    fgets(historyFile, sizeof(historyFile), stdin);
    historyFile[strcspn(historyFile, "\n")] = 0;

    printf("Enter your prompt for %s:\n", modelName);
    fgets(userPrompt, sizeof(userPrompt), stdin);
    userPrompt[strcspn(userPrompt, "\n")] = 0;

    snprintf(command, sizeof(command), "ollama run %s \"%s\"", modelName, userPrompt);

    FILE *fp = popen(command, "r");
    if (!fp) {
        perror("popen failed");
        return;
    }

    printf("\nResponse:\n");
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
        strncat(fullResponse, buffer, sizeof(fullResponse) - strlen(fullResponse) - 1);
    }
    pclose(fp);

    FILE *history = fopen(historyFile, "a");
    if (!history) {
        perror("Failed to open history file");
        return;
    }
    fprintf(history, "Model: %s\n", modelName);
    fprintf(history, "User: %s\n", userPrompt);
    fprintf(history, "Bot:  %s\n", fullResponse);
    fprintf(history, "------------------------\n");
    fclose(history);
}

// This function uploads a text file to Ollama using a selected model.
void uploadFileToOllama() {
    char modelName[64];
    char filename[256];
    char fileContent[MAX_RESPONSE] = {0};
    char buffer[512];
    char command[MAX_CMD];

    printf("Enter the name of the model to use (e.g., mistral, llama3): ");
    fgets(modelName, sizeof(modelName), stdin);
    modelName[strcspn(modelName, "\n")] = 0;

    printf("Enter the name of the .txt file to upload to Ollama: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Failed to open file");
        return;
    }

    while (fgets(buffer, sizeof(buffer), f)) {
        strncat(fileContent, buffer, sizeof(fileContent) - strlen(fileContent) - 1);
    }
    fclose(f);

    for (int i = 0; fileContent[i]; ++i) {
        if (fileContent[i] == '\n') {
            fileContent[i] = ' ';
        }
    }

    snprintf(command, sizeof(command), "ollama run %s \"%s\"", modelName, fileContent);
    FILE *fp = popen(command, "r");
    if (!fp) {
        perror("popen failed");
        return;
    }

    printf("\nOllama response:\n");
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    pclose(fp);
}
