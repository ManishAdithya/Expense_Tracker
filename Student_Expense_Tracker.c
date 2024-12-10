#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EXPENSES 10000
#define MAX_DESC_LEN 50
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

Budget currentBudget;

void addExpense();
void viewExpenses();
void viewEarlierMonthExpenses();
void deleteExpense();
void resetBudget();
void getExpenseForCategory();
void viewBalance();
void generateBill();

void initializeBudget();
void saveExpensesToFile(Expense expenses[], int count, const char *filename);
int  loadExpensesFromFile(Expense expenses[], const char *filename);
void saveBudget();
void updateSpentAmount();
void getExpenseFileName(char *filename, const char *month, int year);

void display_ascii_art(const char *filename);
void printMenu();
void printHeader(const char *title);
void printDivider();


int main() {
    int choice;

    display_ascii_art("ascii_art.txt");
    printf("\nWelcome to the Expense Tracker App!\n");
    printf("\nManage your expenses effectively by tracking them month by month.\n");
    printf("=========================================\n\n");

    initializeBudget();

    do {
        printMenu();
        printf("\033[1;33mEnter your choice: \033[0m");
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
                getExpenseForCategory();
                break;
            case 7:
                viewBalance();
                break;
            case 8:
                generateBill();
                break;
            case 9:
                printHeader("Goodbye!");
                printf("\033[1;32mI hope this was user-friendly ;) . See you next time!\033[0m\n");
                break;
            default:
                printf("\033[1;31mInvalid choice. Please try again.\033[0m\n");
        }
    } while (choice != 9);

    return 0;
}


////// PRINT FUNCTIONS ///////

void display_ascii_art(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open ASCII art file.\n");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);  //printf can be used ?
    }
    fclose(file);
}

void printMenu() {
    printHeader("EXPENSE TRACKER");
    printf("\033[1;36m1. Add Expense\n");
    printf("2. View Current Month Expenses\n");
    printf("3. View Earlier Month Expenses\n");
    printf("4. Delete Expense\n");
    printf("5. Reset Month and Budget\n");
    printf("6. Total Expense for a Category\n");
    printf("7. View balance\n");
    printf("8. Generate Bill \033[0m\n");
    printf("9. Exit\n");
    printDivider();
}

void printHeader(const char *title) {
    printf("\n");
    printf("╭────────────────────────────────────────╮\n");
    printf("│ \033[1;34m%-38s\033[0m │\n", title);
    printf("╰────────────────────────────────────────╯\n");
}

void printDivider() {
    printf("──────────────────────────────────────────\n");
}
//////////////////////////////////////




//////// BACKGROUND FUNCTIONS ///////////

void getExpenseFileName(char *filename, const char *month, int year) {
    sprintf(filename, DATA_FILE_FORMAT, month, year);
}

void initializeBudget() {
    FILE *file = fopen(BUDGET_FILE, "rb");

    if (file != NULL) {
        fread(&currentBudget, sizeof(Budget), 1, file);
        fclose(file);
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    char currentMonth[20];
    int currentYear = local->tm_year + 1900;
    strftime(currentMonth, sizeof(currentMonth), "%B", local);

    if (file == NULL || strcmp(currentBudget.month, currentMonth) != 0 || currentBudget.year != currentYear) {
        printf("\033[1;33mNo budget set or outdated budget found. Please set the budget for %s %d.\033[0m\n",
               currentMonth, currentYear);
        resetBudget();
    } else {
        printf("\033[1;32mLoaded budget for %s %d. Remaining: ₹%.2f\033[0m\n",
               currentBudget.month, currentBudget.year, currentBudget.budget - currentBudget.spent);
        updateSpentAmount();
    }
}

void saveBudget() {
    FILE *file = fopen(BUDGET_FILE, "wb");
    if (file == NULL) {
        perror("Error saving budget");
        return;
    }
    fwrite(&currentBudget, sizeof(Budget), 1, file);
    fclose(file);
}

void saveExpensesToFile(Expense expenses[], int count, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error saving expenses");
        return;
    }
    fwrite(expenses, sizeof(Expense), count, file);
    fclose(file);
}

int loadExpensesFromFile(Expense expenses[], const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }

    int count = 0;
    while (fread(&expenses[count], sizeof(Expense), 1, file)) {
        count++;
    }
    fclose(file);
    return count;
}

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
///////////////////////////////////////////////



///////// FRONTEND FUNCTIONS  /////////////////

void addExpense() {
    Expense expenses[MAX_EXPENSES];
    char filename[MAX_DESC_LEN];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count >= MAX_EXPENSES) {
        printf("\033[1;31mError: Expense limit reached.\033[0m\n");
        return;
    }

    Expense newExpense;
    newExpense.id = (count == 0) ? 1 : expenses[count - 1].id + 1;
    printf("\033[1;36mEnter description: \033[0m");
    getchar(); 
    fgets(newExpense.description, MAX_DESC_LEN, stdin);
    newExpense.description[strcspn(newExpense.description, "\n")] = '\0';

    printf("\033[1;36mEnter amount (in ₹): \033[0m");
    scanf("%f", &newExpense.amount);

    currentBudget.spent += newExpense.amount;
    if (currentBudget.spent > currentBudget.budget) {
        printf("\033[1;31mWARNING: YOU HAVE EXCEEDED YOUR BUDGET!\033[0m\n");
    } else {
        printf("\033[1;32mYou can still spend ₹%.2f this month.\033[0m\n", currentBudget.budget - currentBudget.spent);
    }

    expenses[count++] = newExpense;
    saveExpensesToFile(expenses, count, filename);
    saveBudget();

    printf("\033[1;32mExpense added successfully.\033[0m\n");
}

void viewExpenses() {
    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\033[1;31mNo expenses recorded for %s %d.\033[0m\n", currentBudget.month, currentBudget.year);
        return;
    }

    printHeader("CURRENT MONTH EXPENSES");
    printf("\033[1;33m╭───────┬──────────────────────────────────┬──────────────╮\n");
    printf("│  ID   │ Description                      │ Amount (₹)   │\n");
    printf("├───────┼──────────────────────────────────┼──────────────┤\033[0m\n");

    for (int i = 0; i < count; i++) {
        printf("\033[1;36m│ %-5d │ %-32s │ %-12.2f │\033[0m\n", expenses[i].id, expenses[i].description, expenses[i].amount);
    }

    printf("\033[1;33m╰───────┴──────────────────────────────────┴──────────────╯\033[0m\n");
}

void viewEarlierMonthExpenses() {
    Expense expenses[MAX_EXPENSES];
    char month[20];
    int year;

    printf("\033[1;36mEnter the month: \033[0m");
    scanf("%s", month);

    printf("\033[1;36mEnter the year: \033[0m");
    scanf("%d", &year);

    char filename[50];
    getExpenseFileName(filename, month, year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\033[1;31mNo expenses recorded for %s %d.\033[0m\n", month, year);
        return;
    }

    printHeader("EARLIER MONTH EXPENSES");
    printf("\033[1;33m╭───────┬──────────────────────────────────┬──────────────╮\n");
    printf("│  ID   │ Description                      │ Amount (₹)   │\n");
    printf("├───────┼──────────────────────────────────┼──────────────┤\033[0m\n");

    for (int i = 0; i < count; i++) {
        printf("\033[1;36m│ %-5d │ %-32s │ %-12.2f │\033[0m\n", expenses[i].id, expenses[i].description, expenses[i].amount);
    }

    printf("\033[1;33m╰───────┴──────────────────────────────────┴──────────────╯\033[0m\n");
}

void deleteExpense() {
    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\033[1;31mNo expenses recorded for %s %d.\033[0m\n", currentBudget.month, currentBudget.year);
        return;
    }

    viewExpenses();

    printf("\033[1;36mEnter the ID of the expense to delete: \033[0m");
    int id;
    scanf("%d", &id);

    int index = -1;
    for (int i = 0; i < count; i++) {
        if (expenses[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("\033[1;31mError: Expense ID not found.\033[0m\n");
        return;
    }

    currentBudget.spent -= expenses[index].amount;

    for (int i = index; i < count - 1; i++) {
        expenses[i] = expenses[i + 1];
    }
    count--;

    saveExpensesToFile(expenses, count, filename);
    saveBudget();

    printf("\033[1;32mExpense deleted successfully.\033[0m\n");
}

void resetBudget() {
    printf("\033[1;36mEnter the month: \033[0m");
    scanf("%s", currentBudget.month);

    printf("\033[1;36mEnter the year: \033[0m");
    scanf("%d", &currentBudget.year);

    printf("\033[1;36mEnter your budget for %s %d: \033[0m", currentBudget.month, currentBudget.year);
    scanf("%f", &currentBudget.budget);

    currentBudget.spent = 0.0;
    saveBudget();
    printf("\033[1;32mBudget set successfully for %s %d.\033[0m\n", currentBudget.month, currentBudget.year);
}

void getExpenseForCategory() {
    Expense expenses[MAX_EXPENSES];
    char filename[50];
    getExpenseFileName(filename, currentBudget.month, currentBudget.year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\033[1;31mNo expenses recorded for %s %d.\033[0m\n", currentBudget.month, currentBudget.year);
        return;
    }

    char category[MAX_DESC_LEN];
    printf("\033[1;36mEnter the category description to calculate total expense: \033[0m");
    getchar();
    fgets(category, MAX_DESC_LEN, stdin);
    category[strcspn(category, "\n")] = '\0';

    float total = 0.0;
    for (int i = 0; i < count; i++) {
        if (strcasecmp(expenses[i].description, category) == 0) {
            total += expenses[i].amount;
        }
    }

    printf("\033[1;32mTotal expense for '%s': ₹%.2f\033[0m\n", category, total);
}


void viewBalance() {
    float remaining = currentBudget.budget - currentBudget.spent;
    if (remaining < 0) {
        printf("\033[1;31mYou have exceeded your budget by ₹%.2f!\033[0m\n", -remaining);
    } else {
        printf("\033[1;32mRemaining budget for %s %d: ₹%.2f\033[0m\n", currentBudget.month, currentBudget.year, remaining);
    }
}

void generateBill() {
    Expense expenses[MAX_EXPENSES];
    char month[20];
    int year;

    printf("\033[1;36mEnter the month for which to generate the bill: \033[0m");
    scanf("%s", month);
    printf("\033[1;36mEnter the year: \033[0m");
    scanf("%d", &year);

    char filename[50];
    getExpenseFileName(filename, month, year);

    int count = loadExpensesFromFile(expenses, filename);

    if (count == 0) {
        printf("\033[1;31mNo expenses recorded for %s %d. Bill cannot be generated.\033[0m\n", month, year);
        return;
    }

    char billFile[50];
    sprintf(billFile, "bill_%s_%d.txt", month, year);

    FILE *file = fopen(billFile, "w");
    if (file == NULL) {
        perror("Error creating bill file");
        return;
    }

    fprintf(file, "Expense Bill for %s %d\n", month, year);
    fprintf(file, "────────────────────────────────────────────────\n");
    fprintf(file, "ID    Description                     Amount (₹)\n");
    fprintf(file, "────────────────────────────────────────────────\n");

    for (int i = 0; i < count; i++) {
        fprintf(file, "%-5d %-30s ₹%.2f\n", expenses[i].id, expenses[i].description, expenses[i].amount);
    }

    fclose(file);

    printf("\033[1;32mBill successfully generated: %s\033[0m\n", billFile);
}

///////////////////////////////////////////////////


////// THE END //////////
