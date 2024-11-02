#pragma once

#ifndef NASH_RESOLVER_APP
#define NASH_RESOLVER_APP

#include <wx/wx.h>

class m_app final : public wxApp
{
public:
    bool OnInit() override;
};

#endif
