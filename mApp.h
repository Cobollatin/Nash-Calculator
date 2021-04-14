#pragma once

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "mMain.h"

class mApp : public wxApp
{
public:
	virtual bool OnInit();
};

