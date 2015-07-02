#include "picotorrent.h"

CPicoTorrent::CPicoTorrent(HINSTANCE hInstance)
{
    module_.Init(NULL, hInstance);
    module_.AddMessageLoop(&loop_);

    // Set up main application window
    window_.CreateEx();

    libtorrent::fingerprint fp("LT", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0);
    session_ = std::make_unique<libtorrent::session>(fp, 0);
    session_->set_alert_mask(libtorrent::alert::category_t::all_categories);
    session_->set_alert_dispatch(std::bind(&CPicoTorrent::OnSessionAlert, this, std::placeholders::_1));
    session_->listen_on(std::make_pair(6881, 6889));
}

CPicoTorrent::~CPicoTorrent()
{
    module_.RemoveMessageLoop();
    module_.Term();
}

int CPicoTorrent::Run(LPWSTR lpCmdLine, int nCmdShow)
{
    window_.ShowWindow(nCmdShow);
    return loop_.Run();
}

void CPicoTorrent::OnSessionAlert(std::auto_ptr<libtorrent::alert> alert)
{
    PostMessage(window_, WM_APP + 0x01, NULL, (LPARAM)alert.release());
}
