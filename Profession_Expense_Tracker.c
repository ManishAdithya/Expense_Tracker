#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    float ratio[3]; // Ratio: Necessity, Entertainment, Savings
    ExpenseItem necessity[MAX_ITEMS];
    ExpenseItem entertainment[MAX_ITEMS];
    int necessityCount;
    int entertainmentCount;
} Expense;

// Global variables
int currentMonth = 0, currentYear = 0;
char currentMonthName[20];

// Function prototypes
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

int main() {
    int choice;

    setupMonth(); // Initialize the first month setup

    while (1) {
        printMenu();
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
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

void printMenu() {
    printf("\n===== Expense Tracker =====\n");
    printf("1. Add Expense\n");
    printf("2. Delete Expense\n");
    printf("3. View Current Month Expenses\n");
    printf("4. View Past Month Expenses\n");
    printf("5. Reset Month and Budget\n");
    printf("6. Generate Bill (All Expenses)\n");
    printf("7. Generate Bill for Specific Item\n");
    printf("8. Exit\n");
    printf("Enter your choice: ");
}

void setupMonth() {
    Expense expense = {0};

    printf("===== Setup Initial Month =====\n");
    printf("Enter current month (e.g., January): ");
    scanf(" %[^\n]", currentMonthName);

    printf("Enter current year: ");
    scanf("%d", &currentYear);

    expense.year = currentYear;
    strcpy(expense.monthName, currentMonthName);

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

    printf("\nExpenses for %s %d:\n", expense->monthName, expense->year);
    printf("Necessity Expenses:\n");
    for (int i = 0; i < expense->necessityCount; i++) {
        printf("%d. %s - ₹%.2f\n", i + 1, expense->necessity[i].description, expense->necessity[i].amount);
        totalNecessity += expense->necessity[i].amount;
    }

    printf("\nEntertainment Expenses:\n");
    for (int i = 0; i < expense->entertainmentCount; i++) {
        printf("%d. %s - ₹%.2f\n", i + 1, expense->entertainment[i].description, expense->entertainment[i].amount);
        totalEntertainment += expense->entertainment[i].amount;
    }

    printf("\nTotal Necessity: ₹%.2f\n", totalNecessity);
    printf("Total Entertainment: ₹%.2f\n", totalEntertainment);
    printf("Total Expenses: ₹%.2f\n", totalNecessity + totalEntertainment);
}

void viewCurrentExpenses() {
    Expense *expense = loadMonth(currentMonth, currentYear);

    if (!expense) {
        printf("No data found for the current month.\n");
        return;
    }

    displayExpenses(expense);
}

void viewPastExpenses() {
    char monthName[20];
    int year;

    printf("Enter month (e.g., January): ");
    scanf(" %[^\n]", monthName);
    printf("Enter year: ");
    scanf("%d", &year);

    Expense *expense = loadMonth(currentMonth, currentYear);
    if (!expense) {
        printf("No data found for %s %d.\n", monthName, year);
        return;
    }

    displayExpenses(expense);
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
    float total = 0.0;
    Expense *expense = loadMonth(currentMonth, currentYear);

    if (!expense) {
        printf("No data for the current month. Please setup the month first.\n");
        return;
    }

    printf("\nEnter the item description to calculate total spent: ");
    scanf(" %[^\n]", itemDescription);

    for (int i = 0; i < expense->necessityCount; i++) {
        if (strcmp(expense->necessity[i].description, itemDescription) == 0) {
            total += expense->necessity[i].amount;
        }
    }

    for (int i = 0; i < expense->entertainmentCount; i++) {
        if (strcmp(expense->entertainment[i].description, itemDescription) == 0) {
            total += expense->entertainment[i].amount;
        }
    }

    printf("Total amount spent on '%s': ₹%.2f\n", itemDescription, total);
}
