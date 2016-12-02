// inclusion guards
#ifndef _ITEMS_H_
#define _ITEMS_H_

#define MAXINVENTORY 50
#include <string.h>

// structs for individual items and inventories
struct item {
	char name[25];
	int quantity;
	int goldValue;
};

struct inventory {
	struct item items[MAXINVENTORY];
	int amt;
	int gold;
};

struct item sword = {"sword", 1, 100};
struct item torch = {"torch", 1 , 2};
struct item key = {"key", 1, 5};
struct item grappler = {"grapple", 1, 10};

struct inventory shop1 = {{{"key",1,5}},1,0};

int itemIndex(struct inventory* i, char itemName[]);
int quantityItem(struct inventory* i, char itemName[]);
void addItem(struct inventory* i, char itemName[], int quantity);
void removeItem(struct inventory* i, char itemName[], int quantity);
int buyItem(struct inventory* shopInventory, struct inventory* playerInventory, struct item item);
void shopMenu(struct inventory* shopInventory, struct inventory* playerInventory);

#endif
