#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <iostream>
#include <string>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for (i = 0; i < argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int createdatabase()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int  rc;
	char *sql;

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if (rc){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}
	else{
		fprintf(stdout, "Opened database successfully\n");
	}

	// TODO: ask to create TABLE

	/* Create SQL statement */
	sql = "CREATE TABLE COMPANY("  \
		"ID INT PRIMARY KEY     NOT NULL," \
		"NAME           TEXT    NOT NULL," \
		"AGE            INT     NOT NULL," \
		"ADDRESS        CHAR(50)," \
		"SALARY         REAL );";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else{
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db);
	return 0;
}

int main(int argc, char* argv[])
{
	bool exiting = false;
	while (!exiting)
	{
		std::string selection;
		int selectionInt;
		std::cout << "1: Create database\n2: Insert into created Database\n3: Select from database\n4: Update Operation\n5: Delete Operation\n6: Exit" << std::endl;
		std::cout << "Select Task:";
		std::cin >> selection;

		// TODO: continue here
		for (auto  : selection)
		{

		}
		
		selectionInt = std::stoi(selection);
		
		switch (selectionInt)
		{
		case 1:
		{
			createdatabase();
			break;
		}
		case 6:
		{
			char yes;
			std::cout << "do you want to exit y/n";
			std::cin >> yes;
			if (yes == 'y' | yes == 'Y')
			{
				exiting = true;
			}
			break;
		}
		default:
		{
			std::cout << "Not a correct selection" << std::endl;
			break;
		}
		}
	}
}