// InventoryManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <iomanip>

#include <sqlite3.h>

//Database initialization
bool InitializeDatabase(sqlite3*& db)
{
	int status = sqlite3_open("inventory.db", &db);
	if (status != SQLITE_OK)
	{
		std::cerr << "Cannot open databse: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	//Inventory table schema
	const char* createTableSQL =
		"CREATE TABLE IF NOT EXISTS inventory ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name TEXT NOT NULL,"
		"quantity INTEGER NOT NULL,"
		"price REAL NOT NULL"
		");";

	char* errorMsg = nullptr;
	status = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errorMsg);
	if (status != SQLITE_OK)
	{
		std::cerr << "SQL error: " << errorMsg << std::endl;
		sqlite3_free(errorMsg);
		return false;
	}

	std::cout << "Database initialized successfully\n";
	return true;
}

void AddItem(sqlite3* db)
{
	std::string name;

	int quantity;
	double price;

	std::cout << "Enter item name: ";
	std::cin.ignore();
	std::getline(std::cin, name);

	std::cout << "Enter quantity: ";
	while (!(std::cin >> quantity || quantity < 0))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input. Enter a positive integer for quantity: ";
	}

	std::cout << "Enter price: ";
	while (!(std::cin >> price) || price < 0)
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input. Enter a positive number for price: ";
	}


	//SQL statement
	const char* insertSQL = "INSERT INTO inventory (name, quantity, price) VALUES (?, ?, ?)";
	sqlite3_stmt* statement;

	int status = sqlite3_prepare_v2(db, insertSQL, -1, &statement, nullptr);
	if (status != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	sqlite3_bind_text(statement, 1, name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(statement, 2, quantity);
	sqlite3_bind_double(statement, 3, price);

	//Execution
	status = sqlite3_step(statement);
	if (status != SQLITE_DONE)
	{
		std::cerr << "Failed to insert item: " << sqlite3_errmsg(db) << std::endl;
	}
	else
	{
		std::cout << "Item added successfully" << std::endl;
	}

	sqlite3_finalize(statement);
}

void ListItems(sqlite3* db)
{
	const char* selectSQL = "SELECT id, name, quantity, price FROM inventory";
	sqlite3_stmt* statement;

	int status = sqlite3_prepare_v2(db, selectSQL, -1, &statement, nullptr);

	if (status != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	if (sqlite3_step(statement) == SQLITE_DONE) {
		std::cout << "No items found in the inventory.\n";
		return;
	}

	std::cout << "\n--- Inventory Items ---\n";
	std::cout << std::left << std::setw(5) << "ID"
		<< std::setw(20) << "Name"
		<< std::setw(10) << "Quantity"
		<< std::setw(10) << "Price" << "\n";
	std::cout << "--------------------------\n";


	while ((status = sqlite3_step(statement)) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(statement, 0);
		const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
		int quantity = sqlite3_column_double(statement, 2);
		double price = sqlite3_column_double(statement, 3);

		std::cout << std::left << std::setw(5) << id
			<< std::setw(20) << name
			<< std::setw(10) << quantity
			<< std::setw(10) << std::fixed << std::setprecision(2) << price << "\n";
	}

	if (status != SQLITE_DONE)
	{
		std::cerr << "Error listing items: " << sqlite3_errmsg(db) << std::endl;
	}

	sqlite3_finalize(statement);
}

void DisplayMenu()
{
	std::cout << "\nInventory Management System\n";
	std::cout << "1. Add Item\n";
	std::cout << "2. List All Items\n";
	std::cout << "3. Exit\n";
	std::cout << "Enter your choice: ";
}

int main()
{
	sqlite3* db;

	if (!InitializeDatabase(db))
	{
		return 1;
	}

	int choice;

	do
	{
		DisplayMenu();
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (choice)
		{
			case 1: 
				AddItem(db);
				break;
			case 2: 
				ListItems(db);
				break;
			case 3: 
				std::cout << "Closing program...";
				break;
			default:
				std::cout << "Invalid input.\n";
		}

	} while (choice != 3);

	sqlite3_close(db);
	return 0;
}

