#pragma once

#include <string>
#include <sqlite3.h>

class DatabaseManager
{
public:

	DatabaseManager();
	~DatabaseManager();

	bool InitializeDatabase();
	bool OpenDatabase(const std::string& dbName);
	void CloseDatabase();

	bool ExecuteQuery(const std::string& query);
	sqlite3* GetDatabase() const { return db; }

private:
	sqlite3* db;
};