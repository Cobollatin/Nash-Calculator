#include "mApp.h"
#include "mMain.h"

wxIMPLEMENT_APP(m_app);

bool m_app::OnInit()
{
    const auto main_frame = new m_main(nullptr);
    main_frame->Show(true);
    return true;
}