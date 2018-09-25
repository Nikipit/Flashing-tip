#define SETTINGS_DB_STATUS_OK 0;

#include <string>

#include "sqlite3.h"

using namespace std;

class Settings
{
private:
	const string settings_table_name = "PlUGIN_SETTINGS_TABLE";
	sqlite3 * settingsDB;
	string result;
	int bd_stat;
	string error;

	static int selectCallback(void *result, int argc, char **argv, char **azColName)
	{
		string *input = static_cast<string*>(result);

		for (int i = 0; i<argc; i++)
		{
			*input = string(argv[i]);
		}
		return 0;
	}

public:
	Settings(string file)
	{
		char *ErrMsg = 0;
		string create_settings_table = "CREATE TABLE IF NOT EXISTS " + settings_table_name + " (KEY TEXT PRIMARY KEY NOT NULL, VALUE TEXT NOT NULL);";
		
		bd_stat = sqlite3_open(file.c_str(), &settingsDB);
		
		bd_stat = sqlite3_exec(settingsDB, create_settings_table.c_str(), NULL, 0, &ErrMsg);

		if (bd_stat != SQLITE_OK)
		{
			error = string(ErrMsg);
			sqlite3_free(ErrMsg);
		}
	}

	void set(string key, string value)
	{
		char *ErrMsg = 0;
		string addQuery = "INSERT OR REPLACE INTO " + settings_table_name + " (KEY, VALUE) VALUES ('" + key + "', '" + value + "')";

		bd_stat = sqlite3_exec(settingsDB, addQuery.c_str(), NULL, 0, &ErrMsg);
		if (bd_stat != SQLITE_OK)
		{
			error = string(ErrMsg);
			sqlite3_free(ErrMsg);
		}
	}

	string get(string key)
	{
		char *ErrMsg = 0;
		string getQuery = "SELECT VALUE FROM " + settings_table_name + " WHERE KEY = '" + key + "';";
		string result;
		void *presult = &result;

		bd_stat = sqlite3_exec(settingsDB, getQuery.c_str(), selectCallback, presult, &ErrMsg);
		if (bd_stat)
		{
			error = string(ErrMsg);
			sqlite3_free(ErrMsg);
			return "ERROR";
		}
		return result;
	}

	void close()
	{
		sqlite3_close(settingsDB);
	}

	int getStat()
	{
		return bd_stat;
	}

	string getError()
	{
		return error;
	}
};