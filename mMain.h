#pragma once

#ifndef NASH_RESOLVER_MAIN
#define NASH_RESOLVER_MAIN

#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/grid.h>

class m_main final : public wxFrame
{
    int ne_p1_;
    int ne_p2_;
protected:
    wxGrid* table_j1_;
    wxGrid* table_j2_;
    wxRichTextCtrl* result_;
    wxRadioButton* mode_;
    wxCheckBox* invert_;
    wxMenuBar* menu_bar_;
    wxMenu* matrix_menu_;
    wxMenu* help_menu_;

    void click_calculate(wxCommandEvent& event);
    void click_clear(wxCommandEvent& event);
    void click_matrix_configuration(wxCommandEvent& event);
    void click_change_grid_size(wxCommandEvent& event);
    void click_about(wxCommandEvent& event);
    void click_run_tests(wxCommandEvent& event);
public:
    explicit m_main(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Nash Calculator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(840, 360), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~m_main() override;
};

#endif