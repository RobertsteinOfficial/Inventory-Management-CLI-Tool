#include "InventoryManager.h"

#include <iomanip>

InventoryManager::InventoryManager(DatabaseManager& dbManager) : dbManager(dbManager) {}

InventoryManager::~InventoryManager() {}

void InventoryManager::AddItem()
{
	std::string name;

	int quantity;
	double price;

	std::cout << "Enter item name: ";
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

	sqlite3* db = dbManager.GetDatabase();

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

void InventoryManager::ListItems()
{
	sqlite3* db = dbManager.GetDatabase();

	const char* selectSQL = "SELECT id, name, quantity, price FROM inventory";
	sqlite3_stmt* statement;

	int status = sqlite3_prepare_v2(db, selectSQL, -1, &statement, nullptr);

	if (status != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return;
	}


	std::cout << "\n--- Inventory Items ---\n";
	std::cout << std::left << std::setw(5) << "ID"
		<< std::setw(20) << "Name"
		<< std::setw(10) << "Quantity"
		<< std::setw(10) << "Price" << "\n";
	std::cout << "--------------------------\n";

	bool firstItem = true;
	while ((status = sqlite3_step(statement)) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(statement, 0);
		const char* name = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
		int quantity = sqlite3_column_int(statement, 2);
		double price = sqlite3_column_double(statement, 3);

		std::cout << std::left << std::setw(5) << id
			<< std::setw(20) << name
			<< std::setw(10) << quantity
			<< std::setw(10) << std::fixed << std::setprecision(2) << price << "\n";
		firstItem = false;
	}

	if (firstItem)
	{
		std::cout << "No items found in the inventory.\n";
	}
	else if (status != SQLITE_DONE)
	{
		std::cerr << "Error listing items: " << sqlite3_errmsg(db) << std::endl;
	}

	sqlite3_finalize(statement);
}
