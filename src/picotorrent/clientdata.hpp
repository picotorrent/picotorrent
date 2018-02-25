#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
    template<typename T>
    class ClientData : public wxClientData
    {
    public:
        ClientData(T value)
            : m_val(value)
        {
        }

        T GetValue()
        {
            return m_val;
        }

    private:
        T m_val;
    };
}
