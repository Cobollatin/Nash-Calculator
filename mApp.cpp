#include "mApp.h"

wxIMPLEMENT_APP(mApp);

bool mApp::OnInit()
{
	mMain* mainFrame = new mMain(nullptr);
	mainFrame->Show(true);
	return true;
}