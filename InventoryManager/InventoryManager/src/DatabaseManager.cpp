#include "DatabaseManager.h"

#include <iostream>

DatabaseManager::DatabaseManager() : db(nullptr) {}

DatabaseManager::~DatabaseManager()
{
	CloseDatabase();
}

bool DatabaseManager::InitializeDatabase()
{
	//Inventory table schema
	const char* createTableSQL =
		"CREATE TABLE IF NOT EXISTS inventory ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"name TEXT NOT NULL,"
		"quantity INTEGER NOT NULL,"
		"price REAL NOT NULL"
		");";

	char* errorMsg = nullptr;
	int status = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errorMsg);
	if (status != SQLITE_OK)
	{
		std::cerr << "SQL error: " << errorMsg << std::endl;
		sqlite3_free(errorMsg);
		return false;
	}

	std::cout << "Inventory table is ready.\n";
	return true;
}

bool DatabaseManager::OpenDatabase(const std::string& dbName)
{
	int status = sqlite3_open(dbName.c_str(), &db);

	if (status != SQLITE_OK)
	{
		std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	return true;
}

void DatabaseManager::CloseDatabase()
{
	if (db == nullptr) return;

	sqlite3_close(db);
	db = nullptr;
}

bool DatabaseManager::ExecuteQuery(const std::string& query)
{
	char* errorMsg = nullptr;
	int status = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errorMsg);
	if (status != SQLITE_OK)
	{
		std::cerr << "SQL error: " << errorMsg << std::endl;
		sqlite3_free(errorMsg);
		return false;
	}

	return true;
}
