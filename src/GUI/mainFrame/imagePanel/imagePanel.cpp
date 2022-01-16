#include "imagePanel.h"

imagePanel::imagePanel(wxWindow *parent, wxWindowID id) : wxScrolled<wxPanel>(parent, id) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    auto *button = new wxButton(this, 1, "test2", wxDefaultPosition,wxSize(400,400));
    sizer->Add(button);
    this->SetSizer(sizer);
    this->FitInside();
    this->SetScrollRate(5,5);
}
