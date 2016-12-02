#include <stdio.h>
#include <string.h>
#include "items.h"

int itemIndex(struct inventory* i, char itemName[]) {
	int j, amt = i->amt;
	for(j = 0; j < amt; j++) {
		if(strcmp(i->items[j].name, itemName) == 0) {
			return j;
		}
	}

	return -1;
}

int quantityItem(struct inventory* i, char itemName[]) {
	int j, amt = i->amt;

	for(j = 0; j < amt; j++) {
		if(strcmp(i->items[j].name, itemName) == 0) {
			return i->items[j].quantity;
		}
	}

	return 0;
}

void addItem(struct inventory* i, char itemName[], int quantity) {
	int j, amt = i->amt;

	for(j = 0; j < amt; j++) {
		if(strcmp(i->items[j].name, itemName) == 0) {
			i->items[j].quantity += quantity;
			return;
		}
	}

	strcpy(i->items[amt].name, itemName);
	i->items[amt].quantity = quantity;
	i->amt += 1;
	return;
}

void removeItem(struct inventory* i, char itemName[], int quantity) {
	int j, amt = i->amt;

	for(j = 0; j < amt; j++) {
		if(strcmp(i->items[j].name, itemName) == 0) {

			if(quantity < i->items[j].quantity) {
				i->items[j].quantity -= quantity;
				return;
			} else {
				int k;

				for(k = j; k < amt; k++)
				{
					i->items[k] = i->items[k+1];
				}

				i->amt -= 1;
				return;
			}

		}
	}
	return;
}

void buyItem(struct inventory* shopInventory, struct inventory* playerInventory, struct item i) {
	char itemName[25];
	strcpy(itemName, i.name);
	int quantity = i.quantity;
	int value = i.goldValue;

	int quantityHeld = quantityItem(shopInventory, itemName);
	int playerGold = playerInventory->gold;

	if(quantityHeld >= quantity && quantity > 0 && playerGold >= value) {

		int index = itemIndex(shopInventory, itemName);

		addItem(playerInventory, shopInventory->items[index].name, shopInventory->items[index].quantity);
		removeItem(shopInventory, shopInventory->items[index].name, shopInventory->items[index].quantity);

		playerInventory->gold -= value;
		shopInventory->gold += value;
	}
	return;
}
