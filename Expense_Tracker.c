#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EXPENSES 10000
#define MAX_DESC_LEN 50
#define MAX_ITEMS 100
#define DATA_FILE_FORMAT "expenses_%s_%d.dat"
#define BUDGET_FILE "budget.dat"

typedef struct {
    int id;
    char description[MAX_DESC_LEN];
    float amount;
} Expense;

typedef struct {
    char month[20];
    int year;
    float budget;
    float spent;
} Budget;

// Global variables
Budget currentBudget;

// Function prototypes
void addExpense();
void viewExpenses();
void deleteExpense();
void viewEarlierMonthExpenses();
void saveExpensesToFile(Expense expenses[], int count, const char *filename);
int loadExpensesFromFile(Expense expenses[], const char *filename);
void initializeBudget();
void resetBudget();
void saveBudget();
void loadBudget();
void updateSpentAmount();
void getExpenseFileName(char *filename, const char *month, int year);

// Main function
int main() {
    int choice;

    loadBudget();
    initializeBudget();

    do {
        printf("\n========= Expense Tracker =========\n");
        printf("1. Add Expense\n");
        printf("2. View Current Month Expenses\n");
        printf("3. View Earlier Month Expenses\n");
        printf("4. Delete Expense\n");
        printf("5. Reset Month and Budget\n");
        printf("6. Exit\n");
        printf("===================================\n");
        printf("\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addExpense();
                break;
            case 2:
                viewExpenses();
                break;
            case 3:
                viewEarlierMonthExpenses();
                break;
            case 4:
                deleteExpense();
                break;
            case 5:
                resetBudget();
                break;
            case 6:
                printf("I hope this was user-friendly ;) . Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}


// Get the expense file name for a specific month and year
void getExpenseFileName(char *filename, const char *month, int year) {
    sprintf(filename, DATA_FILE_FORMAT, month, year);
}

// Initialize budget if it's a new month
void initializeBudget() {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    char currentMonth[20];
    int currentYear = local->tm_year + 1900;
    strftime(currentMonth, sizeof(currentMonth), "%B", local);

    if (strcmp(currentBudget.month, currentMonth) != 0 || currentBudget.year != currentYear) {
        printf("\n");
        printf("It seems the month has changed. Please reset the budget.\n");
        resetBudget();
    } else {
        printf("\n");
        printf("Loaded budget for %s %d. Remaining: ₹%.2f\n", currentBudget.month, currentBudget.year, currentBudget.budget - currentBudget.spent);
    }

    updateSpentAmount();
}

// Reset the month, year, and budget
void resetBudget() {
    printf("\n");
    printf("Enter the month: ");
    scanf("%s", currentBudget.month);

    printf("Enter the year: ");
    scanf("%d", &currentBudget.year);

    printf("Enter your budget for %s %d: ", currentBudget.month, currentBudget.year);
    scanf("%f", &currentBudget.budget);

    currentBudget.spent = 0.0; // Reset spent amount
    saveBudget();
    printf("\n");
    printf("Budget set successfully for %s %d.\n", currentBudget.month, currentBudget.year);
}

// Save budget to file
void saveBudget() {
    FILE *file = fopen(BUDGET_FILE, "wb");
    if (file == NULL) {
        perror("Error saving budget");
        return;
    }
    fwrite(&currentBudget, sizeof(Budget), 1, file);
    fclose(file);
}

// Load budget from file
void loadBudget() {
    FILE *file = fopen(BUDGET_FILE, "rb");
    if (file == NULL) {
        printf("\n");
        printf("No previous budget found. Please set up a new budget.\n");
        return;
    }
    fread(&currentBudget, sizeof(Budget), 1, file);
    fclose(file);
}

// Add an expense
void addExpense() {
    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count >= MAX_EXPENSES) {
        printf("Error: Expense limit reached.\n");
        return;
    }

    Expense newExpense;
    newExpense.id = (count == 0) ? 1 : expenses[count - 1].id + 1;
    printf("\n");
    printf("Enter description: ");
    getchar(); // Clear the newline character
    fgets(newExpense.description, MAX_DESC_LEN, stdin);
    newExpense.description[strcspn(newExpense.description, "\n")] = '\0'; // Remove trailing newline

    printf("Enter amount (in ₹): ");
    scanf("%f", &newExpense.amount);

    currentBudget.spent += newExpense.amount;
    if (currentBudget.spent > currentBudget.budget) {
        printf("\n");
        printf("WARNING: YOU HAVE EXCEEDED YOUR BUDGET FOR %s %d!\n", currentBudget.month, currentBudget.year);
    } else {
        printf("\n");
        printf("You can still spend ₹%.2f this month.\n", currentBudget.budget - currentBudget.spent);
    }

    expenses[count++] = newExpense;
    saveExpensesToFile(expenses, count, filename);
    saveBudget();

    printf("Expense added successfully.\n");
}

// Save expenses to file
void saveExpensesToFile(Expense expenses[], int count, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error saving expenses");
        return;
    }
    fwrite(expenses, sizeof(Expense), count, file);
    fclose(file);
}

// Load expenses from file
int loadExpensesFromFile(Expense expenses[], const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return 0; // File doesn't exist, no expenses
    }

    int count = 0;
    while (fread(&expenses[count], sizeof(Expense), 1, file)) {
        count++;
    }
    fclose(file);
    return count;
}

// Update spent amount based on saved expenses
void updateSpentAmount() {
    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);
    currentBudget.spent = 0.0;

    for (int i = 0; i < count; i++) {
        currentBudget.spent += expenses[i].amount;
    }
}

void viewExpenses() {
    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\n");
        printf("\nNo expenses recorded for %s %d.\n", currentBudget.month, currentBudget.year);
        return;
    }
    printf("\n");
    printf("\nExpenses for %s %d:\n", currentBudget.month, currentBudget.year);
    printf("ID\tDescription\tAmount (₹)\n");
    printf("====================================\n");

    for (int i = 0; i < count; i++) {
        printf("%d\t%s\t₹%.2f\n", expenses[i].id, expenses[i].description, expenses[i].amount);
    }

    printf("====================================\n");
    printf("Total Spent: ₹%.2f\n", currentBudget.spent);
}


void viewEarlierMonthExpenses() {
    char month[20];
    int year;
    printf("\n");
    printf("\nEnter the month: ");
    scanf("%s", month);
    printf("Enter the year: ");
    scanf("%d", &year);

    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, month, year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\n");
        printf("\nNo expenses recorded for %s %d.\n", month, year);
        return;
    }

    printf("\nExpenses for %s %d:\n", month, year);
    printf("ID\tDescription\tAmount (₹)\n");
    printf("====================================\n");

    for (int i = 0; i < count; i++) {
        printf("%d\t%s\t₹%.2f\n", expenses[i].id, expenses[i].description, expenses[i].amount);
    }

    printf("====================================\n");
    float totalSpent = 0.0;
    for (int i = 0; i < count; i++) {
        totalSpent += expenses[i].amount;
    }
    printf("Total Spent: ₹%.2f\n", totalSpent);
}

void deleteExpense() {
    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\n");
        printf("\nNo expenses recorded for %s %d to delete.\n", currentBudget.month, currentBudget.year);
        return;
    }

    int idToDelete;
    printf("\n");
    printf("\nEnter the ID of the expense to delete: ");
    scanf("%d", &idToDelete);

    int index = -1;
    for (int i = 0; i < count; i++) {
        if (expenses[i].id == idToDelete) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("\n");
        printf("Expense with ID %d not found.\n", idToDelete);
        return;
    }

    currentBudget.spent -= expenses[index].amount;

    for (int i = index; i < count - 1; i++) {
        expenses[i] = expenses[i + 1];
    }
    count--;

    saveExpensesToFile(expenses, count, filename);
    saveBudget();
    printf("\n");
    printf("Expense with ID %d deleted successfully.\n", idToDelete);
}



