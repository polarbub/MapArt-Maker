
#include "../mainFrame.h"

void mainFrame::OnClick(wxCommandEvent& event) {
    //wxMessageBox("\n\nThis is a image to litematica converter\n\nCopyright 2021 Hunsinger and polarbub 2021",
    //             "About", wxOK | wxICON_INFORMATION);

    auto* newBitmap = new wxBitmap("/mnt/pics/Thing.png", wxBITMAP_TYPE_PNG);
    staticBitmap->SetBitmap(*newBitmap);
    staticBitmap->SetSize(100,100);


    event.Skip();
}