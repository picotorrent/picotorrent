#include "ConfigurePage.hpp"

#include <memory>

#include <atlctrls.h>

#include "../Sources/Source.hpp"
#include "WizardState.hpp"

using Wizard::ConfigurePage;

ConfigurePage::ConfigurePage(std::shared_ptr<Wizard::WizardState> state)
	: m_state(state)
{
    m_title = L"Configure";
    SetHeaderTitle(m_title.c_str());
}

ConfigurePage::~ConfigurePage()
{
}

LRESULT ConfigurePage::OnInitDialog(UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return TRUE;
}

BOOL ConfigurePage::OnSetActive()
{
    PropSheet_ShowWizButtons(
        m_hWnd,
        PSWIZB_CANCEL | PSWIZB_NEXT | PSWIZB_FINISH,
        PSWIZB_CANCEL | PSWIZB_NEXT);

    PropSheet_EnableWizButtons(
        m_hWnd,
        PSWIZB_CANCEL | PSWIZB_NEXT,
        PSWIZB_CANCEL | PSWIZB_NEXT);

    PropSheet_SetButtonText(m_hWnd, PSWIZB_NEXT, TEXT("Preview"));

	HWND hWnd = m_state->source->GetWindowHandle(ModuleHelper::GetResourceInstance(), m_hWnd);
	::ShowWindow(hWnd, SW_SHOW);

    return TRUE;
}
