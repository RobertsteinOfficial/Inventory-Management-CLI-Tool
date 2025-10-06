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

void InventoryManager::SearchItem()
{
	int choice;

	do
	{
		std::cout << "\nBy what criteria do you want to search?\n";
		std::cout << "1. By ID\n";
		std::cout << "2. By Name\n";

		std::cin >> choice;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input. Enter a valid number.\n";
			continue;
		}

		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (choice)
		{
		case 1:
			SearchItemById();
			break;
		case 2:
			SearchItemByName();
			break;
		default:
			std::cout << "Invalid input.\n";
			break;
		}

	} while (choice != 1 && choice != 2);

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

void InventoryManager::RemoveItem()
{
	int id;
	std::cout << "Enter the ID of the item to remove: ";

	while (!(std::cin >> id))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input. Please enter a valid integer ID: ";
	}
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	if (!CheckItem(id)) return;

	sqlite3* db = dbManager.GetDatabase();
	const char* deleteSQL = "DELETE FROM inventory WHERE id = ?";
	sqlite3_stmt* deleteStatement;

	int status = sqlite3_prepare_v2(db, deleteSQL, -1, &deleteStatement, nullptr);
	if (status != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	sqlite3_bind_int(deleteStatement, 1, id);

	status = sqlite3_step(deleteStatement);
	if (status != SQLITE_DONE)
	{
		std::cerr << "Failed to remove item: " << sqlite3_errmsg(db) << std::endl;
	}
	else
	{
		std::cout << "Item removed successfully.\n";
	}


	sqlite3_finalize(deleteStatement);
}

bool InventoryManager::CheckItem(int id)
{
	sqlite3* db = dbManager.GetDatabase();

	const char* checkSQL = "SELECT id FROM inventory WHERE id = ?";
	sqlite3_stmt* checkStatement;

	int status = sqlite3_prepare_v2(db, checkSQL, -1, &checkStatement, nullptr);
	if (status != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	sqlite3_bind_int(checkStatement, 1, id);
	status = sqlite3_step(checkStatement);
	if (status == SQLITE_DONE)
	{
		std::cout << "No item found with ID " << id << ". Nothing to remove.\n";
		sqlite3_finalize(checkStatement);
		return false;
	}
	else if (status != SQLITE_ROW)
	{
		std::cerr << "Error checking item: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(checkStatement);
		return false;
	}

	sqlite3_finalize(checkStatement);
	return true;
}

void InventoryManager::SearchItemById()
{
	int searchId;

	std::cout << "Enter item ID:\n";
	while (!(std::cin >> searchId) || searchId < 0)
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input. Enter a positive integer ID: ";
	}

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	sqlite3* db = dbManager.GetDatabase();
	const char* searchSQL = "SELECT * FROM inventory WHERE id = ?";
	sqlite3_stmt* searchStatement;


	int status = sqlite3_prepare_v2(db, searchSQL, -1, &searchStatement, nullptr);
	if (status != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	sqlite3_bind_int(searchStatement, 1, searchId);

	status = sqlite3_step(searchStatement);
	if (status != SQLITE_ROW)
	{
		std::cerr << "Failed to find item: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(searchStatement);
		return;
	}


	std::cout << "\n--- Inventory Items ---\n";
	std::cout << std::left << std::setw(5) << "ID"
		<< std::setw(20) << "Name"
		<< std::setw(10) << "Quantity"
		<< std::setw(10) << "Price" << "\n";
	std::cout << "--------------------------\n";


	int id = sqlite3_column_int(searchStatement, 0);
	const char* name = reinterpret_cast<const char*>(sqlite3_column_text(searchStatement, 1));
	int quantity = sqlite3_column_int(searchStatement, 2);
	double price = sqlite3_column_double(searchStatement, 3);

	std::cout << std::left << std::setw(5) << id
		<< std::setw(20) << name
		<< std::setw(10) << quantity
		<< std::setw(10) << std::fixed << std::setprecision(2) << price << "\n";


	sqlite3_finalize(searchStatement);
}

void InventoryManager::SearchItemByName()
{
	std::string name;

	std::cout << "Enter item name:\n";
	std::getline(std::cin, name);

	sqlite3* db = dbManager.GetDatabase();
	const char* searchSQL = "SELECT * FROM inventory WHERE LOWER(name) LIKE LOWER(?)";
	std::string searchTerm = "%" + name + "%";

	sqlite3_stmt* searchStatement;

	int status = sqlite3_prepare_v2(db, searchSQL, -1, &searchStatement, nullptr);

	if (status != SQLITE_OK)
	{
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	sqlite3_bind_text(searchStatement, 1, name.c_str(), -1, SQLITE_STATIC);

	std::cout << "\n--- Inventory Items ---\n";
	std::cout << std::left << std::setw(5) << "ID"
		<< std::setw(20) << "Name"
		<< std::setw(10) << "Quantity"
		<< std::setw(10) << "Price" << "\n";
	std::cout << "--------------------------\n";

	bool firstItem = true;
	while ((status = sqlite3_step(searchStatement) == SQLITE_ROW))
	{

		int id = sqlite3_column_int(searchStatement, 0);
		const char* name = reinterpret_cast<const char*>(sqlite3_column_text(searchStatement, 1));
		int quantity = sqlite3_column_int(searchStatement, 2);
		double price = sqlite3_column_double(searchStatement, 3);

		std::cout << std::left << std::setw(5) << id
			<< std::setw(20) << name
			<< std::setw(10) << quantity
			<< std::setw(10) << std::fixed << std::setprecision(2) << price << "\n";
		firstItem = false;
	}

	if (firstItem)
	{
		std::cout << "No items with name " + name + " found in the inventory.\n";
	}
	else if (status != SQLITE_DONE)
	{
		std::cerr << "Error listing items: " << sqlite3_errmsg(db) << std::endl;
	}

	sqlite3_finalize(searchStatement);

}
