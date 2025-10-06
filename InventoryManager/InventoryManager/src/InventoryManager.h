#pragma once

#include "DatabaseManager.h"

#include <iostream>

struct InventoryItem
{
	int id;
	std::string name;
	int quantity;
	double price;
};

class InventoryManager
{
public:

	InventoryManager(DatabaseManager& dbManager);
	~InventoryManager();

	void AddItem();
	void SearchItem();
	void ListItems();
	void RemoveItem();

private:
	DatabaseManager& dbManager;

	bool CheckItem(int id);
	void SearchItemById();
	void SearchItemByName();
};