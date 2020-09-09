#include "createtorrentdialog.hpp"

#include <filesystem>

#include <fmt/format.h>
#include <libtorrent/create_torrent.hpp>
#include <wx/hyperlink.h>
#include <wx/tokenzr.h>

#include "../../buildinfo.hpp"
#include "../../core/utils.hpp"
#include "../translator.hpp"

wxDEFINE_EVENT(ptEVT_CREATE_TORRENT_THREAD_START, wxThreadEvent);
wxDEFINE_EVENT(ptEVT_CREATE_TORRENT_THREAD_STOP, wxThreadEvent);
wxDEFINE_EVENT(ptEVT_CREATE_TORRENT_THREAD_PROGRESS, wxThreadEvent);

namespace fs = std::filesystem;
namespace lt = libtorrent;
using pt::UI::Dialogs::CreateTorrentDialog;

struct CreateTorrentDialog::CreateTorrentParams
{
    std::string comment;
    std::string creator;
    fs::path path;
    std::vector<std::string> trackers;
    std::vector<std::string> url_seeds;
    bool priv;
};

struct ProgressPayload
{
    int totalPieces;
    int currentPiece;
};

struct StopPayload
{
    lt::entry e;
};

CreateTorrentDialog::CreateTorrentDialog(wxWindow* parent, wxWindowID id)
    : wxDialog(parent, id, i18n("create_torrent"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    auto pathSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("files"));
    m_numFiles = new wxStaticText(pathSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_path = new wxTextCtrl(pathSizer->GetStaticBox(), wxID_ANY);
    m_selectFile = new wxButton(pathSizer->GetStaticBox(), ptID_BTN_BROWSE_FILE, i18n("select_file"));
    m_selectDir = new wxButton(pathSizer->GetStaticBox(), ptID_BTN_BROWSE_DIR, i18n("select_directory"));

    auto selectSizer = new wxBoxSizer(wxHORIZONTAL);
    selectSizer->Add(m_numFiles, 0, wxALIGN_CENTER_VERTICAL);
    selectSizer->AddStretchSpacer();
    selectSizer->Add(m_selectFile);
    selectSizer->AddSpacer(FromDIP(7));
    selectSizer->Add(m_selectDir);

    pathSizer->Add(m_path, 1, wxEXPAND | wxALL, FromDIP(3));
    pathSizer->Add(selectSizer, 0, wxEXPAND | wxALL, FromDIP(3));

    auto optionsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, i18n("options"));
    m_mode = new wxChoice(optionsSizer->GetStaticBox(), wxID_ANY);
    m_mode->Append({ i18n("mode_v1"), i18n("mode_hybrid"), i18n("mode_v2") });
    m_mode->Select(1);

    m_private = new wxCheckBox(optionsSizer->GetStaticBox(), wxID_ANY, i18n("private"));
    m_comment = new wxTextCtrl(optionsSizer->GetStaticBox(), wxID_ANY, wxEmptyString);
    m_creator = new wxTextCtrl(optionsSizer->GetStaticBox(), wxID_ANY, fmt::format("PicoTorrent {0}", pt::BuildInfo::version()));

    auto optionsGrid = new wxFlexGridSizer(2, FromDIP(7), FromDIP(25));
    optionsGrid->AddGrowableCol(1, 1);
    optionsGrid->Add(new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, i18n("mode")), 0, wxALL, FromDIP(3));
    optionsGrid->Add(m_mode, 1, wxEXPAND | wxALL, FromDIP(3));
    optionsGrid->Add(new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, i18n("comment")), 0, wxALL, FromDIP(3));
    optionsGrid->Add(m_comment, 1, wxEXPAND | wxALL, FromDIP(3));
    optionsGrid->Add(new wxStaticText(optionsSizer->GetStaticBox(), wxID_ANY, i18n("creator")), 0, wxALL, FromDIP(3));
    optionsGrid->Add(m_creator, 1, wxEXPAND | wxALL, FromDIP(3));
    optionsGrid->AddStretchSpacer(0);
    optionsGrid->Add(m_private, 1, wxALL, FromDIP(3));

    optionsSizer->Add(optionsGrid, 1, wxEXPAND);

    // Trackers
    auto trackersSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("trackers_input_per_line"));
    m_trackers = new wxTextCtrl(trackersSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxTE_MULTILINE);
    trackersSizer->Add(m_trackers, 1, wxEXPAND);

    // URL seeds
    auto urlSeedsSizer = new wxStaticBoxSizer(wxVERTICAL, this, i18n("url_seeds_input_per_line"));
    m_urlSeeds = new wxTextCtrl(urlSeedsSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxTE_MULTILINE);
    urlSeedsSizer->Add(m_urlSeeds, 1, wxEXPAND);

    // Progress, in the bottom
    auto progressSizer = new wxBoxSizer(wxVERTICAL);
    m_status = new wxTextCtrl(this, wxID_ANY, fmt::format(i18n("status_s"), i18n("status_ready")), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxBORDER_NONE);
    m_progress = new wxGauge(this, wxID_ANY, 0);
    m_progress->SetRange(100);
    progressSizer->Add(m_status, 0, wxEXPAND);
    progressSizer->AddSpacer(FromDIP(3));
    progressSizer->Add(m_progress, 0, wxEXPAND);

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    m_create = new wxButton(this, ptID_BTN_CREATE_TORRENT, i18n("create_torrent"));
    m_create->SetDefault();

    buttonsSizer->Add(new wxHyperlinkCtrl(this, wxID_ANY, i18n("how_to_create_torrents"), "https://docs.picotorrent.org/en/master/creating-torrents.html"), 0, wxALIGN_CENTER_VERTICAL);
    buttonsSizer->AddStretchSpacer();
    buttonsSizer->Add(m_create);
    buttonsSizer->AddSpacer(FromDIP(7));
    buttonsSizer->Add(new wxButton(this, wxID_CANCEL));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(pathSizer, 0, wxEXPAND | wxALL, FromDIP(11));
    sizer->Add(optionsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));
    sizer->Add(trackersSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));
    sizer->Add(urlSeedsSizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(11));
    sizer->Add(progressSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, FromDIP(11));
    sizer->Add(buttonsSizer, 0, wxEXPAND | wxALL, FromDIP(11));

    this->SetSizerAndFit(sizer);
    this->SetSize(FromDIP(wxSize(400, 600)));

    this->Bind(wxEVT_BUTTON, &CreateTorrentDialog::OnBrowsePath, this, ptID_BTN_BROWSE_DIR);
    this->Bind(wxEVT_BUTTON, &CreateTorrentDialog::OnBrowsePath, this, ptID_BTN_BROWSE_FILE);
    this->Bind(wxEVT_BUTTON, &CreateTorrentDialog::OnCreateTorrent, this, ptID_BTN_CREATE_TORRENT);

    this->Bind(ptEVT_CREATE_TORRENT_THREAD_START,
        [this](wxThreadEvent&)
        {
            m_path->Disable();
            m_selectDir->Disable();
            m_selectFile->Disable();
            m_create->Disable();
            m_status->SetLabel(fmt::format(i18n("status_s"), i18n("status_adding_files")));
        });

    this->Bind(ptEVT_CREATE_TORRENT_THREAD_STOP,
        [this](wxThreadEvent& evt)
        {
            m_path->Enable();
            m_selectDir->Enable();
            m_selectFile->Enable();
            m_create->Enable();

            m_status->SetLabel(fmt::format(i18n("status_s"), i18n("status_saving_torrent")));

            wxFileDialog save(
                this,
                wxEmptyString,
                wxEmptyString,
                wxEmptyString,
                "Torrent file (*.torrent)|*.torrent",
                wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

            if (save.ShowModal() != wxID_OK)
            {
                return;
            }

            auto sp = evt.GetPayload<StopPayload>();
            std::ofstream out(save.GetPath().ToStdString(), std::ios_base::binary);
            lt::bencode(std::ostream_iterator<char>(out), sp.e);
        });

    this->Bind(ptEVT_CREATE_TORRENT_THREAD_PROGRESS,
        [this](wxThreadEvent& evt)
        {
            auto pp = evt.GetPayload<ProgressPayload>();
            m_status->SetLabel(
                fmt::format(
                    i18n("status_s"),
                    fmt::format(
                        i18n("status_hashing_piece"), pp.currentPiece + 1, pp.totalPieces)));

            float progress = 0;

            if (pp.currentPiece > 0)
            {
                progress = (pp.currentPiece + 1) / float(pp.totalPieces);
            }

            m_progress->SetValue(static_cast<int>(progress * 100));
        });
}

CreateTorrentDialog::~CreateTorrentDialog()
{
    if (m_worker.joinable()) { m_worker.join(); }
}

void CreateTorrentDialog::GenerateTorrent(std::unique_ptr<CreateTorrentParams> p)
{
    wxQueueEvent(this, new wxThreadEvent(ptEVT_CREATE_TORRENT_THREAD_START));

    lt::file_storage fs;
    lt::add_files(fs, p->path.string());

    lt::create_torrent ct(fs, 0);
    ct.set_comment(p->comment.c_str());
    ct.set_creator(p->creator.c_str());
    ct.set_priv(p->priv);

    for (size_t i = 0; i < p->trackers.size(); i++)
    {
        // Don't add more than int32 max trackers. ok?
        ct.add_tracker(p->trackers.at(i), static_cast<int>(i));
    }

    for (size_t i = 0; i < p->url_seeds.size(); i++)
    {
        ct.add_url_seed(p->url_seeds.at(i));
    }

    lt::error_code ec;
    lt::set_piece_hashes(
        ct,
        p->path.parent_path().string(),
        [this, &ct](lt::piece_index_t idx)
        {
            ProgressPayload pp;
            pp.currentPiece = static_cast<int>(idx);
            pp.totalPieces = ct.num_pieces();

            auto evt = new wxThreadEvent(ptEVT_CREATE_TORRENT_THREAD_PROGRESS);
            evt->SetPayload(pp);

            wxQueueEvent(this, evt);
        },
        ec);

    if (ec)
    {
        return;
    }

    lt::entry e;

    try
    {
        e = ct.generate();
    }
    catch (const std::exception&)
    {
        return;
    }

    StopPayload sp;
    sp.e = e;

    auto sevt = new wxThreadEvent(ptEVT_CREATE_TORRENT_THREAD_STOP);
    sevt->SetPayload(sp);

    wxQueueEvent(this, sevt);
}

void CreateTorrentDialog::OnBrowsePath(wxCommandEvent& evt)
{
    switch (evt.GetId())
    {
    case ptID_BTN_BROWSE_DIR:
    {
        wxDirDialog dlg(this);
        if (dlg.ShowModal() != wxID_OK) { return; }
        m_path->SetValue(dlg.GetPath());
        break;
    }
    case ptID_BTN_BROWSE_FILE:
    {
        wxFileDialog dlg(this);
        if (dlg.ShowModal() != wxID_OK) { return; }
        m_path->SetValue(dlg.GetPath());
        break;
    }
    }
}

void CreateTorrentDialog::OnCreateTorrent(wxCommandEvent&)
{
    auto params = std::make_unique<CreateTorrentParams>();
    params->comment = m_comment->GetValue();
    params->creator = m_creator->GetValue();
    params->path = m_path->GetValue().ToStdWstring();
    params->priv = m_private->IsChecked();

    {
        wxStringTokenizer tokenizer(m_trackers->GetValue());
        while (tokenizer.HasMoreTokens()) { params->trackers.push_back(tokenizer.GetNextToken().ToStdString()); }
    }

    {
        wxStringTokenizer tokenizer(m_urlSeeds->GetValue());
        while (tokenizer.HasMoreTokens()) { params->url_seeds.push_back(tokenizer.GetNextToken().ToStdString()); }
    }

    // wait for previous run to join before starting new. this shouldn't
    // happen since we disable the button when the worker starts and
    // enable it last thing before it ends

    if (m_worker.joinable()) { m_worker.join(); }
    m_worker = std::thread(&CreateTorrentDialog::GenerateTorrent, this, std::move(params));
}
