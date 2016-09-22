#pragma once

#include "Source.hpp"

namespace Sources
{
	class qBittorrentSource : public Source
	{
	public:
		std::wstring GetName() { return L"qBittorrent"; }
		std::vector<std::wstring> GetPreview();
		HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent);

	private:
		static INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
	};
}
