#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EXPENSES 100
#define MAX_MONTHS 12
#define DATA_FILE "expenses.dat"

typedef struct {
    char category[50];
    float amount;
    char type[20]; // "Compulsory" or "Entertainment"
} Expense;

typedef struct {
    int month;
    float monthly_salary;
    float savings;
    float compulsory_total;
    float entertainment_budget;
    Expense expenses[MAX_EXPENSES];
    int expense_count;
} MonthlyExpenses;

MonthlyExpenses history[MAX_MONTHS];
int current_month_index = -1;

// Function Prototypes
void initialize_month();
void add_expense();
void delete_expense();
void view_current_expenses();
void view_past_expenses();
void save_data();
void load_data();
void clear_input_buffer();

int main() {
    load_data(); // Load saved data on program start
    int choice;
    while (1) {
        printf("\n--- Expense Tracker ---\n");
        printf("1. Initialize Current Month\n");
        printf("2. Add Expense\n");
        printf("3. Delete Expense\n");
        printf("4. View Current Month Expenses\n");
        printf("5. View Past Month Expenses\n");
        printf("6. Save and Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice) {
            case 1:
                initialize_month();
                break;
            case 2:
                add_expense();
                break;
            case 3:
                delete_expense();
                break;
            case 4:
                view_current_expenses();
                break;
            case 5:
                view_past_expenses();
                break;
            case 6:
                save_data();
                printf("Data saved. Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}

// Function to clear input buffer
void clear_input_buffer() {
    while (getchar() != '\n');
}

// Save data to a file
void save_data() {
    FILE *file = fopen(DATA_FILE, "wb");
    if (file == NULL) {
        perror("Error saving data");
        return;
    }
    fwrite(&current_month_index, sizeof(int), 1, file);
    fwrite(history, sizeof(MonthlyExpenses), current_month_index + 1, file);
    fclose(file);
}

// Load data from a file
void load_data() {
    FILE *file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("No previous data found. Starting fresh.\n");
        return;
    }
    fread(&current_month_index, sizeof(int), 1, file);
    fread(history, sizeof(MonthlyExpenses), current_month_index + 1, file);
    fclose(file);
}

// Initialize a new month
void initialize_month() {
    if (current_month_index >= MAX_MONTHS - 1) {
        printf("Maximum month history reached. Cannot initialize more months.\n");
        return;
    }

    current_month_index++;
    MonthlyExpenses *current_month = &history[current_month_index];
    current_month->expense_count = 0;
    current_month->compulsory_total = 0;

    printf("\n--- Initializing Current Month ---\n");
    printf("Enter month number (1-12): ");
    scanf("%d", &current_month->month);
    clear_input_buffer();

    printf("Enter monthly salary: ");
    scanf("%f", &current_month->monthly_salary);
    clear_input_buffer();

    printf("Enter savings for the month: ");
    scanf("%f", &current_month->savings);
    clear_input_buffer();

    // Automatically calculate entertainment budget
    current_month->entertainment_budget = current_month->monthly_salary - current_month->savings;

    if (current_month->entertainment_budget < 0) {
        printf("Warning: Savings exceed the monthly salary. Entertainment budget set to 0.\n");
        current_month->entertainment_budget = 0;
    }

    printf("\nMonth Initialized Successfully!\n");
    printf("Initial Entertainment Budget: %.2f\n", current_month->entertainment_budget);
}

// Add a new expense
void add_expense() {
    if (current_month_index == -1) {
        printf("No month initialized. Please initialize the current month first.\n");
        return;
    }

    MonthlyExpenses *current_month = &history[current_month_index];
    if (current_month->expense_count >= MAX_EXPENSES) {
        printf("Maximum expenses reached for the month.\n");
        return;
    }

    Expense *new_expense = &current_month->expenses[current_month->expense_count];

    printf("Enter expense category: ");
    fgets(new_expense->category, 50, stdin);
    new_expense->category[strcspn(new_expense->category, "\n")] = '\0'; // Remove newline character

    printf("Enter expense amount: ");
    scanf("%f", &new_expense->amount);
    clear_input_buffer();

    printf("Select expense type:\n");
    printf("1. Compulsory Expenditure\n");
    printf("2. Entertainment\n");
    printf("Enter your choice: ");
    int type_choice;
    scanf("%d", &type_choice);
    clear_input_buffer();

    if (type_choice == 1) {
        strcpy(new_expense->type, "Compulsory");
        current_month->compulsory_total += new_expense->amount;
    } else if (type_choice == 2) {
        strcpy(new_expense->type, "Entertainment");
        current_month->entertainment_budget -= new_expense->amount;
        if (current_month->entertainment_budget < 0) {
            printf("Warning: Entertainment budget exceeded!\n");
        }
    } else {
        printf("Invalid choice. Expense not added.\n");
        return;
    }

    current_month->expense_count++;
    printf("Expense added successfully!\n");
}

// Delete an expense
void delete_expense() {
    if (current_month_index == -1) {
        printf("No month initialized. Please initialize the current month first.\n");
        return;
    }

    MonthlyExpenses *current_month = &history[current_month_index];
    if (current_month->expense_count == 0) {
        printf("No expenses to delete.\n");
        return;
    }

    printf("Enter the expense index to delete (0 to %d): ", current_month->expense_count - 1);
    int index;
    scanf("%d", &index);
    clear_input_buffer();

    if (index < 0 || index >= current_month->expense_count) {
        printf("Invalid index.\n");
        return;
    }

    Expense *deleted_expense = &current_month->expenses[index];
    if (strcmp(deleted_expense->type, "Compulsory") == 0) {
        current_month->compulsory_total -= deleted_expense->amount;
    } else if (strcmp(deleted_expense->type, "Entertainment") == 0) {
        current_month->entertainment_budget += deleted_expense->amount;
    }

    for (int i = index; i < current_month->expense_count - 1; i++) {
        current_month->expenses[i] = current_month->expenses[i + 1];
    }
    current_month->expense_count--;
    printf("Expense deleted successfully!\n");
}

// View current month expenses
void view_current_expenses() {
    if (current_month_index == -1) {
        printf("No month initialized. Please initialize the current month first.\n");
        return;
    }

    MonthlyExpenses *current_month = &history[current_month_index];
    printf("\n--- Current Month Expenses (Month: %d) ---\n", current_month->month);
    printf("Monthly Salary: %.2f\n", current_month->monthly_salary);
    printf("Savings: %.2f\n", current_month->savings);
    printf("Compulsory Expenditure Total: %.2f\n", current_month->compulsory_total);
    printf("Remaining Entertainment Budget: %.2f\n", current_month->entertainment_budget);
    printf("\nExpenses:\n");
    for (int i = 0; i < current_month->expense_count; i++) {
        printf("%d. [%s] %s - %.2f\n", i, current_month->expenses[i].type, current_month->expenses[i].category, current_month->expenses[i].amount);
    }
}

// View past month expenses
void view_past_expenses() {
    if (current_month_index == -1) {
        printf("No month initialized yet.\n");
        return;
    }

    printf("\n--- Past Months Expenses ---\n");
    for (int i = 0; i <= current_month_index; i++) {
        MonthlyExpenses *month = &history[i];
        printf("\nMonth: %d\n", month->month);
        printf("Monthly Salary: %.2f\n", month->monthly_salary);
        printf("Savings: %.2f\n", month->savings);
        printf("Compulsory Expenditure Total: %.2f\n", month->compulsory_total);
        printf("Remaining Entertainment Budget: %.2f\n", month->entertainment_budget);
        printf("Expenses:\n");
        for (int j = 0; j < month->expense_count; j++) {
            printf("%d. [%s] %s - %.2f\n", j, month->expenses[j].type, month->expenses[j].category, month->expenses[j].amount);
        }
    }
}
