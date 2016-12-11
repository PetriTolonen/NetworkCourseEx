#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <iostream>
#include <string>
#include <sstream>

namespace
{
	int currentPlayer;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;

	for (i = 0; i < argc; i++) {
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
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}
	else {
		fprintf(stdout, "Opened database successfully\n");
	}

	/* Create SQL statement */
	sql = "CREATE TABLE PLAYERS("  \
		"ID INT PRIMARY KEY     NOT NULL AUTOINCREMENT UNIQUE," \
		"NAME           TEXT    NOT NULL," \
		"SCORE			INT);";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db);
	return 0;
}

void createNewPlayer()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	const char *sql;
	std::string playerName;
	std::string inputPlayer;

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}

	std::cout << "Give player name" << std::endl;
	std::cin >> playerName;

	inputPlayer = "INSERT INTO PLAYERS (NAME,SCORE) "  \
		"VALUES ('" + playerName + "', 0 ); " \
		;

	/* Create SQL statement */
	sql = inputPlayer.c_str();

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Records created successfully\n");
	}
	sqlite3_close(db);
	return;
}

void selectPlayer()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	const char *sql;
	std::string playerName;
	std::string selectPlayer;

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}

	std::cout << "Give player name to select" << std::endl;
	std::cin >> playerName;

	selectPlayer = "SELECT * FROM PLAYERS WHERE NAME = '" + playerName + "'"  \
		;
	/* Create SQL statement */
	sql = selectPlayer.c_str();

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Player selection complete\n");
	}
	sqlite3_close(db);
	return;
}

void updateScore()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	const char *sql;
	std::string playerId;
	std::string selectPlayer;
	std::string score;

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}	

	std::cout << "Give player ID to select" << std::endl;
	std::cin >> playerId;

	std::cout << "Give score for player" << std::endl;
	std::cin >> score;

	selectPlayer = "UPDATE PLAYERS set SCORE = " + score + " WHERE ID = '" + playerId + "'"  \
		;
	/* Create SQL statement */
	sql = selectPlayer.c_str();

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Player updated successfully\n");
	}
	sqlite3_close(db);
	return;
}

void high10()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	const char *sql;
	std::string selectPlayer;

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}

	selectPlayer = "SELECT * FROM PLAYERS ORDER BY SCORE DESC LIMIT 10"  \
		;
	/* Create SQL statement */
	sql = selectPlayer.c_str();

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Top 10 successfully\n");
	}
	sqlite3_close(db);
	return;
}

void deletePlayer()
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	const char *sql;
	const char* data = "Callback function called";
	std::string playerId;
	std::string selectPlayer;

	/* Open database */
	rc = sqlite3_open("test.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}

	std::cout << "Give player ID to select" << std::endl;
	std::cin >> playerId;

	/* Create merged SQL statement */
	selectPlayer = "DELETE from PLAYERS where ID=" + playerId;

	/* Create SQL statement */
	sql = selectPlayer.c_str();

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Operation done successfully\n");
	}
	sqlite3_close(db);
}

int main(int argc, char* argv[])
{
	bool exiting = false;
	while (!exiting)
	{
		char* selection;
		int selectionInt;
		std::string catchThis;
		std::cout << "1: Create database\n2: Create new player into created Database\n3: Select from database\n4: Update Score Operation\n5: Delete Operation\n6: HighScoreTop10\n7: Exit" << std::endl;
		std::cout << "Select Task:";
		std::cin >> catchThis;
		std::istringstream ss(catchThis);
		ss >> selectionInt;

		system("cls");

		switch (selectionInt)
		{
		case 1:
		{
			createdatabase();
			break;
		}
		case 2:
		{
			createNewPlayer();
			break;
		}
		case 3:
		{
			selectPlayer();
			break;
		}
		case 4:
		{
			updateScore();
			break;
		}
		case 5:
		{
			deletePlayer();
			break;
		}
		case 6:
		{
			high10();
			break;
		}
		case 7:
		{
			fflush(stdin);
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
			std::cin.ignore();
			break;
		}
		}
	}
}