// alpacaInteraction.c - with model list using popen()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alpacaLocal.h"
#include "alpacaInteraction.h"

#define MAX_CMD 1024

void chatWithModelToFile(struct dirStruct dirArr[MAX_DIR]) {
    char modelName[64];
    char userPrompt[MAX_CONTENT_CHAR];
    char command[MAX_CMD];
    char buffer[512];
    char fullResponse[MAX_RESPONSE] = {0};
    char filePath[2 * MAX_TITLE_CHAR + 2];

    printf("\n--- List of Available Models ---\n");
    FILE *modelList = popen("ollama list", "r");
    if (modelList) {
        char line[256];
        while (fgets(line, sizeof(line), modelList)) {
            printf("%s", line);
        }
        pclose(modelList);
    } else {
        printf("(Failed to list models — is Ollama running?)\n");
    }
    printf("\nEnter the exact name of the mode to use: ");
    fgets(modelName, sizeof(modelName), stdin);
    modelName[strcspn(modelName, "\n")] = 0;

    if (listDir(dirArr) != 0) {
        printf("No directories exist. Please create one first.\n");
        return;
    }
    int dirIndex;
    printf("Select a directory to save the chat file: ");
    scanf("%d", &dirIndex);
    getchar();
    dirIndex--;
    if (dirIndex < 0 || dirIndex >= MAX_DIR || dirArr[dirIndex].dirTitle[0] == '\0') {
        printf("Invalid directory selection.\n");
        return;
    }

    int fileIndex;
    for (fileIndex = 0; fileIndex < MAX_FILES; fileIndex++) {
        if (dirArr[dirIndex].fileTitle[fileIndex][0] == '\0') {
            break;
        }
    }

    printf("Enter the name of the file to save this conversation: ");
    fgets(dirArr[dirIndex].fileTitle[fileIndex], sizeof(dirArr[dirIndex].fileTitle[fileIndex]), stdin);
    dirArr[dirIndex].fileTitle[fileIndex][strcspn(dirArr[dirIndex].fileTitle[fileIndex], "\n")] = 0;

    snprintf(filePath, sizeof(filePath), "%s/%s", dirArr[dirIndex].dirTitle, dirArr[dirIndex].fileTitle[fileIndex]);

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
    fullResponse[MAX_RESPONSE - 1] = '\0';
    int len = strlen(fullResponse);
    while (len > 0 && (fullResponse[len - 1] == ' ' || fullResponse[len - 1] == '\n' || fullResponse[len - 1] == '\r')) {
        fullResponse[--len] = '\0';
    }
    pclose(fp);

    FILE *history = fopen(filePath, "w");
    if (!history) {
        perror("Failed to open chat file");
        return;
    }
    fprintf(history, "Model: %s\n", modelName);
    fprintf(history, "User: %s\n", userPrompt);
    fprintf(history, "Bot:  %s\n", fullResponse);
    fprintf(history, "------------------------\n");
    fclose(history);

    printf("Conversation saved to %s.\n", filePath);
}

void uploadFileToOllama(struct dirStruct dirArr[MAX_DIR]) {
    char modelName[64];
    char filePath[2 * MAX_TITLE_CHAR + 2];
    char fileContent[MAX_RESPONSE] = {0};
    char buffer[512];
    char command[MAX_CMD];

    printf("\n--- List of Available Models ---\n");
    FILE *modelList = popen("ollama list", "r");
    if (modelList) {
        char line[256];
        while (fgets(line, sizeof(line), modelList)) {
            printf("%s", line);
        }
        pclose(modelList);
    } else {
        printf("(Failed to list models — is Ollama running?)\n");
    }
    printf("Enter the name of the model to use (e.g., mistral, llama3): ");
    fgets(modelName, sizeof(modelName), stdin);
    modelName[strcspn(modelName, "\n")] = 0;

    if (listDir(dirArr) != 0) {
        printf("No directories exist. Please create one first.\n");
        return;
    }
    int dirIndex;
    printf("Select a directory to choose the .txt file from: ");
    scanf("%d", &dirIndex);
    getchar();
    dirIndex--;
    if (dirIndex < 0 || dirIndex >= MAX_DIR || dirArr[dirIndex].dirTitle[0] == '\0') {
        printf("Invalid directory selection.\n");
        return;
    }

    if (listFile(dirArr, dirIndex) != 0) {
        printf("No files exist in this directory.\n");
        return;
    }
    int fileIndex;
    printf("Select a file to upload: ");
    scanf("%d", &fileIndex);
    getchar();
    fileIndex--;
    if (fileIndex < 0 || fileIndex >= MAX_FILES || dirArr[dirIndex].fileTitle[fileIndex][0] == '\0') {
        printf("Invalid file selection.\n");
        return;
    }

    snprintf(filePath, sizeof(filePath), "%s/%s", dirArr[dirIndex].dirTitle, dirArr[dirIndex].fileTitle[fileIndex]);

    FILE *f = fopen(filePath, "r");
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
    printf("\nUsing file: %s\n\n", filePath);

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
