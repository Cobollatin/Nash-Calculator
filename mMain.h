#pragma once
#include <wx/wxprec.h>
    #ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/richtext/richtextctrl.h>
#endif

#ifndef MGAME
#define MGAME
#include "mGame.h"
#endif // !



class mMain : public wxFrame
{
private:
	int neP1;
	int neP2;

protected:
	wxGrid* TablaJ1;
	wxButton* Calcular;
	wxButton* Limpiar;
	wxGrid* TablaJ2;
	wxRichTextCtrl* Resultado;
	wxRadioButton* Pura;
	wxCheckBox* invertir;
	wxMenuBar* m_menubar1;
	wxMenu* matrices;
	
	void ClickCalcular(wxCommandEvent& event);
	void ClickLimpiar(wxCommandEvent& event);
	void ClickMatrices(wxCommandEvent& event);
	void CambiarGrid(wxCommandEvent& event);

public:

	mMain(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("TEORIA DE JUEGOS"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(850, 364), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	~mMain();

};

