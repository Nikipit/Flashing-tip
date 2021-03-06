#include "stdafx.h"
#include "Plugin.h"
#include <sstream>
#include <list>
#include <fstream>


string getSelString()
{
	int which = -1;
	::SendMessage(pluginsettings.nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
		return NULL;
	HWND curScintilla = (which == 0) ? pluginsettings.nppData._scintillaMainHandle : pluginsettings.nppData._scintillaSecondHandle;

	size_t start = ::SendMessage(curScintilla, SCI_GETSELECTIONSTART, 0, 0);
	size_t end = ::SendMessage(curScintilla, SCI_GETSELECTIONEND, 0, 0);

	if (end < start)
	{
		size_t tmp = start;
		start = end;
		end = tmp;
	}

	size_t textLen = end - start;

	if (textLen != 0)
	{
		char *_str;
		_str = new char[textLen + 1];
		::SendMessageW(curScintilla, SCI_GETSELTEXT, 0, (LPARAM)_str);
		string result = string(_str);
		delete[] _str;
		return result;
	}
	return "";
}

static int callback_description(void *data, int argc, char **argv, char **azColName) {

	*(string*)data = argv[0];
	return 0;
}

string get_description(string _str)
{
	string sql_select_exec = "SELECT VALUE from CATALOG WHERE UPPER(KEY) = UPPER('" + _str + "') AND UPPER(TYPE) = UPPER('" + pluginsettings.insert_type + "') ORDER BY TYPE DESC";
	string description = "";
	void *desc = &description;
	char *err = 0;
	sqlite3_exec(pluginsettings.db, sql_select_exec.c_str(), callback_description, desc, &err);

	return description;
}

void insert_description()
{
	int which = -1;
	::SendMessage(pluginsettings.nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
		return;
	HWND curScintilla = (which == 0) ? pluginsettings.nppData._scintillaMainHandle : pluginsettings.nppData._scintillaSecondHandle;

	int end = ::SendMessage(curScintilla, SCI_GETSELECTIONEND, 0, 0);
	char end_s = ::SendMessage(curScintilla, SCI_GETCHARAT, end, 0);

	if (end_s == '\"')
	{
		end++;
	}

	string selText = getSelString();
	string desc = get_description(selText);

	if (!desc.compare(""))
	{
		return;
	}

	desc = "/* " + desc + " */";
	::SendMessage(curScintilla, SCI_INSERTTEXT, end, (LPARAM)desc.c_str());
}

struct row
{
	string key;
	string type;
	string value;
};

static int callback_tip(void *data, int argc, char **argv, char **azColName) {

	row curr_row;

	curr_row.key = argv[1];
	curr_row.type = argv[2];
	curr_row.value = argv[3];

	std::list<row> *x = (std::list<row>*)data;

	x->push_back(curr_row);

	return 0;
}

string getTip(string _str)
{
	string sql_select_exec = "SELECT * from CATALOG WHERE UPPER(KEY) = UPPER('" + _str + "') ORDER BY TYPE DESC";

	std::list<row> rows;
	void *rws = &rows;

	char *err = 0;
	sqlite3_exec(pluginsettings.db, sql_select_exec.c_str(), callback_tip, rws, &err);

	string lasttype;
	bool isfirst = true;
	stringstream tip;
	for (row r : rows)
	{

		if (isfirst) isfirst = false; else tip << "\n";

		if (r.type != lasttype)
		{
			tip << r.type << ":\n";
			lasttype = r.type;
		}

		tip << "  " << r.value;
	}

	return tip.str();

}

void showTip(SCNotification *notifyCode)
{
	string selText = getSelString();
	string tip = getTip(selText);
	int which = -1;
	::SendMessage(pluginsettings.nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	HWND curScintilla = (which == 0) ? pluginsettings.nppData._scintillaMainHandle : pluginsettings.nppData._scintillaSecondHandle;
	::SendMessageW(curScintilla, SCI_CALLTIPSHOW, notifyCode->position, (LPARAM)tip.c_str());
}

void insert_quotes()
{
	int which = -1;
	::SendMessage(pluginsettings.nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
		return;
	HWND curScintilla = (which == 0) ? pluginsettings.nppData._scintillaMainHandle : pluginsettings.nppData._scintillaSecondHandle;

	int start = ::SendMessage(curScintilla, SCI_GETSELECTIONSTART, 0, 0);
	int end = ::SendMessage(curScintilla, SCI_GETSELECTIONEND, 0, 0);

	::SendMessage(curScintilla, SCI_INSERTTEXT, start, (LPARAM)"\"");
	::SendMessage(curScintilla, SCI_INSERTTEXT, end + 1, (LPARAM)"\"");
}

void initSettingsDB()
{
	TCHAR confDir[MAX_PATH];
	::SendMessage(pluginsettings.nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)confDir);
	wstring SetPath = wstring(confDir) + L"Flashing_tip_settings.db";
	pluginsettings.set = new Settings(utf8_encode(SetPath));
}

void initSetting()
{
	sqlite3_close(pluginsettings.db);
	pluginsettings.insert_type = pluginsettings.set->get("INSERT_TYPE");
	pluginsettings.path = pluginsettings.set->get("PATH");
	if (sqlite3_open(pluginsettings.path.c_str(), &pluginsettings.db))
	{
		MessageBoxA(NULL, (LPSTR)(sqlite3_errmsg(pluginsettings.db)), (LPSTR)("Ошибка открытия бд"), MB_OK);
	}
}

wstring createdbfile(wstring path, char delimeter)
{
	std::ifstream in(path);
	
	if (in.is_open())
	{
		string DB_file = utf8_encode(path) + "db";
		char *ErrMsg = 0;

		sqlite3 *DB;
		int db_stat = sqlite3_open(DB_file.c_str(), &DB);

		string drop = "DROP TABLE IF EXISTS CATALOG";
		db_stat = sqlite3_exec(DB, drop.c_str(), NULL, 0, &ErrMsg);

		string create = "CREATE TABLE IF NOT EXISTS CATALOG(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, KEY TEXT NOT NULL, TYPE TEXT NOT NULL, VALUE TEXT NOT NULL)";
		db_stat = sqlite3_exec(DB, create.c_str(), NULL, 0, &ErrMsg);

		string add;

		string substring;
		string key;
		string type;
		string value;
		int x;

		sqlite3_exec(DB, "BEGIN TRANSACTION", NULL, NULL, &ErrMsg);

		while (in.good())
		{
			getline(in, substring, '\n');

			key = "";
			type = "";
			value = "";
			x = 0;

			for (char c : substring)
			{
				if (c == '|')
				{
					x++;
				}
				else
				{
					if (x == 0) key.push_back(c);
					if (x == 1) type.push_back(c);
					if (x == 2) value.push_back(c);
				}
			}

			if (key != "" && type != "" && value != "")
				add = "INSERT INTO CATALOG(KEY,TYPE,VALUE) VALUES (\"" + key + "\", \"" + type + "\", \"" + value + "\")";
			else
				add = "";
			
			if (add != "")
				db_stat = sqlite3_exec(DB, add.c_str(), NULL, 0, &ErrMsg);
		}
		sqlite3_exec(DB, "END TRANSACTION", NULL, NULL, &ErrMsg);
		sqlite3_close(DB);
		in.close();
		return utf8_decode(DB_file);
	}
	in.close();
	return wstring(L"Error open file");
}