#include <stdio.h>
#include <stdlib.h>

void display_ascii_art(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open ASCII art file.\n");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    fclose(file);
}

int main() {
    // Display ASCII art from file
    display_ascii_art("ascii_art.txt");

    // Print welcome message
    printf("\nWelcome to the Expense Tracker App!\n");
    printf("\nManage your expenses effectively by tracking them month by month.\n");
    printf("=========================================\n\n");

    // Ask user type
    printf("Are you a:\n");
    printf("1. Student\n");
    printf("2. Working Professional\n");
    printf("Enter your choice (1 or 2): ");
    
    int choice;
    scanf("%d", &choice);

    // Execute corresponding program
    if (choice == 1) {
        printf("\nLaunching Student Expense Tracker...\n");
        system("gcc Expense_Tracker.c -o Expense_Tracker && ./Expense_Tracker");
    } else if (choice == 2) {
        printf("\nLaunching Professional Expense Tracker...\n");
        system("gcc Professional.c -o Professional && ./Professional");
    } else {
        printf("\nInvalid choice. Exiting the program.\n");
    }

    return 0;
}