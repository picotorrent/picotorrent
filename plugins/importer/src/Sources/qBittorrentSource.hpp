#pragma once

#include <memory>

#include "Source.hpp"

namespace Sources
{
	class qBittorrentSource : public Source
	{
	public:
		std::wstring GetName() { return L"qBittorrent"; }
		std::vector<AddTorrentRequest> GetRequests();
		HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent);

	private:
        static INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

        HWND m_hWnd;
	};
}
