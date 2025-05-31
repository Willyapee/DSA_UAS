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

typedef struct MenuItem {
    int index;
    char name[100];
    int price;
    struct MenuItem *next, *prev;
} MenuItem;

typedef struct OrderItem {
    char name[100];
    int price;
    struct OrderItem *left, *right;
} OrderItem;

void freeMenu(MenuItem **menuHead){
    while(*menuHead != NULL) {
        MenuItem *temp = *menuHead;
        *menuHead = (*menuHead)->next;
        free(temp);
    }
    *menuHead = NULL;
}

void freeOrder(OrderItem **orderRoot){
    if (*orderRoot == NULL) return;

    freeOrder(&(*orderRoot)->left);
    freeOrder(&(*orderRoot)->right);
    free(*orderRoot);
    *orderRoot = NULL;
}

//Memasukkan node baru ke dalam binary search tree
OrderItem* insertOrderToBST(OrderItem *root, OrderItem *newOrder){
    if (root == NULL) return newOrder;

    if (newOrder->price < root->price){
        root->left = insertOrderToBST(root->left, newOrder);
    } else{
        root->right = insertOrderToBST(root->right, newOrder);
    }
    return root;
}

MenuItem *createMenuItem(char *itemName, int itemPrice, int index){
    MenuItem *newItem = (MenuItem *)malloc(sizeof(MenuItem));
    if (!newItem) {
        printf("Failed to create new item\n");
        exit(1);
    }

    strcpy(newItem->name, itemName);
    newItem->price = itemPrice;
    newItem->index = index;

    newItem->next = NULL;
    newItem->prev = NULL;
    return newItem;
}

OrderItem *createOrderItem(char *orderName, int orderPrice){
    OrderItem *newOrder = (OrderItem *)malloc(sizeof(OrderItem));
    if(!newOrder) {
        printf("Failed to create order item\n");
        exit(1);
    }

    strcpy(newOrder->name, orderName);
    newOrder->price = orderPrice;

    newOrder->left = newOrder->right = NULL;
    return newOrder;
}

OrderItem* findMin(OrderItem *node){
    while (node && node->left != NULL)
        node = node->left;
    return node;
}

OrderItem *searchByDynamicIndex(OrderItem *root, int targetIndex, int *currentCount) {
    if (root == NULL) return NULL;

    OrderItem* leftResult = searchByDynamicIndex(root->left, targetIndex, currentCount);
    if (leftResult != NULL) return leftResult;

    (*currentCount)++;
    if (*currentCount == targetIndex) return root;

    return searchByDynamicIndex(root->right, targetIndex, currentCount);
}

OrderItem *searchOrderItem(OrderItem *root, int targetIndex) {
    if (root == NULL) return NULL;
    
    int count = 0;
    OrderItem *result = NULL;

    OrderItem *stack[100];
    int top = -1;
    OrderItem *current = root;
    
    while (current != NULL || top != -1) {
        while (current != NULL) {
            stack[++top] = current;
            current = current->left;
        }
        
        current = stack[top--];
        count++;
        
        if (count == targetIndex) {
            result = current;
            break;
        }
        
        current = current->right;
    }
    
    return result;
}

void handleRemoveOrderItem(OrderItem *root, int targetIndex) {
    OrderItem* item = searchOrderItem(root, targetIndex);
    if (item != NULL) {
        printf("\n%s has been removed successfully (Press any key to continue)", item->name);
        getch();
    } else {
        printf("\nItem not found (Press any key to continue)");
        getch();
    }
}

OrderItem *maximum(OrderItem *node){
    OrderItem* curr = node;
    while (curr != NULL && curr->right != NULL) {
        curr = curr->right;
    }
    return curr;
}

OrderItem *minimum(OrderItem *node){
    OrderItem* curr = node;
    while (curr != NULL && curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}


OrderItem* parentSearch(OrderItem *root, OrderItem *child){
    if (root == NULL || root == child) return NULL;

    if (root->left == child || root->right == child) return root;

    if (child->price < root->price){
        return parentSearch(root->left, child);
    }
    else{
        return parentSearch(root->right, child);
    }
}

OrderItem* getPredecessor(OrderItem *root, OrderItem *node) {
    if (node == NULL) return NULL;

    if (node->left != NULL) {
        OrderItem* curr = node->left;
        while (curr->right != NULL) {
            curr = curr->right;
        }
        return curr;
    }

    OrderItem* parent = parentSearch(root, node);
    OrderItem* curr = node;
    
    while (parent != NULL && parent->right != curr) {
        curr = parent;
        parent = parentSearch(root, curr);
    }
    
    return parent;
}

void deleteOrderItem(OrderItem **root, int targetIndex, int *itemIndex) {
    if (*root == NULL) return;

    OrderItem *curr = searchOrderItem(*root, targetIndex);
    if (curr == NULL) return; 

    OrderItem *parent = parentSearch(*root, curr);

    if (curr->left == NULL && curr->right == NULL) {
        if (parent == NULL) {
            free(*root);
            *root = NULL;
        } else if (parent->left == curr) {
            parent->left = NULL;
            free(curr);
        } else {
            parent->right = NULL;
            free(curr);
        }
    } else if (curr->left == NULL || curr->right == NULL) {
        OrderItem *child = (curr->left != NULL) ? curr->left : curr->right;

        if (parent == NULL) {
            free(*root);
            *root = child;
        } else if (parent->left == curr) {
            parent->left = child;
            free(curr);
        } else {
            parent->right = child;
            free(curr);
        }

    } else {
        OrderItem *pred = getPredecessor(*root, curr);

        curr->price = pred->price;
        strcpy(curr->name, pred->name);

        int predIndex = 1;
        OrderItem *temp = *root;
        while (temp != pred) {
            if (pred->price < temp->price) {
                temp = temp->left;
            } else {
                temp = temp->right;
            }
            predIndex++;
        }
        
        deleteOrderItem(root, predIndex, itemIndex);
    
}

void loadMenu(MenuItem **menuHead, MenuItem **menuTail){
    FILE *menuFile = fopen("menu.txt", "r");
    if (!menuFile) {
        printf("Cannot open menu.txt\n");
        return;
    }

    char itemName[100];
    int itemPrice, index = 1;

    while (!feof(menuFile)){
        fscanf(menuFile, "%[^#]#%d\n", itemName, &itemPrice);
        MenuItem *newItem = createMenuItem(itemName, itemPrice, index);
        MenuItem *current = *menuHead;
        
        if (*menuHead == NULL) {
            *menuHead = newItem;
            newItem->prev = NULL;

        } else {
            while (current->next != NULL){
                current = current->next;
            }
            (*menuTail)->next = newItem;
            newItem->prev = *menuTail;
        }
        *menuTail = newItem;
        (*menuTail)->next = NULL;
        index++;
    }
    fclose(menuFile);
}

void showMenu(MenuItem *currentItem){
    clearScreen();
    printf("====================================\n");
    printf("\tPizza Hut Delivery\n");
    printf("====================================\n");
    printf("Menu List\n");
    
    for(int i = 0; i < 5; i++) {
        printf("%d. %s (Rp %d)\n", currentItem->index, currentItem->name, currentItem->price);
        currentItem = currentItem->next;
    }
}

void inOrder(OrderItem *node, int *index) {
    if (node == NULL) return;

    inOrder(node->left, index);
    printf("%d. %s (Rp %d)\n", (*index)++, node->name, node->price);
    inOrder(node->right, index);
}

void showCart(OrderItem *orderList){
    printf("\nFood Cart\n");
    if(orderList == NULL){
        printf("Empty\n");
        return;
    } 
    
    int index = 1;
    inOrder(orderList, &index);
}

void navigateMenu(MenuItem **currentItem, MenuItem *menuHead, MenuItem *menuTail, int choice) {
    if(choice == 1){
        for(int i = 0; i < 5; i++) {
            if((*currentItem)->next != NULL) {
                *currentItem = (*currentItem)->next;

            } else{
                *currentItem = menuHead;
            }
        }

    } else if(choice == 2){
        for(int i = 0; i < 5; i++){
            if((*currentItem)->prev != NULL){
                *currentItem = (*currentItem)->prev;

            } else{
                *currentItem = menuTail;
            }
        }
    }
    showMenu(*currentItem);
}

void addFoodToOrder(OrderItem **orderRoot, MenuItem *menuHead){
    int itemNumber;
    printf("Enter food number: ");
    scanf("%d", &itemNumber);
    
    MenuItem *menuItem = menuHead;
    while(menuItem != NULL && menuItem->index != itemNumber) {
        menuItem = menuItem->next;
    }

    if(menuItem == NULL || itemNumber < 1 || itemNumber > 20) {
        printf("There's no such food (Press any key to continue)\n");  
        getch();
        return;
    }

    OrderItem *newOrder = createOrderItem(menuItem->name, menuItem->price); 
    *orderRoot = insertOrderToBST(*orderRoot, newOrder);
}

void searchFood(MenuItem *menuHead) {
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
    
    while(menuHead != NULL) {
        char menuName[100];
        strcpy(menuName, menuHead->name);
        
        char *pos = strstr(menuName, keyword);
        if(pos != NULL) {
            printf("%d. %s (Rp %d)\n", menuHead->index, menuHead->name, menuHead->price);
            found = 1;
        }
        menuHead = menuHead->next;
    }
    
    if(!found) {
        printf("No items containing %s", keyword);
    }
    
    printf("\nPress any key to continue...");
    getch();
}


void modifyOrder(OrderItem **orderRoot, int *itemIndex){
    int choice, itemNumber;

    if(*orderRoot == NULL) {
        printf("\nCart is empty (Press any key to continue)");
        getch();
        return;
    }

    printf("\n1. Delete entire cart\n");
    printf("2. Delete ordered food\n");
    printf("3. Cancel remove\n");
    printf("Choose: ");
    scanf("%d", &choice);

    if(choice == 1) {
        freeOrder(orderRoot);
        *itemIndex = 1;
        printf("\nFood cart has been removed successfully (Press any key to continue)");
        getch();

    } else if(choice == 2) {
        printf("\nEnter ordered food number: ");
        scanf("%d", &itemNumber);
        
        OrderItem* item = searchOrderItem(*orderRoot, itemNumber);
        if(item == NULL){
            printf("\nNo such item (Press any key to continue)");
            getch();
            return;
        }
        
        deleteOrderItem(orderRoot, itemNumber, itemIndex);
        handleRemoveOrderItem(*orderRoot, itemNumber);

    } else if (choice == 3){
        return;

    }
}

void calculateTotal(OrderItem *node, int *total){
    if (node == NULL) return;
    calculateTotal(node->left, total);
    *total += node->price;
    calculateTotal(node->right, total);
}

void writeOrderToFile(OrderItem *node, FILE *file){
    if (node == NULL) return;
    writeOrderToFile(node->left, file, currentIndex);
    fprintf(file, "%d. %s (Rp %d)\n", (*currentIndex)++, node->name, node->price);
    writeOrderToFile(node->right, file, currentIndex);
}

void checkout(OrderItem **orderRoot, int *orderCount, int *itemIndex){
    int total = 0, choice;
    OrderItem *current = *orderRoot;
    
    if(*orderRoot == NULL){
        printf("\nCart is empty (Press any key to continue)");
        getch();
        return;
    }

    clearScreen();
    printf("====================================\n");
    printf("\tPizza Hut Delivery\n");
    printf("====================================");
    showCart(*orderRoot);

    calculateTotal(*orderRoot, &total);

    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("\tTotal price : Rp %d\n", total);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("1. Checkout\n");
    printf("2. Cancel payment\n");
    printf("\nChoose: ");
    scanf("%d", &choice);

    if(choice == 1){
        printf("\nThank You for Ordering at Pizza Hut Delivery! (Press any key to continue)");

        FILE *historyFile = fopen("history.txt", "a");
        if(historyFile == NULL) {
            printf("Failed to write history.txt (Press any key to continue)");
        } else { 
            fprintf(historyFile, "Order number %d\n", *orderCount);
            int fileIndex = 1;
            writeOrderToFile(*orderRoot, historyFile, &fileIndex);
            fprintf(historyFile, "Total : Rp %d\n", total);
            fprintf(historyFile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            (*orderCount)++;
            *itemIndex = 1;
            freeOrder(orderRoot);
            *orderRoot = NULL;
        }
        fclose(historyFile);
        getch();

    } else if (choice == 2){
        return;

    }
}

void showHistory() {
    clearScreen();
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

void processOrder(MenuItem **currentItem, MenuItem *menuHead, MenuItem *menuTail, OrderItem **orderRoot, int *orderCount, int *itemIndex){
    int choice = 0;
    char isContinue = 'Y';
    
    while(choice != 7){
        clearScreen();
        showMenu(*currentItem);
        showCart(*orderRoot);

        printf("\n1. Next menu page\n");
        printf("2. Prev menu page\n");
        printf("3. Order Food\n");
        printf("4. Search Food\n");
        printf("5. Remove Order\n");
        printf("6. Complete order\n");
        printf("7. Back to menu\n");
        printf("Choose: ");
        scanf("%d", &choice);
        
        if(choice == 1 || choice == 2) {
            navigateMenu(currentItem, menuHead, menuTail, choice);

        } else if (choice == 3) {
            while(isContinue == 'Y' || isContinue == 'y'){
                addFoodToOrder(orderRoot, menuHead, itemIndex);  
                showCart(*orderRoot);

                printf("Add more food? (y/n): "); 
                getchar();
                scanf("%c", &isContinue);   
            }
                
            
        } else if (choice == 4) {
            searchFood(menuHead);

        } else if (choice == 5) {
            modifyOrder(orderRoot, itemIndex);

        } else if (choice == 6) {
            checkout(orderRoot, orderCount, itemIndex);

        } else if (choice == 7) {
            break;

        }
    }
}

int main(){
    FILE *historyFile = fopen("history.txt", "w");
    fprintf(historyFile, "");
    fclose(historyFile);

    OrderItem *orderRoot = NULL;
    MenuItem *menuHead = NULL, *menuTail = NULL;
    loadMenu(&menuHead, &menuTail);

    MenuItem *currentItem = menuHead;
    int choice = 0, orderCount = 1, itemIndex = 1;
    
    while(1){
        showMenu(currentItem);
        printf("\n1. Next menu page\n");
        printf("2. Prev menu page\n");
        printf("3. Order\n");
        printf("4. Order History\n");
        printf("5. Exit\n");
        printf("Choose: ");
        scanf("%d", &choice);

        if(choice == 1 || choice == 2){
            navigateMenu(&currentItem, menuHead, menuTail, choice);

        } else if(choice == 3){
            processOrder(&currentItem, menuHead, menuTail, &orderRoot, &orderCount, &itemIndex);
        
        } else if(choice == 4){
            showHistory();

        } else if(choice == 5){ 
            freeMenu(&menuHead);
            freeOrder(&orderRoot);
            break;
        }
    }
    return 0;
}