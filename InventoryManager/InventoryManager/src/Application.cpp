// InventoryManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "sqlite3.h"

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

	std::cout << "Database initialized successfully" << std::endl;
	return true;
}


int main() 
{
	sqlite3* db;
	int rc = sqlite3_open("inventory.db", &db);
	if (rc != SQLITE_OK) 
	{
		std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
		return 1;
	}
	std::cout << "Database opened successfully!\n";

	InitializeDatabase(db);

	std::cin.get();
	sqlite3_close(db);
	return 0;
}

