#pragma once
#include "stdafx.h"

struct PluginSettings
{
	Settings *set;
	HINSTANCE _hInst;
	NppData nppData;
	string path;
	string insert_type;
	sqlite3 *db;
};

extern PluginSettings pluginsettings;

void initSettingsDB();
void initSetting();

void writeDesc();
void settingsDlg();
void insert_quotes();
void insert_description();
void createBDdialog();

void showTip(SCNotification *notifyCode);

wstring createdbfile(wstring path, char delimeter);