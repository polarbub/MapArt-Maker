#pragma once
#include "includes.h"

class mainFrame : public wxFrame {
public:
    wxScrolled<wxPanel> *scrolledTest;
    wxGenericStaticBitmap *staticBitmap;
    mainFrame();
    //~mainFrame();

    void OnClick(wxCommandEvent& event);
};
