#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILENAME "expenses.dat"
#define MAX_ITEMS 100
#define MAX_DESC_LENGTH 50

typedef struct {
    char description[MAX_DESC_LENGTH];
    float amount;
} ExpenseItem;

typedef struct {
    int month, year;
    char monthName[20];
    float salary;
    float ratio[3]; 
    ExpenseItem necessity[MAX_ITEMS];
    ExpenseItem entertainment[MAX_ITEMS];
    int necessityCount;
    int entertainmentCount;
} Expense;

int currentMonth = 0, currentYear = 0;
char currentMonthName[20];

void setupMonth();
void addExpense();
void deleteExpense();
void viewCurrentExpenses();
void viewPastExpenses();
void resetMonth();
void generateBill();
void generateItemBill();
void setRatio(float ratio[3]);
void printMenu();
void displayExpenses(const Expense *expense);
Expense *loadMonth(int month, int year);
void saveMonth(const Expense *expense);
void printHeader(const char *title);
void printDivider();

int main() {
    int choice;

    setupMonth();

    while (1) {
        printMenu();
        printf("\033[1;33mEnter your choice: \033[0m");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addExpense();
                break;
            case 2:
                deleteExpense();
                break;
            case 3:
                viewCurrentExpenses();
                break;
            case 4:
                viewPastExpenses();
                break;
            case 5:
                resetMonth();
                break;
            case 6:
                generateBill();
                break;
            case 7:
                generateItemBill();
                break;
            case 8:
                printHeader("Goodbye!");
                printf("\033[1;32mI hope this was user-friendly ;) . See you next time!\033[0m\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

void printMenu() {
    printHeader("EXPENSE TRACKER");
    printf("1. Add Expense\n");
    printf("2. Delete Expense\n");
    printf("3. View Current Month Expenses\n");
    printf("4. View Past Month Expenses\n");
    printf("5. Reset Month and Budget\n");
    printf("6. Generate Bill (All Expenses)\n");
    printf("7. Generate Bill for Specific Item\n");
    printf("8. Exit\n");
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

void setupMonth() {
    Expense expense = {0};

    printf("===== Setup Initial Month =====\n");
    printf("Enter current month (e.g., January): ");
    fgets(currentMonthName, sizeof(currentMonthName), stdin);
    currentMonthName[strcspn(currentMonthName, "\n")] = 0; // Remove trailing newline

    printf("Enter current year: ");
    scanf("%d", &currentYear);

    printf("Enter salary for the month: ");
    scanf("%f", &expense.salary);

    char choice;
    printf("Use standard 50:30:20 ratio? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
        expense.ratio[0] = 50.0;
        expense.ratio[1] = 30.0;
        expense.ratio[2] = 20.0;
    } else {
        setRatio(expense.ratio);
    }

    expense.month = currentMonth = localtime(&(time_t){time(NULL)})->tm_mon + 1;
    strcpy(expense.monthName, currentMonthName);
    expense.year = currentYear;

    expense.necessityCount = 0;
    expense.entertainmentCount = 0;

    saveMonth(&expense);
    printf("Month setup complete for %s %d!\n", currentMonthName, currentYear);
}

void resetMonth() {
    printf("\n===== Reset Month and Budget =====\n");
    setupMonth();
}

void setRatio(float ratio[3]) {
    float sum;
    do {
        printf("Enter ratio for Necessity (percentage): ");
        scanf("%f", &ratio[0]);
        printf("Enter ratio for Entertainment (percentage): ");
        scanf("%f", &ratio[1]);
        printf("Enter ratio for Savings (percentage): ");
        scanf("%f", &ratio[2]);

        sum = ratio[0] + ratio[1] + ratio[2];

        if (sum != 100.0) {
            printf("Invalid ratio! The sum must be 100. Please re-enter.\n");
        }
    } while (sum != 100.0);

    printf("Ratio successfully set to Necessity: %.2f%%, Entertainment: %.2f%%, Savings: %.2f%%\n",
           ratio[0], ratio[1], ratio[2]);
}

Expense *loadMonth(int month, int year) {
    static Expense expense;
    FILE *file = fopen(FILENAME, "rb");

    if (!file) {
        return NULL;
    }

    while (fread(&expense, sizeof(Expense), 1, file)) {
        if (expense.month == month && expense.year == year) {
            fclose(file);
            return &expense;
        }
    }

    fclose(file);
    return NULL;
}

void saveMonth(const Expense *expense) {
    FILE *file = fopen(FILENAME, "rb+");
    Expense temp;

    if (!file) {
        file = fopen(FILENAME, "wb");
    }

    while (fread(&temp, sizeof(Expense), 1, file)) {
        if (temp.year == expense->year && strcmp(temp.monthName, expense->monthName) == 0) {
            fseek(file, -sizeof(Expense), SEEK_CUR);
            fwrite(expense, sizeof(Expense), 1, file);
            fclose(file);
            return;
        }
    }

    fwrite(expense, sizeof(Expense), 1, file);
    fclose(file);
}

void displayExpenses(const Expense *expense) {
    float totalNecessity = 0.0, totalEntertainment = 0.0;

    printf("\n\033[1;34mExpenses for %s %d:\033[0m\n", expense->monthName, expense->year);
    printf("\033[1;36m╭─────────────────────────────────────────────────────────────╮\033[0m\n");
    printf("\033[1;36m│ %-10s │ %-30s │ %-10s │\033[0m\n", "Category", "Description", "Amount (₹)");
    printf("\033[1;36m├───────────┼────────────────────────────────┼─────────────┤\033[0m\n");

    if (expense->necessityCount == 0 && expense->entertainmentCount == 0) {
        printf("\033[1;31m│ No expenses recorded for this month.                     │\033[0m\n");
    } else {
        for (int i = 0; i < expense->necessityCount; i++) {
            printf("\033[1;36m│ %-10s │ %-30s │ %-10.2f │\033[0m\n", "Necessity", expense->necessity[i].description, expense->necessity[i].amount);
            totalNecessity += expense->necessity[i].amount;
        }
        for (int i = 0; i < expense->entertainmentCount; i++) {
            printf("\033[1;36m│ %-10s │ %-30s │ %-10.2f │\033[0m\n", "Entertainment", expense->entertainment[i].description, expense->entertainment[i].amount);
            totalEntertainment += expense->entertainment[i].amount;
        }
    }

    printf("\033[1;36m├───────────┴────────────────────────────────┴─────────────┤\033[0m\n");
    printf("\033[1;36m│ %-43s │ %-10.2f │\033[0m\n", "Total Necessity Expenses", totalNecessity);
    printf("\033[1;36m│ %-43s │ %-10.2f │\033[0m\n", "Total Entertainment Expenses", totalEntertainment);
    printf("\033[1;36m│ %-43s │ %-10.2f │\033[0m\n", "Total Expenses", totalNecessity + totalEntertainment);
    printf("\033[1;36m╰─────────────────────────────────────────────────────────────╯\033[0m\n");
}


void viewCurrentExpenses() {
    Expense *expense = loadMonth(currentMonth, currentYear);

    if (!expense) {
        printf("No data found for the current month.\n");
        return;
    }

    displayExpenses(expense);
}

int monthNameToIndex(const char *monthName) {
    const char *months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    for (int i = 0; i < 12; i++) {
        if (strcasecmp(monthName, months[i]) == 0) {
            return i + 1; 
        }
    }

    return -1; 
}


void viewPastExpenses() {
    char monthName[20];
    int year;

    printf("Enter month and year (e.g., January 2024): ");
    scanf("%19s %d", monthName, &year); 
    getchar(); 

    int monthIndex = monthNameToIndex(monthName);
    if (monthIndex == -1) {
        printf("\033[1;31mInvalid month name entered.\033[0m\n");
        return;
    }

    Expense *expenses = loadMonth(monthIndex, year);
    if (!expenses) {
        printf("\033[1;33mNo details entered for %s %d.\033[0m\n", monthName, year);
        return;
    }

    printf("\n\033[1;34mExpenses for %s %d:\033[0m\n", monthName, year);
    displayExpenses(expenses); 
}




void addExpense() {
    int type;
    char description[MAX_DESC_LENGTH];
    float amount;

    Expense *expense = loadMonth(currentMonth, currentYear);
    if (!expense) {
        printf("No data for the current month. Please setup the month first.\n");
        return;
    }

    printf("Enter type of expense (0 for Necessity, 1 for Entertainment): ");
    scanf("%d", &type);
    printf("Enter description: ");
    scanf(" %[^\n]", description);
    printf("Enter amount: ");
    scanf("%f", &amount);

    if (type == 0 && expense->necessityCount < MAX_ITEMS) {
        strcpy(expense->necessity[expense->necessityCount].description, description);
        expense->necessity[expense->necessityCount].amount = amount;
        expense->necessityCount++;
    } else if (type == 1 && expense->entertainmentCount < MAX_ITEMS) {
        strcpy(expense->entertainment[expense->entertainmentCount].description, description);
        expense->entertainment[expense->entertainmentCount].amount = amount;
        expense->entertainmentCount++;
    } else {
        printf("Expense list full for the selected type.\n");
        return;
    }

    saveMonth(expense);
    printf("Expense added successfully!\n");
}

void deleteExpense() {
    int type;
    char description[MAX_DESC_LENGTH];
    Expense *expense = loadMonth(currentMonth, currentYear);

    if (!expense) {
        printf("No data for the current month. Please setup the month first.\n");
        return;
    }

    printf("Enter type of expense to delete (0 for Necessity, 1 for Entertainment): ");
    scanf("%d", &type);
    printf("Enter description of expense to delete: ");
    scanf(" %[^\n]", description);

    ExpenseItem *list = (type == 0) ? expense->necessity : expense->entertainment;
    int *count = (type == 0) ? &expense->necessityCount : &expense->entertainmentCount;

    for (int i = 0; i < *count; i++) {
        if (strcmp(list[i].description, description) == 0) {
            for (int j = i; j < *count - 1; j++) {
                list[j] = list[j + 1];
            }
            (*count)--;
            saveMonth(expense);
            printf("Expense deleted successfully!\n");
            return;
        }
    }

    printf("Expense not found.\n");
}

void generateBill() {
    Expense *expense = loadMonth(currentMonth, currentYear);

    if (!expense) {
        printf("No data for the current month. Please setup the month first.\n");
        return;
    }

    char filename[50];
    snprintf(filename, sizeof(filename), "bill_%s_%d.txt", expense->monthName, expense->year);

    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error creating bill file.\n");
        return;
    }

    fprintf(file, "===== Expense Bill for %s %d =====\n", expense->monthName, expense->year);
    fprintf(file, "\nNecessity Expenses:\n");
    for (int i = 0; i < expense->necessityCount; i++) {
        fprintf(file, "%s - ₹%.2f\n", expense->necessity[i].description, expense->necessity[i].amount);
    }

    fprintf(file, "\nEntertainment Expenses:\n");
    for (int i = 0; i < expense->entertainmentCount; i++) {
        fprintf(file, "%s - ₹%.2f\n", expense->entertainment[i].description, expense->entertainment[i].amount);
    }

    fclose(file);
    printf("Bill saved to file: %s\n", filename);
}

void generateItemBill() {
    char itemDescription[MAX_DESC_LENGTH];
    Expense *expense = loadMonth(currentMonth, currentYear);

    if (!expense) {
        printf("No data for the current month. Please setup the month first.\n");
        return;
    }

    printf("Enter the item description to generate bill for: ");
    fgets(itemDescription, sizeof(itemDescription), stdin);
    itemDescription[strcspn(itemDescription, "\n")] = 0; // Remove trailing newline



    char filename[50];
    snprintf(filename, sizeof(filename), "item_bill_%s_%d.txt", expense->monthName, expense->year);

    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error creating bill file.\n");
        return;
    }

    fprintf(file, "===== Itemized Bill for '%s' in %s %d =====\n", itemDescription, expense->monthName, expense->year);

    int found = 0;

    fprintf(file, "\nNecessity Expenses:\n");
    for (int i = 0; i < expense->necessityCount; i++) {
        if (strstr(expense->necessity[i].description, itemDescription)) {
            fprintf(file, "%s - ₹%.2f\n", expense->necessity[i].description, expense->necessity[i].amount);
            found = 1;
        }
    }

    fprintf(file, "\nEntertainment Expenses:\n");
    for (int i = 0; i < expense->entertainmentCount; i++) {
        if (strstr(expense->entertainment[i].description, itemDescription)) {
            fprintf(file, "%s - ₹%.2f\n", expense->entertainment[i].description, expense->entertainment[i].amount);
            found = 1;
        }
    }

    fclose(file);

    if (found) {
        printf("Itemized bill saved to file: %s\n", filename);
    } else {
        printf("No expenses found for the item '%s'.\n", itemDescription);
        remove(filename); 
    }
}

