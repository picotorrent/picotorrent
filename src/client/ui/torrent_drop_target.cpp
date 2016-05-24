#include <picotorrent/client/ui/torrent_drop_target.hpp>

#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/is_valid_torrent_file.hpp>

#include <shellapi.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <algorithm>
#include <vector>

using picotorrent::client::ui::torrent_drop_target;
using picotorrent::common::to_string;
using picotorrent::core::is_valid_torrent_file;
using picotorrent::core::signals::signal;
using picotorrent::core::signals::signal_connector;

class PicoDropTarget : public IDropTarget
{
public:
    PicoDropTarget(signal<void, const std::vector<std::string>&> &dropped)
        : on_dropped_(dropped)
    {
    }

    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
#pragma warning( push )
#pragma warning( disable : 4838 )
        static const QITAB qit[] = {
            QITABENT(PicoDropTarget, IDropTarget),
            { 0 }
        };
#pragma warning( pop )

        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&ref_);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long ref = InterlockedDecrement(&ref_);

        if (!ref)
        {
            delete this;
        }

        return ref;
    }

    IFACEMETHODIMP_(HRESULT) DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    {
        effect = DROPEFFECT_COPY;
        paths_ = GetPaths(pDataObject);

        if (!IsValidTorrents(paths_))
        {
            effect = DROPEFFECT_NONE;
        }

        *pdwEffect = effect;
        return S_OK;
    }

    IFACEMETHODIMP_(HRESULT) DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    {
         *pdwEffect = effect;
        return S_OK;
    }

    IFACEMETHODIMP_(HRESULT) DragLeave()
    {
        return S_OK;
    }

    IFACEMETHODIMP_(HRESULT) Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    {
        on_dropped_.emit(paths_);
        return S_OK;
    }

private:
    static std::vector<std::string> GetPaths(IDataObject *pDataObject)
    {
        FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg;
        pDataObject->GetData(&fmtetc, &stg);

        HDROP drop = static_cast<HDROP>(stg.hGlobal);
        int files = DragQueryFile(drop, -1, 0, 0);
        std::vector<std::string> paths;

        for (int i = 0; i < files; i++)
        {
            TCHAR path[MAX_PATH];
            DragQueryFile(drop, i, path, ARRAYSIZE(path));
            paths.push_back(to_string(path));
        }

        return paths;
    }

    static bool IsValidTorrents(const std::vector<std::string> &paths)
    {
        return std::all_of(paths.begin(), paths.end(), [](const std::string &p) { return is_valid_torrent_file(p); });
    }

    DWORD effect = DROPEFFECT_NONE;
    std::vector<std::string> paths_;
    long ref_;
    signal<void, const std::vector<std::string>&>& on_dropped_;
};

torrent_drop_target::torrent_drop_target(HWND hParent)
    : parent_(hParent)
{
    OleInitialize(NULL);
    RegisterDragDrop(parent_, new PicoDropTarget(on_torrents_dropped_));
}

torrent_drop_target::~torrent_drop_target()
{
    RevokeDragDrop(parent_);
}

signal_connector<void, const std::vector<std::string>&>& torrent_drop_target::on_torrents_dropped()
{
    return on_torrents_dropped_;
}
