#pragma once

#include <memory>

#include "Source.hpp"

class IFileSystem;

namespace Sources
{
	class qBittorrentSource : public Source
	{
	public:
        qBittorrentSource(std::shared_ptr<IFileSystem> fileSystem);

		std::wstring GetName() { return L"qBittorrent"; }
		std::vector<PreviewItem> GetPreview();
		HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent);
        void Import();

	private:
        HWND m_hWnd;
        std::shared_ptr<IFileSystem> m_fileSystem;
		static INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
	};
}
