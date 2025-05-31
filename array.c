#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

void clearScreen(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

typedef struct {
    int index;
    char name[100];
    int price;
} OrderItem;

typedef struct {
    int data[50]; 
    int size; 
    int top;              
} OrderStack;

int pop(OrderStack *stack) {
    if (stack->top == -1) {
        return -1; // Stack kosong
    }
    int poppedValue = stack->data[stack->top];
    stack->top--;
    stack->size--;
    return poppedValue;
}

void loadMenu(OrderItem menu[], int *menuSize) {
    FILE *menuFile = fopen("menu.txt", "r");
    if (!menuFile) {
        printf("Cannot open menu.txt\n");
        return;
    }

    *menuSize = 0;
    while (!feof(menuFile)) {
        fscanf(menuFile, "%[^#]#%d\n", menu[*menuSize].name, &menu[*menuSize].price);
        menu[*menuSize].index = *menuSize + 1;
        (*menuSize)++;
    }
    fclose(menuFile);
}

void showMenu(OrderItem menu[], int menuSize, int startIndex) {
    clearScreen();
    printf("====================================\n");
    printf("\tPizza Hut Delivery\n");
    printf("====================================\n");
    printf("Menu List\n");
    
    for (int i = 0; i < 5 && (startIndex + i) < menuSize; i++) {
        printf("%d. %s (Rp %d)\n", menu[startIndex + i].index, menu[startIndex + i].name, menu[startIndex + i].price);
    }
}

void showCart(OrderStack *orderStack, OrderItem menu[]) {
    printf("\nFood Cart\n");
    if (orderStack->top == -1) {
        printf("Empty\n");

    } else {
        for (int i = 0; i <= orderStack->top; i++) {
            int itemIndex = orderStack->data[i] - 1;
            printf("%d. %s (Rp %d)\n", i + 1, menu[itemIndex].name, menu[itemIndex].price);
        }
    }
}

void navigateMenu(int *currentItem, int menuSize, int choice) {
    if (choice == 1) {
        *currentItem += 5;
        if (*currentItem >= menuSize) {
            *currentItem = 0;
        }

    } else if (choice == 2) {
        *currentItem -= 5;
        if (*currentItem < 0) {
            *currentItem = (menuSize / 5) * 5;
            if (*currentItem >= menuSize) {
                *currentItem -= 5;
            }
        }
    }
}

void initializeStack(OrderStack *stack) {
    stack->top = -1;  
    stack->size = 0;  
}

void addFoodToOrder(OrderStack *orderStack, OrderItem menu[], int menuSize) {
    char isContinue = 'Y';

    while (isContinue == 'Y' || isContinue == 'y') {
        int itemNumber;
        printf("\nEnter food number: ");
        scanf("%d", &itemNumber);

        if (itemNumber < 1 || itemNumber > menuSize) {
            printf("Invalid selection! (Press any key to continue)\n");
            getch();
            continue;
        }

        if (orderStack->size >= 50) { 
            printf("Cart is full! (Press any key to continue)\n");
            getch();
            break;
        }

        orderStack->top++;
        orderStack->data[orderStack->top] = itemNumber;  
        orderStack->size++;  

        showCart(orderStack, menu);

        printf("\nAdd more food? (y/n): ");
        getchar();
        scanf("%c", &isContinue); 
    } 
}

void searchFood(OrderItem menu[], int menuSize) {
    char keyword[100];
    int found = 0;
    clearScreen();
    printf("====================================\n");
    printf("\tPizza Hut Delivery - Search\n");
    printf("====================================\n");
    
    printf("Enter food keyword: ");
    getchar();
    scanf("%[^\n]", keyword);
    
    printf("\nSearch Results for %s:\n", keyword);
    printf("------------------------------------\n");
    
    for (int i = 0; i < menuSize; i++) {
        if (strstr(menu[i].name, keyword) != NULL) {
            printf("%d. %s (Rp %d)\n", menu[i].index, menu[i].name, menu[i].price);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No items containing %s", keyword);
    }
    
    printf("\nPress any key to continue...");
    getch();
}

void modifyOrder(OrderStack *orderStack, OrderItem menu[]) {
    if (orderStack->top == -1) {
        printf("\nCart is empty (Press any key to continue)");
        getch();
        return;
    }

    int choice;
    printf("\n1. Clear entire cart\n");
    printf("2. Remove last item\n");
    printf("3. Cancel\n");
    printf("Choose: ");
    scanf("%d", &choice);

    if (choice == 1) {
        orderStack->top = -1;
        printf("\nCart has been cleared (Press any key to continue)");
        getchar();

    } else if (choice == 2) {
        int itemIndex = orderStack->data[orderStack->top] - 1;
        printf("\n%s has been removed from the cart (Press any key to continue)", menu[itemIndex].name);
        orderStack->top--;
        orderStack->size--;
        getch();

    } else if (choice == 3){
        return;

    }
}

void checkout(OrderStack *orderStack, int *orderCount, OrderItem menu[]) {
    int total = 0, choice;

    if (orderStack->top == -1) {
        printf("\nCart is empty (Press any key to continue)");
        getch();
        return;
    }

    printf("====================================\n");
    printf("\tPizza Hut Delivery\n");
    printf("====================================\n");
    showCart(orderStack, menu);

    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    FILE *historyFile = NULL;
    if ((historyFile = fopen("history.txt", "a")) == NULL) {
        printf("Failed to write history.txt (Press any key to continue)");
        getch();
        return;
    }

    fprintf(historyFile, "Order number %d\n", *orderCount);

    while (orderStack->top != -1) {
        int itemCode = pop(orderStack);  // index di stack adalah index+1
        if (itemCode == -1) break; // prevent error kalau kosong

        int itemIndex = itemCode - 1;
        total += menu[itemIndex].price;

        fprintf(historyFile, "%d. %s (Rp %d)\n", menu[itemIndex].index, menu[itemIndex].name, menu[itemIndex].price);
    }

    fprintf(historyFile, "Total : Rp %d\n", total);
    fprintf(historyFile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    fclose(historyFile);

    printf("\tTotal price : Rp %d\n", total);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("1. Checkout\n");
    printf("2. Cancel payment\n");
    printf("\nChoose: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("\nThank You for Ordering at Pizza Hut Delivery! (Press any key to continue)");
        (*orderCount)++;
        getch();
    } else if (choice == 2) {
        printf("\nPayment canceled. Cart has been cleared. (Press any key to continue)");
        getch();
    }
}

void showHistory() {
    printf("====================================\n");
    printf("\tPizza Hut Delivery\n");
    printf("====================================\n");

    FILE *historyFile = fopen("history.txt", "r");
    if (historyFile == NULL) {
        printf("No order history found (Press any key to continue)\n");
        getch();
        return;
    }
    
    char line[256] = "";
    int hasContent = 0;

    while (fscanf(historyFile, " %255[^\n]%*c", line) == 1) {
        printf("%s\n", line);
        hasContent = 1;
    }

    if (!hasContent) {
        printf("No order history available.\n");
    }

    fclose(historyFile);
    printf("\nPress any key to continue...");
    getch();
}

void processOrder(OrderItem menu[], int menuSize, int *currentItem, 
    OrderStack *orderStack, int *orderCount) {
    int choice = 0;

    while (choice != 7) {
        clearScreen();
        showMenu(menu, menuSize, *currentItem);
        showCart(orderStack, menu);

        printf("\n1. Next menu page\n");
        printf("2. Prev menu page\n");
        printf("3. Order Food\n");
        printf("4. Search Food\n");
        printf("5. Remove Order\n"); 
        printf("6. Complete order\n");
        printf("7. Back to menu\n");
        printf("Choose: ");
        scanf("%d", &choice);

        if (choice == 1 || choice == 2) {
            navigateMenu(currentItem, menuSize, choice);

        } else if (choice == 3) {
            addFoodToOrder(orderStack, menu, menuSize);

        } else if (choice == 4) {
            searchFood(menu, menuSize);

        } else if (choice == 5) {
            modifyOrder(orderStack, menu); 

        } else if (choice == 6) {
            checkout(orderStack, orderCount, menu);

        } else if (choice == 7){
            break;

        }
    }
}

int main() {
    FILE *historyFile = fopen("history.txt", "w");
    fprintf(historyFile, "");
    fclose(historyFile);

    OrderItem menu[100];
    OrderStack orderStack;
    int menuSize = 0;
    int currentItem = 0, orderCount = 1;
    int choice = 0;

    initializeStack(&orderStack);
    loadMenu(menu, &menuSize);

    while (1) {
        showMenu(menu, menuSize, currentItem);
        printf("\n1. Next menu page\n");
        printf("2. Prev menu page\n");
        printf("3. Order\n");
        printf("4. Order History\n");
        printf("5. Exit\n");
        printf("Choose: ");
        scanf("%d", &choice);

        if (choice == 1 || choice == 2) {
            navigateMenu(&currentItem, menuSize, choice);

        } else if (choice == 3) {
            processOrder(menu, menuSize, &currentItem, &orderStack, &orderCount);
        
        } else if (choice == 4) {
            showHistory();
        
        } else if (choice == 5) {
            break;
        
        }
    }
    return 0;
}