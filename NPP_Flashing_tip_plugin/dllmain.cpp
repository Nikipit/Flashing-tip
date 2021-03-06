#include "stdafx.h"
#include "Plugin.h"

PluginSettings pluginsettings;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID /*lpReserved*/)
{
	if (reasonForCall = DLL_PROCESS_ATTACH)
	{
		pluginsettings._hInst = (HINSTANCE)hModule;

	}

	return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	pluginsettings.nppData = notpadPlusData;
	initSettingsDB();
	initSetting();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return TEXT("Flasing Tip");
}

const int nbFunc = 5;
FuncItem funcItem[nbFunc];

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{

	

	*nbF = nbFunc;

	ShortcutKey *insert_quotes_sck = new ShortcutKey{ false, true, false, 0x51 };
	ShortcutKey *insert_desc_sck = new ShortcutKey{ false, true, false, 0x5A };

	lstrcpy(funcItem[0]._itemName, TEXT("Insert description"));
	funcItem[0]._pFunc = insert_description;
	funcItem[0]._init2Check = false;
	funcItem[0]._pShKey = insert_desc_sck;

	lstrcpy(funcItem[1]._itemName, TEXT("Insert quotes"));
	funcItem[1]._pFunc = insert_quotes;
	funcItem[1]._init2Check = false;
	funcItem[1]._pShKey = insert_quotes_sck;

	lstrcpy(funcItem[2]._itemName, TEXT("Create DB"));
	funcItem[2]._pFunc = createBDdialog;
	funcItem[2]._init2Check = false;
	funcItem[2]._pShKey = NULL;

	lstrcpy(funcItem[3]._itemName, TEXT("Settings"));
	funcItem[3]._pFunc = settingsDlg;
	funcItem[3]._init2Check = false;
	funcItem[3]._pShKey = NULL;

	lstrcpy(funcItem[4]._itemName, TEXT("About"));
	funcItem[4]._pFunc = writeDesc;
	funcItem[4]._init2Check = false;
	funcItem[4]._pShKey = NULL;

	return funcItem;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	switch (notifyCode->nmhdr.code)
	{
	case NPPN_SHUTDOWN:
		break;

	case SCN_DOUBLECLICK:
		showTip(notifyCode);

	default:
		return;
	}
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}


#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}
#endif