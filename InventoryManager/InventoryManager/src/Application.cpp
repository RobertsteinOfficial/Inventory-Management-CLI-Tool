// InventoryManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "DatabaseManager.h"
#include "InventoryManager.h"


void DisplayMenu()
{
	std::cout << "\nInventory Management System\n";
	std::cout << "1. Add Item\n";
	std::cout << "2. List All Items\n";
	std::cout << "3. Search Item\n";
	std::cout << "4. Remove Item\n";
	std::cout << "5. Exit\n";
	std::cout << "Enter your choice: ";
}

int main()
{
	DatabaseManager dbManager;

	if (!dbManager.OpenDatabase("inventory.db")) { return 1; }
	if (!dbManager.InitializeDatabase()) { return 1; }

	InventoryManager inventoryManager(dbManager);

	int choice;

	do
	{
		DisplayMenu();
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (choice)
		{
		case 1:
			inventoryManager.AddItem();
			break;
		case 2:
			inventoryManager.ListItems();
			break;
		case 3:
			inventoryManager.SearchItem();
			break;
		case 4:
			inventoryManager.RemoveItem();
			break;
		case 5:
			std::cout << "Closing program...";
			break;
		default:
			std::cout << "Invalid input.\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			break;
		}

	} while (choice != 5);

	return 0;
}

