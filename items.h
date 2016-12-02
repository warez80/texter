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

struct item key = {"key", 1, 5};

struct inventory shop1 = {{{"key",1,5}},1,0};

int itemIndex(struct inventory* i, char itemName[]);
int quantityItem(struct inventory* i, char itemName[]);
void addItem(struct inventory* i, char itemName[], int quantity);
void removeItem(struct inventory* i, char itemName[], int quantity);
void buyItem(struct inventory* shopInventory, struct inventory* playerInventory, struct item item);

#endif
