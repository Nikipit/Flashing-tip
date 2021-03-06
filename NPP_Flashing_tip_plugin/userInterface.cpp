#include "stdafx.h"
#include "Plugin.h"
#include "resource.h"
#include <string>
#include <sstream>

void writeDesc()
{
	// открываем новую вкладку (документ)
	::SendMessage(pluginsettings.nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

	// получаем текущую сцинтиллу
	int which = -1;
	::SendMessage(pluginsettings.nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
		return;
	HWND curScintilla = (which == 0) ? pluginsettings.nppData._scintillaMainHandle : pluginsettings.nppData._scintillaSecondHandle;

	//выводим в сциллу текст
	::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)"Project on GitHub:\n\thttps://github.com/Nikipit/Flashing-tip\n\nAuthor:\n\tTipikin Aleksandr\n\tA.Tipikin@outlook.com");
}

INT_PTR CALLBACK SetingsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_FILE_PATH, (LPCWSTR)utf8_decode(pluginsettings.set->get("PATH")).c_str());
		SetDlgItemText(hDlg, IDC_INSERT_TYPE_BOX, (LPCWSTR)utf8_decode(pluginsettings.set->get("INSERT_TYPE")).c_str());
		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDOK:
			TCHAR text[_MAX_PATH + _MAX_FNAME];
			GetDlgItemText(hDlg, IDC_FILE_PATH, text, _MAX_PATH + _MAX_FNAME);
			pluginsettings.set->set("PATH", utf8_encode(text));

			GetDlgItemText(hDlg, IDC_INSERT_TYPE_BOX, text, 128);
			pluginsettings.set->set("INSERT_TYPE", utf8_encode(wstring(text)));

			EndDialog(hDlg, LOWORD(wParam));
			initSetting();
			return (INT_PTR)TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;

		case IDC_FILE_BUTTON:

			OPENFILENAME ofn;
			wchar_t szFile[_MAX_PATH + _MAX_FNAME];
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"All Files\0*.*\0\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				SetDlgItemText(hDlg, IDC_FILE_PATH, ofn.lpstrFile);
			}
		}
	}

	return (INT_PTR)FALSE;
}

void settingsDlg()
{
	HWND hSelf = CreateDialogParam(pluginsettings._hInst, MAKEINTRESOURCE(IDD_SETTINGS_DIALOG), pluginsettings.nppData._nppHandle, (DLGPROC)SetingsProc, (LPARAM)NULL);
	EnableWindow(pluginsettings.nppData._nppHandle, false);
	ShowWindow(hSelf, SW_SHOW);
}

INT_PTR CALLBACK CreateDPProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			case IDOK:
			TCHAR file[_MAX_PATH + _MAX_FNAME];
			GetDlgItemText(hDlg, IDC_FILE_PATH, file, _MAX_PATH + _MAX_FNAME);
			MessageBoxW(hDlg, (LPWSTR)createdbfile(file, ',').c_str(), (LPWSTR)L"Done", MB_OK);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;

		case IDC_FILE_BUTTON:

			OPENFILENAME ofn;
			wchar_t szFile[_MAX_PATH + _MAX_FNAME];
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"All Files\0*.*\0\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) == TRUE)
			{
				SetDlgItemText(hDlg, IDC_FILE_PATH, ofn.lpstrFile);
			}
		}
	}

	return (INT_PTR)FALSE;
}

void createBDdialog()
{
	HWND hSelf = CreateDialogParam(pluginsettings._hInst, MAKEINTRESOURCE(IDD_CREATE_DIALOG), pluginsettings.nppData._nppHandle, (DLGPROC)CreateDPProc, (LPARAM)NULL);
	EnableWindow(pluginsettings.nppData._nppHandle, false);
	ShowWindow(hSelf, SW_SHOW);
}


