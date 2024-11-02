#include "mMain.h"

#include <filesystem>

#include "nashResolver.h"

namespace {
    void create_default_grid(wxGrid* grid, const int rows, const int cols)
    {
        // Grid
        grid->CreateGrid(rows, cols);
        grid->EnableEditing(true);
        grid->EnableGridLines(true);
        grid->SetMargins(0, 0);
        grid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);

        // Columns
        grid->EnableDragColMove(false);
        grid->EnableDragColSize(true);
        grid->SetColLabelSize(20);
        grid->SetDefaultColSize(20, true);
        grid->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

        // Rows
        grid->EnableDragRowSize(true);
        grid->SetRowLabelSize(20);
        grid->SetRowLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
    }

    void set_grid_values_to_zero(wxGrid* grid)
    {
        for (int i = 0; i < grid->GetNumberRows(); i++)
        {
            for (int j = 0; j < grid->GetNumberCols(); j++)
            {
                grid->SetCellValue(i, j, "0");
            }
        }
    }
}

enum m_main_id : uint16_t {
    id_matrix_configuration = 1000,
    id_run_tests,
};

void m_main::click_calculate(wxCommandEvent& event)
{
    std::vector<double> p1(ne_p2_ * ne_p1_);
    std::vector<double> p2(ne_p2_ * ne_p1_);

    const bool is_inverted{ invert_->GetValue() };

    for (int i = 0; i < ne_p1_; i++)
    {
        for (int j = 0; j < ne_p2_; j++)
        {
            p1[i * ne_p2_ + j] = wxAtof(table_j1_->GetCellValue(i, j));
            p2[i * ne_p2_ + j] = wxAtof(table_j2_->GetCellValue(i, j));
            if (is_inverted)
            {
                p1[j * ne_p1_ + i] = wxAtof(table_j1_->GetCellValue(i, j));
            }
            p2[j * ne_p1_ + i] = wxAtof(table_j2_->GetCellValue(i, j));
        }
    }

    std::swap(ne_p1_, ne_p2_);
    std::string result = "Unknown.";
    if (mode_->GetValue()) {
        result = calculate_pure_nash_equilibrium(p1, p2, ne_p1_, ne_p2_);
    }
    else {
        result = calculate_mixed_nash_equilibrium(p1, p2, ne_p1_, ne_p2_);
    }

    result_->AppendText(wxString(result));
    result_->Refresh();

    event.Skip();
}

void m_main::click_clear(wxCommandEvent& event)
{
    table_j1_->ClearGrid();
    table_j2_->ClearGrid();
    result_->Clear();

    set_grid_values_to_zero(table_j1_);
    set_grid_values_to_zero(table_j2_);

    table_j1_->Refresh();
    table_j2_->Refresh();
    result_->Refresh();

    event.Skip();
}

void m_main::click_matrix_configuration(wxCommandEvent& event)
{
    const auto dialog = new wxDialog(this, wxID_ANY, wxT("Matrix"), wxDefaultPosition, wxSize(300, 140));
    const auto label_p1 = new wxStaticText(dialog, wxID_ANY,
        wxT("# strategies player 1"), wxDefaultPosition + wxPoint(10, 10), wxDefaultSize, 0);
    const auto label_p2 = new wxStaticText(dialog, wxID_ANY,
        wxT("# strategies player 2"), label_p1->GetPosition() + wxPoint(0, 30), wxDefaultSize, 0);

    const auto b_sizer = new wxBoxSizer(wxVERTICAL);

    const auto text_p1 = new wxTextCtrl(dialog, 10001,
        wxT(""), label_p1->GetPosition() + wxPoint(150, -5), wxDefaultSize, 0);

    const auto text_p2 = new wxTextCtrl(dialog, 10002,
        wxT(""), label_p2->GetPosition() + wxPoint(150, -5), wxDefaultSize, 0);

    b_sizer->Add(text_p1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    b_sizer->Add(text_p2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    const auto btn_register = new wxButton(dialog, wxID_ANY,
        wxT("Change"), label_p2->GetPosition() + wxPoint(100, 30), wxDefaultSize, 0);

    btn_register->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(m_main::click_change_grid_size), nullptr, this);
    table_j1_->ForceRefresh();
    table_j2_->ForceRefresh();

    dialog->ShowModal();

    table_j1_->ForceRefresh();
    table_j2_->ForceRefresh();
    btn_register->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(m_main::click_change_grid_size), nullptr, this);

    dialog->Destroy();

    delete b_sizer;

    event.Skip();
}

void m_main::click_about(wxCommandEvent& event)
{
    wxMessageBox(wxT("Nash Calculator\n\nAuthor: Adrian Marquina Vichino\n\nLicense: GNU GPL v3\n\nVersion:" NASH_CALCULATOR_VERSION "\n\nContact:Cobollatin@gmail.com"));

    event.Skip();
}

void m_main::click_run_tests(wxCommandEvent& event) {
    result_->AppendText(wxT("===============================================================\n"));
    result_->AppendText(wxT("TESTS\n"));
    result_->AppendText(wxT("===============================================================\n"));

    result_->AppendText(wxString(run_all_tests()));

    result_->AppendText(wxT("TESTS RUN SUCCESSFULLY\n"));
    result_->AppendText(wxT("===============================================================\n"));

    result_->Refresh();

    event.Skip();
}

void m_main::click_change_grid_size(wxCommandEvent& event)
{
    const int ne_p1 = wxAtoi(dynamic_cast<wxTextCtrl*>(FindWindowById(10001))->GetValue());
    const int ne_p2 = wxAtoi(dynamic_cast<wxTextCtrl*>(FindWindowById(10002))->GetValue());
    if (ne_p1 < 1 && ne_p2 < 1)
    {
        wxMessageBox(wxT("Invalid range (Should be higher than 1)"), wxT("Error"),
            wxOK | wxICON_ERROR, this);
        return;
    }

    if (ne_p1 > 5 && ne_p2 > 5)
    {
        wxMessageBox(wxT("Ranges higher than 5 for player 1 or higher than 5 for player 2 could be difficult to navigate."), wxT("Warning"),
            wxOK | wxICON_EXCLAMATION, this);
    }

    if (ne_p1_ < ne_p1)
    {
        while (ne_p1_ < ne_p1)
        {
            table_j1_->AppendRows();
            table_j2_->AppendRows();
            ne_p1_++;
        }
    }
    else
    {
        while (ne_p1_ > ne_p1)
        {
            table_j1_->DeleteRows();
            table_j2_->DeleteRows();
            ne_p1_--;
        }
    }
    if (ne_p2_ < ne_p2)
    {
        while (ne_p2_ < ne_p2)
        {
            table_j1_->AppendCols();
            table_j2_->AppendCols();
            ne_p2_++;
        }
    }
    else
    {
        while (ne_p2_ > ne_p2)
        {
            table_j1_->DeleteCols();
            table_j2_->DeleteCols();
            ne_p2_--;
        }
    }

    table_j1_->ForceRefresh();
    table_j2_->ForceRefresh();

    set_grid_values_to_zero(table_j1_);
    set_grid_values_to_zero(table_j2_);

    event.Skip();
}

m_main::m_main(wxWindow* parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long style) : wxFrame(parent, id, title, pos, size, style)
{
    ne_p1_ = ne_p2_ = 2;

    if (std::filesystem::exists("icon.ico"))
    {
        this->SetIcon(wxIcon(wxT("icon.ico"), wxBITMAP_TYPE_ICO));
    }

    this->SetSizeHints(wxSize(800, 600), wxSize(800, 600));
    this->wxWindowBase::SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
    this->wxTopLevelWindowMSW::Maximize(false);

    const auto box_sizer = new wxBoxSizer(wxVERTICAL);
    const auto top_box_sizer = new wxBoxSizer(wxHORIZONTAL);
    const auto bottom_box_sizer = new wxBoxSizer(wxVERTICAL);

    const auto players_box_sizer = new wxBoxSizer(wxHORIZONTAL);
    const auto player_1_box_sizer = new wxBoxSizer(wxVERTICAL);
    const auto player_2_box_sizer = new wxBoxSizer(wxVERTICAL);

    const auto options_box_sizer = new wxBoxSizer(wxVERTICAL);

    box_sizer->Add(top_box_sizer, 1, wxEXPAND, 5);
    box_sizer->Add(bottom_box_sizer, 1, wxEXPAND, 5);

    top_box_sizer->Add(players_box_sizer, 1, wxEXPAND, 5);
    top_box_sizer->Add(options_box_sizer, 0, wxEXPAND, 5);

    players_box_sizer->Add(player_1_box_sizer, 1, wxEXPAND, 5);
    players_box_sizer->Add(player_2_box_sizer, 1, wxEXPAND, 5);

    const auto player_1 = new wxStaticText(this, wxID_ANY, wxT("Player 1"), wxDefaultPosition, wxDefaultSize, 0);
    player_1->Wrap(-1);

    table_j1_ = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(300, 200), wxBORDER_SIMPLE);
    create_default_grid(table_j1_, ne_p1_, ne_p2_);
    set_grid_values_to_zero(table_j1_);

    player_1_box_sizer->Add(player_1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    player_1_box_sizer->Add(table_j1_, 0, wxALL, 15);

    const auto player_2 = new wxStaticText(this, wxID_ANY, wxT("Player 2"), wxDefaultPosition, wxDefaultSize, 0);
    player_2->Wrap(-1);

    table_j2_ = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(300, 200), wxBORDER_SIMPLE);
    create_default_grid(table_j2_, ne_p1_, ne_p2_);
    set_grid_values_to_zero(table_j2_);

    player_2_box_sizer->Add(player_2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    player_2_box_sizer->Add(table_j2_, 0, wxALL, 5);

    const auto mode = new wxStaticText(this, wxID_ANY, wxT("Mode"), wxDefaultPosition, wxDefaultSize, 0);
    mode->Wrap(-1);
    options_box_sizer->Add(mode, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 15);

    mode_ = new wxRadioButton(this, wxID_ANY, wxT("Pure"), wxDefaultPosition, wxDefaultSize, 0);
    options_box_sizer->Add(mode_, 0, wxALL, 5);
    mode_->SetValue(true);

    const auto mixed = new wxRadioButton(this, wxID_ANY, wxT("Mixed"), wxDefaultPosition, wxDefaultSize, 0);
    options_box_sizer->Add(mixed, 0, wxALL, 5);
    mixed->SetValue(false);

    invert_ = new wxCheckBox(this, wxID_ANY, wxT("Invert strategies"), wxDefaultPosition, wxDefaultSize, 0);
    options_box_sizer->Add(invert_, 0, wxALL, 5);
    invert_->SetValue(false);

    const auto divider = new wxStaticText(this, wxID_ANY, wxT("-----"), wxDefaultPosition, wxDefaultSize, 0);
    divider->Wrap(-1);
    options_box_sizer->Add(divider, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    const auto actions = new wxStaticText(this, wxID_ANY, wxT("Actions"), wxDefaultPosition, wxDefaultSize, 0);
    actions->Wrap(-1);
    options_box_sizer->Add(actions, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    const auto calculate = new wxButton(this, wxID_ANY, wxT("Calculate"), wxDefaultPosition, wxDefaultSize, 0);
    options_box_sizer->Add(calculate, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    const auto clear = new wxButton(this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0);
    options_box_sizer->Add(clear, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    const auto result_label = new wxStaticText(this, wxID_ANY, wxT("Result"), wxDefaultPosition, wxDefaultSize, 0);
    result_label->Wrap(-1);
    bottom_box_sizer->Add(result_label, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    result_ = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    result_->Enable(true);
    result_->SetEditable(false);
    result_->SetScrollbar(wxVERTICAL, result_->GetScrollPos(wxVERTICAL) - result_->GetSize().GetHeight(), 255, 255);

    bottom_box_sizer->Add(result_, 1, wxALL | wxEXPAND, 5);

    this->SetSizer(box_sizer);
    this->wxTopLevelWindowBase::Layout();

    // Menu
    menu_bar_ = new wxMenuBar(0);
    matrix_menu_ = new wxMenu();
    matrix_menu_->Append(id_matrix_configuration, wxString(wxT("Configuration")));
    menu_bar_->Append(matrix_menu_, wxT("Matrix"));
    help_menu_ = new wxMenu();
    help_menu_->Append(id_run_tests, wxString(wxT("Run Tests")));
    help_menu_->Append(wxID_ABOUT, wxString(wxT("About")));
    menu_bar_->Append(help_menu_, wxT("Help"));
    this->wxFrameBase::SetMenuBar(menu_bar_);

    this->Centre(wxBOTH);

    // Connect Events
    calculate->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(m_main::click_calculate), nullptr, this);
    clear->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(m_main::click_clear), nullptr, this);
    Connect(id_matrix_configuration, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(m_main::click_matrix_configuration), nullptr, this);
    Connect(id_run_tests, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(m_main::click_run_tests), nullptr, this);
    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(m_main::click_about), nullptr, this);
}

m_main::~m_main()
{
    Disconnect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(m_main::click_calculate), nullptr, this);
    Disconnect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(m_main::click_clear), nullptr, this);
    Disconnect(id_matrix_configuration, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(m_main::click_matrix_configuration), nullptr, this);
    Disconnect(id_run_tests, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(m_main::click_run_tests), nullptr, this);
    Disconnect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(m_main::click_about), nullptr, this);
    this->wxTopLevelWindowMSW::Destroy();
}

