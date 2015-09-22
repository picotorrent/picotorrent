#pragma once

#include <memory>
#include "stdafx.h"

namespace pico
{
    class TorrentListView;

    class MainWindow
    {
    public:
        MainWindow();
        ~MainWindow();

        void Create();

    private:
        LRESULT CALLBACK WndProc(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
        static LRESULT CALLBACK WndProcProxy(_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);

        HWND hWnd_;
        std::unique_ptr<TorrentListView> torrentsView_;
    };
}
