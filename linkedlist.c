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

typedef struct {
    int index;
    char name[100];
    int price;
} ArrayMenuItem;

typedef struct {
    char name[100];
    int price;
} HeapOrderItem;

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

void freeAllMemory(MenuItem **menuHead, MenuItem **sortedMenuHead, OrderItem **orderRoot) {
    freeMenu(menuHead);
    freeMenu(sortedMenuHead);
    freeOrder(orderRoot);
}

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

OrderItem *searchOrderItem(OrderItem *root, int targetIndex){
    if (root == NULL || targetIndex < 1){
        return NULL;
    }
    
    int count = 0, top = -1;
    OrderItem *result = NULL, *stack[100], *current = root;
    
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

OrderItem *maximum(OrderItem *node) {
    if (node == NULL) return NULL;
    
    while (node->right != NULL) {
        node = node->right;
    }
    return node;
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

    if ((root->left != NULL && root->left == child) || (root->right != NULL && root->right == child)) {
        return root;
    }

    OrderItem *leftParent = parentSearch(root->left, child);
    if (leftParent != NULL) {
        return leftParent;
    }

    return parentSearch(root->right, child);
}

OrderItem* getPredecessor(OrderItem *root, OrderItem *node) {
    if (node == NULL) return NULL;

    if (node->left != NULL)
        return maximum(node->left);

    OrderItem *curr = node;
    OrderItem *parent = parentSearch(root, curr);
    
    while (parent != NULL && parent->left == curr) {
        curr = parent;
        parent = parentSearch(root, curr);
    }
    
    return parent;
}

void deleteOrderItem(OrderItem **root, int targetIndex, int *currentIndex) {
    if (*root == NULL) {
        printf("\nCart is empty (Press any key to continue)");
        getch();
        return;
    }

OrderItem *currentOrderItem = searchOrderItem(*root, targetIndex);
    if (currentOrderItem == NULL) {
        printf("\nItem not found (Press any key to continue)");
        getch();
        return;
    }
    
    printf("\n%s has been removed successfully (Press any key to continue)", currentOrderItem->name);

    if (currentOrderItem->left == NULL && currentOrderItem->right == NULL) {
        if (currentOrderItem == *root) {
            free(*root);
            *root = NULL;
        } else {
            OrderItem *parent = parentSearch(*root, currentOrderItem);
            if (parent->left == currentOrderItem) {
                free(parent->left);
                parent->left = NULL;
            } else {
                free(parent->right);
                parent->right = NULL;
            }
        }

    } else if (currentOrderItem->left == NULL || currentOrderItem->right == NULL) {
        OrderItem *child = (currentOrderItem->left != NULL) ? currentOrderItem->left : currentOrderItem->right;
        if (currentOrderItem == *root) {
            free(*root);
            *root = child;
        } else {
            OrderItem *parent = parentSearch(*root, currentOrderItem);
            if (parent->left == currentOrderItem) {
                free(parent->left);
                parent->left = child;
            } else {
                free(parent->right);
                parent->right = child;
            }
        }

    } else {
        OrderItem *predecessor = maximum(currentOrderItem->left);
        strcpy(currentOrderItem->name, predecessor->name);
        currentOrderItem->price = predecessor->price;

        int predecessorIndex = 1;
        OrderItem *temp = *root;
        OrderItem *stack[100];
        int top = -1;
        
        while (temp != NULL || top != -1) {
            while (temp != NULL) {
                stack[++top] = temp;
                temp = temp->left;
            }
            temp = stack[top--];
            if (temp == predecessor) break;
            predecessorIndex++;
            temp = temp->right;
        }

        deleteOrderItem(root, predecessorIndex, currentIndex);
        return;
    }

    getch();
}

void handleRemoveOrderItem(OrderItem **root, int targetIndex, int *itemIndex) {
    if (*root == NULL) {
        printf("\nCart is empty (Press any key to continue)");
        getch();
        return;
    }
    
    int totalItems = 0;
    OrderItem *temp = *root;
    OrderItem *stack[100];
    int top = -1;
    
    while (temp != NULL || top != -1) {
        while (temp != NULL) {
            stack[++top] = temp;
            temp = temp->left;
        }
        
        temp = stack[top--];
        totalItems++;
        temp = temp->right;
    }
    
    if (targetIndex < 1 || targetIndex > totalItems) {
        printf("\nInvalid item number (Press any key to continue)");
        getch();
        return;
    }
    
    deleteOrderItem(root, targetIndex, itemIndex);
}

void loadMenu(MenuItem **menuHead, MenuItem **menuTail){
    FILE *menuFile = fopen("menu.txt", "r");
    if (!menuFile) {
        printf("Cannot open menu.txt\n");
        freeMenu(menuHead);  
        *menuHead = NULL; 
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
    
    for(int i = 0; i < 5 && currentItem != NULL; i++) {
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

void addFoodToOrder(OrderItem **orderRoot, MenuItem *currentDisplayItem, int displayMode, MenuItem *menuHead, MenuItem *sortedMenuHead){
    int itemNumber;
    printf("Enter food number: ");
    scanf("%d", &itemNumber);
    
    MenuItem *menuItem = NULL;
     if (displayMode == 0) {
        menuItem = menuHead;
        while(menuItem != NULL && menuItem->index != itemNumber) {
            menuItem = menuItem->next;
        }
    } else if (displayMode == 1) {
        menuItem = sortedMenuHead;
        while(menuItem != NULL && menuItem->index != itemNumber) {
            menuItem = menuItem->next;
        }
    }

    if(menuItem == NULL || itemNumber < 1 || itemNumber > 20) {
        printf("There's no such food (Press any key to continue)\n");  
        getch();
        return;
    }

    OrderItem *newOrder = createOrderItem(menuItem->name, menuItem->price); 
    *orderRoot = insertOrderToBST(*orderRoot, newOrder);
}

void sortMenuByName(ArrayMenuItem *menuArray, int count) {
    for(int i = 0; i < count - 1; i++) {
        for(int j = 0; j < count-i-1; j++) {
            if(strcmp(menuArray[j].name, menuArray[j+1].name) > 0) {
                ArrayMenuItem temp = menuArray[j];
                menuArray[j] = menuArray[j+1];
                menuArray[j+1] = temp;
            }
        }
    }
}

void convertMenuToArray(MenuItem *menuHead, ArrayMenuItem *menuArray, int *count){
    MenuItem *current = menuHead;
    *count = 0;
    while(current != NULL && *count < 20) {
        strcpy(menuArray[*count].name, current->name);
        menuArray[*count].index = current->index;
        menuArray[*count].price = current->price;
        (*count)++;
        current = current->next;
    }
}

int binarySearch(ArrayMenuItem *menuArray, int count, char *searchKey) {
    int mid, low, high;
    low = 0;
    high = count-1;
    while (low <= high) {
        mid = (low + high) / 2;
        if (strstr(menuArray[mid].name, searchKey) != NULL) return mid;
        if (strcmp(searchKey, menuArray[mid].name) < 0) high = mid - 1;
        else low = mid + 1;
    }
    return -1;
}

void searchFood(MenuItem *menuHead) {
    char keyword[100];
    int found = 0, count, searchResult;
    ArrayMenuItem menuArray[20];
    
    clearScreen();
    printf("\n====================================\n");
    printf("\tPizza Hut Delivery - Search\n");
    printf("====================================\n");
    
    convertMenuToArray(menuHead, menuArray, &count);
    sortMenuByName(menuArray, count);
    
    printf("Sorted Menu List (For Debugging Purpose)\n");
    for(int i = 0;i<count;i++){
        printf("%d. %s (Rp %d)\n", i+1, menuArray[i].name, menuArray[i].price);
    }
    
    printf("\nEnter food keyword: "); getchar(); scanf("%[^\n]", keyword); 
    
    searchResult = binarySearch(menuArray, count, keyword);
    printf("\nSearch Results for %s:\n", keyword);
    printf("------------------------------------\n");

    if (searchResult != -1) {
        printf("\n%d. %s (Rp %d)\n", menuArray[searchResult].index, menuArray[searchResult].name, menuArray[searchResult].price);
        found = 1;

        int left = searchResult - 1;
        while (left >= 0 && strstr(menuArray[left].name, keyword) != NULL) {
            printf("%d. %s (Rp %d)\n", 
                   menuArray[left].index,
                   menuArray[left].name,
                   menuArray[left].price);
            left--;
        }
        
        int right = searchResult + 1;
        while (right < count && strstr(menuArray[right].name, keyword) != NULL) {
            printf("%d. %s (Rp %d)\n", 
                   menuArray[right].index,
                   menuArray[right].name,
                   menuArray[right].price);
            right++;
        }
    } 
    
    if (!found){
        printf("No item containing %s", keyword);
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
        
        handleRemoveOrderItem(orderRoot, itemNumber, itemIndex);

    } else if (choice == 3){
        return;

    }
}


void writeHeapToFile(HeapOrderItem heapItem[], int heapSize, FILE *file) {
    for (int i = 0; i < heapSize; i++) {
        fprintf(file, "%d. %s - Rp %d\n", i + 1, heapItem[i].name, heapItem[i].price);
    }
}

void swapOrder(HeapOrderItem *a, HeapOrderItem *b) {
    HeapOrderItem temp = *a;
    *a = *b;
    *b = temp;
}

void minheapify(HeapOrderItem heapItem[], int i, int n){
    int lowest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if(left < n && heapItem[left].price < heapItem[lowest].price){
        lowest = left;
    }
    if(right < n && heapItem[right].price < heapItem[lowest].price){
        lowest = right;
    }
    if(lowest != i){
        swapOrder(&heapItem[i], &heapItem[lowest]);
        minheapify(heapItem, lowest, n);
    }
}

void buildMinHeap(HeapOrderItem heapItem[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {  
        minheapify(heapItem, i, n);
    }
}

void convertBSTToArray(OrderItem *orderRoot, HeapOrderItem heapItem[], int *index) {
    if(orderRoot == NULL) return;
    convertBSTToArray(orderRoot->left, heapItem, index);
    
    strcpy(heapItem[*index].name, orderRoot->name);
    heapItem[*index].price = orderRoot->price;
    (*index)++;
    
    convertBSTToArray(orderRoot->right, heapItem, index);
}

void showMinHeap(HeapOrderItem heapItem[], int heapSize) {
    printf("\nYour Cart (Sorted by Price - Min Heap):\n");
    printf("========================================\n");
    for (int i = 0; i < heapSize; i++) {
        printf("%d. %s - Rp %d\n", i + 1, heapItem[i].name, heapItem[i].price);
    }
    printf("========================================\n");
}

int calculateHeapTotal(HeapOrderItem heapItem[], int heapSize) {
    int total = 0;
    for (int i = 0; i < heapSize; i++) {
        total += heapItem[i].price;
    }
    return total;
}
void removeMinHeap(HeapOrderItem heapItem[], int *heapSize) {
    if (*heapSize == 0) return;

    HeapOrderItem last = heapItem[*heapSize - 1];
    (*heapSize)--;

    int ptr = 0;
    int left = 1;
    int right = 2;
    heapItem[ptr] = last;

    while (left < *heapSize) {
        int smallest = ptr;

        if (heapItem[left].price < heapItem[smallest].price)
            smallest = left;

        if (right < *heapSize && heapItem[right].price < heapItem[smallest].price)
            smallest = right;

        if (smallest == ptr) break;

        swapOrder(&heapItem[ptr], &heapItem[smallest]);

        ptr = smallest;
        left = 2 * ptr + 1;
        right = 2 * ptr + 2;
    }
}


void checkout(OrderItem **orderRoot, int *orderCount, int *itemIndex){
    if(*orderRoot == NULL){
        printf("\nCart is empty (Press any key to continue)");
        getch();
        return;
    }

    int total = 0, choice, heapSize = 0;
    OrderItem *current = *orderRoot;
    HeapOrderItem heapItem[100];
    
    convertBSTToArray(*orderRoot, heapItem, &heapSize);
    buildMinHeap(heapItem, heapSize);
    
    while(1){
        clearScreen();
        printf("====================================\n");
        printf("\tPizza Hut Delivery\n");
        printf("====================================");

        showMinHeap(heapItem, heapSize);
        total = calculateHeapTotal(heapItem, heapSize);

        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("\tTotal price : Rp %d\n", total);
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("1. Checkout\n");
        printf("2. Remove last item from order\n");
        printf("3. Cancel payment\n");
        printf("\nChoose: ");
        scanf("%d", &choice);

        if(choice == 1){
            printf("\nThank You for Ordering at Pizza Hut Delivery! (Press any key to continue)");

            FILE *historyFile = fopen("history.txt", "a");
            if(historyFile == NULL) {
                printf("Failed to write history.txt (Press any key to continue)");
            } else { 
                fprintf(historyFile, "Order number %d\n", *orderCount);
                writeHeapToFile(heapItem, heapSize, historyFile);
                fprintf(historyFile, "Total : Rp %d\n", total);
                fprintf(historyFile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                (*orderCount)++;
                *itemIndex = 1;
                freeOrder(orderRoot);
                *orderRoot = NULL;
            }
            fclose(historyFile);
            getch();
            return;

        } else if (choice == 2){
            printf("Removing item: %s - Rp %d\n", heapItem[0].name, heapItem[0].price);
            total -= heapItem[0].price; 
            removeMinHeap(heapItem, &heapSize);
            printf("Item removed! (Press any key to continue)");
            getch();

        } else if (choice == 3){
            return;

        }
    }
}

void showHistory() {
    clearScreen();
    printf("\n====================================\n");
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

void swapMenu(MenuItem* a, MenuItem* b) {
    int tempPrice = a->price, tempIndex = a->index;
    char tempName[100];
    strcpy(tempName, a->name);

    a->price = b->price;
    strcpy(a->name, b->name);
    a->index = b->index;

    b->price = tempPrice;
    strcpy(b->name, tempName);
    b->index = tempIndex;
}

MenuItem *getNodeAt(MenuItem* head, int index) {
    int i = 0;
    while (head != NULL && i < index) {
        head = head->next;
        i++;
    }
    return head;
}

MenuItem *duplicateMenu(MenuItem *menuHead) {
    if (menuHead == NULL) {
        return NULL;
    }

    MenuItem *newHead = NULL, *newTail = NULL;
    
    while(menuHead != NULL) {
        MenuItem *newNode = createMenuItem(menuHead->name, menuHead->price, menuHead->index);
        if (newHead == NULL) {
            newHead = newTail = newNode;
        } else {
            newTail->next = newNode;
            newNode->prev = newTail;
            newTail = newNode;
        }
        menuHead = menuHead->next;
    }

    return newHead;
}

MenuItem *bubbleSortMenuByPrice(MenuItem *sortedMenuHead, int n){
    int sorted = 0, i = 1, j;
    while(!sorted) {
        sorted = 1;
        for(j = n-1;j >= i;j--){
            MenuItem *item1 = getNodeAt(sortedMenuHead, j-1);
            MenuItem *item2 = getNodeAt(sortedMenuHead, j);
            if(item1 != NULL && item2 != NULL && item1->price > item2->price){
                swapMenu(item1, item2);
                sorted = 0;
            }
        }
        i++;
    }
    return sortedMenuHead;
}

MenuItem *selectionSortMenuByPrice(MenuItem *head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    MenuItem *current = head;
    
    while (current != NULL) {
        MenuItem *max = current;
        MenuItem *temp = current->next;

        while (temp != NULL) {
            if (temp->price > max->price) {
                max = temp;
            }
            temp = temp->next;
        }

        if (max != current) {
            swapMenu(current, max);
        }
        
        current = current->next;
    }
    
    return head;
}

void updateIndex(MenuItem **head) {
    MenuItem *current = *head;
    int idx = 1;
    while (current != NULL) {
        current->index = idx++;
        current = current->next;
    }
}

void processOrder(MenuItem **currentItem, MenuItem *menuHead, MenuItem *menuTail, OrderItem **orderRoot, int *orderCount, int *itemIndex, int *displayMode, MenuItem **currentSortedItem, MenuItem **sortedMenuHead, MenuItem **sortedMenuTail) {
    int choice = 0;
    
    while(1){
        char isContinue = 'Y';
        clearScreen();
        
        if(*displayMode == 0) {
            showMenu(*currentItem);
        } else if(*displayMode == 1) {
            showMenu(*currentSortedItem ? *currentSortedItem : *sortedMenuHead);
        }
        showCart(*orderRoot);

        printf("\n1. Next menu page\n");
        printf("2. Prev menu page\n");
        printf("3. Show default menu page\n");
        printf("4. Sort by Price (Ascending)\n");
        printf("5. Sort by Price (Descending)\n");
        printf("6. Order Food\n");
        printf("7. Search Food\n");
        printf("8. Remove Order\n");
        printf("9. Complete order\n");
        printf("10. Back to menu\n");
        printf("Choose: ");
        scanf("%d", &choice);
        
        if(choice == 1 || choice == 2) {
            if(*displayMode == 0){
                navigateMenu(currentItem, menuHead, menuTail, choice);
            } else {
                navigateMenu(currentSortedItem, *sortedMenuHead, *sortedMenuTail, choice);
            }

        } else if(choice == 3){
            *displayMode = 0;
            *currentItem = menuHead;

        } else if(choice == 4 || choice == 5){
            if(*sortedMenuHead != NULL) {
                freeMenu(sortedMenuHead); 
                *sortedMenuHead = NULL;
                *sortedMenuTail = NULL;
            }
            
            *sortedMenuHead = duplicateMenu(menuHead);

            if(choice == 4){
                *sortedMenuHead = bubbleSortMenuByPrice(*sortedMenuHead, 20);
            } else{
                *sortedMenuHead = selectionSortMenuByPrice(*sortedMenuHead);
            }

            *sortedMenuTail = *sortedMenuHead;
            if(*sortedMenuTail != NULL) {
                while((*sortedMenuTail)->next != NULL) {
                    *sortedMenuTail = (*sortedMenuTail)->next;
                }
            }
            updateIndex(sortedMenuHead);
            *displayMode = 1;
            *currentSortedItem = *sortedMenuHead;
            
        } else if (choice == 6) {
            while(isContinue == 'Y' || isContinue == 'y'){
                addFoodToOrder(orderRoot, (*displayMode == 0) ? *currentItem : *currentSortedItem, *displayMode, menuHead, *sortedMenuHead); 
                showCart(*orderRoot);

                printf("Add more food? (y/n): "); 
                getchar();
                scanf("%c", &isContinue);   
            }         
            
        } else if (choice == 7) {
            searchFood(menuHead);

        } else if (choice == 8) {
            modifyOrder(orderRoot, itemIndex);
            
        } else if (choice == 9) {
            checkout(orderRoot, orderCount, itemIndex);

        } else if (choice == 10) {
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
    MenuItem *sortedMenuHead = NULL, *sortedMenuTail = NULL, *currentSortedItem = NULL;

    loadMenu(&menuHead, &menuTail);

    MenuItem *currentItem = menuHead;

    int choice = 0, orderCount = 1, itemIndex = 1, displayMode = 0;

    while(1){
        if(displayMode == 0) {
            showMenu(currentItem);
        } else if(displayMode == 1) {
            showMenu(currentSortedItem ? currentSortedItem : sortedMenuHead);
        }
        
        printf("\n1. Next menu page\n");
        printf("2. Prev menu page\n");
        printf("3. Show default menu page\n");
        printf("4. Sort by Price (Ascending)\n");
        printf("5. Sort by Price (Descending)\n");
        printf("6. Order\n");
        printf("7. Order History\n");
        printf("8. Exit\n");
        printf("Choose: ");
        scanf("%d", &choice);

        if(choice == 1 || choice == 2){
            if(displayMode == 0){
                navigateMenu(&currentItem, menuHead, menuTail, choice);
            } else {
                navigateMenu(&currentSortedItem, sortedMenuHead, sortedMenuTail, choice);
            }

        } else if(choice == 3){
            displayMode = 0;
            currentItem = menuHead;

        } else if(choice == 4 || choice == 5){
            if(sortedMenuHead != NULL) {
                freeMenu(&sortedMenuHead);
                sortedMenuHead = NULL;
                sortedMenuTail = NULL;
                currentSortedItem = NULL;
            }
            
            sortedMenuHead = duplicateMenu(menuHead);
        
            if(choice == 4){
                sortedMenuHead = bubbleSortMenuByPrice(sortedMenuHead, 20);
            } else{
                sortedMenuHead = selectionSortMenuByPrice(sortedMenuHead);
            }

            sortedMenuTail = sortedMenuHead;
            if(sortedMenuTail != NULL) {
                while(sortedMenuTail->next != NULL) {
                    sortedMenuTail = sortedMenuTail->next;
                }
            }
            updateIndex(&sortedMenuHead);

            displayMode = 1;
            currentSortedItem = sortedMenuHead;

        } else if(choice == 6){
            processOrder(&currentItem, menuHead, menuTail, &orderRoot, &orderCount, &itemIndex, &displayMode, &currentSortedItem, &sortedMenuHead, &sortedMenuTail);
            
        } else if(choice == 7){
            showHistory();

        } else if(choice == 8){ 
            freeAllMemory(&menuHead, &sortedMenuHead, &orderRoot);
            break;
        }
    }
    return 0;
}