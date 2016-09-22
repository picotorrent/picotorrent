#include "qBittorrentSource.hpp"

#include "../resources.h"

using Sources::qBittorrentSource;

std::vector<std::wstring> qBittorrentSource::GetPreview()
{
	return std::vector<std::wstring>();
}

HWND qBittorrentSource::GetWindowHandle(HINSTANCE hInstance, HWND hWndParent)
{
	return CreateDialogParam(
		hInstance,
		MAKEINTRESOURCE(IDD_CONFIG_QBITTORRENT),
		hWndParent,
		&qBittorrentSource::DialogProc,
		reinterpret_cast<LPARAM>(this));
}

INT_PTR qBittorrentSource::DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return TRUE;
	}

	return FALSE;
}
