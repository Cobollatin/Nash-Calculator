#include "mMain.h"

void mMain::ClickCalcular(wxCommandEvent& event)
{
	double** p1 = new double* [neP1];
	double** p2 = new double* [neP1];
	for (int i = 0; i < neP1; i++)
	{
		p1[i] = new double[neP2];
		p2[i] = new double[neP2];
		for (int j = 0; j < neP2; j++)
		{
			p1[i][j] = wxAtoi(TablaJ1->GetCellValue(i, j));
			p2[i][j] = wxAtoi(TablaJ2->GetCellValue(i, j));
		}
	}
	mGame* juego;
	if(invertir->GetValue())
		juego = new mGame(p1, p2, neP1, neP2);
	else
		juego = new mGame(p2, p1, neP2, neP1);
	juego->Compute(Pura->GetValue());
	Resultado->AppendText(wxString(juego->get_result(invertir->GetValue())));
	Resultado->Refresh();

	delete juego;
	event.Skip();
}

void mMain::ClickLimpiar(wxCommandEvent& event)
{
	TablaJ1->ClearGrid();
	TablaJ2->ClearGrid();
	Resultado->Clear();

	TablaJ1->Refresh();
	TablaJ2->Refresh();
	Resultado->Refresh();

	event.Skip();
}

void mMain::ClickMatrices(wxCommandEvent& event)
{
	wxDialog* d = new wxDialog(this, wxID_ANY, wxT("Matrices"), wxDefaultPosition, wxSize(300,140));

	
	wxStaticText* labelP1 = new wxStaticText(d, wxID_ANY,
		wxT("# de estrategias jugador 1"), wxDefaultPosition + wxPoint(10, 10), wxDefaultSize, 0);
	wxStaticText* labelP2 = new wxStaticText(d, wxID_ANY,
		wxT("# de estrategias jugador 2"), labelP1->GetPosition() + wxPoint(0, 30), wxDefaultSize, 0);


	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	wxTextCtrl* txtJ1 = new wxTextCtrl(d, 10001,
		wxT(""), labelP1->GetPosition() + wxPoint(150, -5), wxDefaultSize, 0);
	
	wxTextCtrl* txtJ2 = new wxTextCtrl(d, 10002,
		wxT(""), labelP2->GetPosition() + wxPoint(150, -5), wxDefaultSize, 0);

	bSizer3->Add(txtJ1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
	bSizer3->Add(txtJ2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

	wxButton* btnRegister = new wxButton(d, wxID_ANY,
		wxT("Cambiar"), labelP2->GetPosition() + wxPoint(100, 30), wxDefaultSize, 0);

	btnRegister->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mMain::CambiarGrid), NULL, this);
	TablaJ1->ForceRefresh();
	TablaJ2->ForceRefresh();

	d->ShowModal();
	
	TablaJ1->ForceRefresh();
	TablaJ2->ForceRefresh();
	
	delete d;
	
	event.Skip();
}

void mMain::CambiarGrid(wxCommandEvent& event)
{
	int _neP1 = wxAtoi(((wxTextCtrl*)wxWindow::FindWindowById(10001))->GetValue());
	int _neP2 = wxAtoi(((wxTextCtrl*)wxWindow::FindWindowById(10002))->GetValue());
	if (_neP1 > 1 && _neP2 > 1)
	{
		if (neP1 < _neP1)
			while (neP1 < _neP1)
			{
				TablaJ1->AppendRows();
				TablaJ2->AppendRows();
				neP1++;
			}
		else
			while (neP1 > _neP1)
			{
				TablaJ1->DeleteRows();
				TablaJ2->DeleteRows();
				neP1--;
			}
		if (neP2 < _neP2)
			while (neP2 < _neP2)
			{
				TablaJ1->AppendCols();
				TablaJ2->AppendCols();
				neP2++;
			}
		else
			while (neP2 > _neP2)
			{
				TablaJ1->DeleteCols();
				TablaJ2->DeleteCols();
				neP2--;
			}
	}
	else
		wxMessageBox(wxT("Rango invalido (debe ser mayor a 1)"), wxT("Error"),
			wxOK | wxICON_INFORMATION, this);

	TablaJ1->ForceRefresh();
	TablaJ2->ForceRefresh();

	event.Skip();
}

mMain::mMain(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{

	neP1 = neP2 = 2;

	this->SetIcon(wxIcon(wxT("iconoMCexe.ico"),wxBITMAP_TYPE_ICO));

	this->SetSizeHints(wxSize(800, 600), wxSize(800, 600));
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT));
	this->Maximize(false);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	wxStaticText* Jugador1;
	Jugador1 = new wxStaticText(this, wxID_ANY, wxT("Jugador 1"), wxDefaultPosition, wxDefaultSize, 0);
	Jugador1->Wrap(-1);
	bSizer2->Add(Jugador1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

	TablaJ1 = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(250,200), 0);

	// Grid
	TablaJ1->CreateGrid(neP1, neP2);
	TablaJ1->EnableEditing(true);
	TablaJ1->EnableGridLines(true);
	//TablaJ1->EnableDragGridSize(false);
	TablaJ1->SetMargins(0, 0);

	// Columns
	TablaJ1->EnableDragColMove(false);
	TablaJ1->EnableDragColSize(true);
	TablaJ1->SetColLabelSize(30);
	TablaJ1->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	// Rows
	TablaJ1->EnableDragRowSize(true);
	TablaJ1->SetRowLabelSize(20);
	TablaJ1->SetRowLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	// Label Appearance

	// Cell Defaults
	TablaJ1->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);
	bSizer2->Add(TablaJ1, 0, wxALL, 30);

	Calcular = new wxButton(this, wxID_ANY, wxT("Calcular"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(Calcular, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 15);

	Limpiar = new wxButton(this, wxID_ANY, wxT("Limpiar"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(Limpiar, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 15);


	bSizer1->Add(bSizer2, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	wxStaticText* Jugador2;
	Jugador2 = new wxStaticText(this, wxID_ANY, wxT("Jugador 2"), wxDefaultPosition, wxDefaultSize, 0);
	Jugador2->Wrap(-1);
	bSizer3->Add(Jugador2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

	TablaJ2 = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(250, 200), 0);

	// Grid
	TablaJ2->CreateGrid(neP1, neP2);
	TablaJ2->EnableEditing(true);
	TablaJ2->EnableGridLines(true);
	//TablaJ2->EnableDragGridSize(false);
	TablaJ2->SetMargins(0, 0);

	// Columns
	TablaJ2->EnableDragColMove(false);
	TablaJ2->EnableDragColSize(true);
	TablaJ2->SetColLabelSize(30);
	TablaJ2->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	// Rows
	TablaJ2->EnableDragRowSize(true);
	TablaJ2->SetRowLabelSize(20);
	TablaJ2->SetRowLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	// Label Appearance

	// Cell Defaults
	TablaJ2->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);
	bSizer3->Add(TablaJ2, 0, wxALL, 30);

	wxStaticText* Resultado_label;
	Resultado_label = new wxStaticText(this, wxID_ANY, wxT("Resultado"), wxDefaultPosition, wxDefaultSize, 0);
	Resultado_label->Wrap(-1);
	bSizer3->Add(Resultado_label, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

	Resultado = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	Resultado->Enable(true);
	Resultado->SetEditable(false);
	Resultado->SetScrollbar(wxVERTICAL, Resultado->GetScrollPos(wxVERTICAL)- Resultado->GetSize().GetHeight(),255,255);

	bSizer3->Add(Resultado, 1, wxALL | wxEXPAND, 5);


	bSizer1->Add(bSizer3, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxVERTICAL);

	wxStaticText* Estrategia;
	Estrategia = new wxStaticText(this, wxID_ANY, wxT("Modo"), wxDefaultPosition, wxDefaultSize, 0);
	Estrategia->Wrap(-1);
	bSizer4->Add(Estrategia, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 15);

	Pura = new wxRadioButton(this, wxID_ANY, wxT("Pura"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(Pura, 0, wxALL, 10);
	Pura->SetValue(true);

	wxRadioButton* Mixta;
	Mixta = new wxRadioButton(this, wxID_ANY, wxT("Mixta"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(Mixta, 0, wxALL, 10);
	Mixta->SetValue(false);

	invertir = new wxCheckBox(this, wxID_ANY, wxT("Invertir estrategias"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(invertir, 0, wxALL, 10);
	invertir->SetValue(false);

	bSizer1->Add(bSizer4, 1, wxEXPAND, 5);

	this->SetSizer(bSizer1);
	this->Layout();
	m_menubar1 = new wxMenuBar(0);
	matrices = new wxMenu();
	

	matrices->Append(10000, wxString(wxT("Cambiar dimensión")));

	m_menubar1->Append(matrices, wxT("Matrices"));

	this->SetMenuBar(m_menubar1);

	this->Centre(wxBOTH);

	// Connect Events
	Calcular->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mMain::ClickCalcular), NULL, this);
	Limpiar->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mMain::ClickLimpiar), NULL, this);
	Connect(10000,wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(mMain::ClickMatrices), NULL, this);
}

mMain::~mMain()
{
	Calcular->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mMain::ClickCalcular), NULL, this);
	Limpiar->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(mMain::ClickLimpiar), NULL, this);
}

