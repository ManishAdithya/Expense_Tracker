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
    display_ascii_art("ascii_art.txt");

    printf("\nWelcome to the Expense Tracker App!\n");
    printf("\nManage your expenses effectively by tracking them month by month.\n");
    printf("=========================================\n\n");

    printf("Are you a:\n");
    printf("1. Student\n");
    printf("2. Working Professional\n");
    printf("Enter your choice (1 or 2): ");
    
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        printf("\nLaunching Student Expense Tracker...\n");
        system("gcc Student_Expense_Tracker.c -o Expense_Tracker && ./Expense_Tracker");
    } else if (choice == 2) {
        printf("\nLaunching Professional Expense Tracker...\n");
        system("gcc Profession_Expense_Tracker.c -o Profession && ./Profession");
    } else {
        printf("\nInvalid choice. Exiting the program.\n");
    }

    return 0;
}
