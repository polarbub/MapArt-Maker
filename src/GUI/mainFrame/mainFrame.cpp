#include "mainFrame.h"
#include "imagePanel/imagePanel.h"


mainFrame::mainFrame() : wxFrame(nullptr, wxID_ANY, "MapArt Maker", wxPoint() ,wxSize(800,600)) {
    //auto* boxSizer = new wxBoxSizer(wxVERTICAL);
    //boxSizer->Add(imageBitmapPointer, 1);
    //boxSizer->Add(imageBitmapPointer2, 1);

    /*auto *button = new wxButton(this, 1, "test", wxPoint(120, 70), wxSize(300, 30));
    button->Bind(wxEVT_BUTTON, &mainFrame::OnClick, this);

    auto* bitmap = new wxBitmap("/mnt/pics/mask triangle.png", wxBITMAP_TYPE_PNG);
    staticBitmap = new wxGenericStaticBitmap(this, wxID_ANY, *bitmap);
    staticBitmap->SetScaleMode(staticBitmap->Scale_AspectFit);
    staticBitmap->SetPosition(wxPoint(1, 100));
    staticBitmap->SetSize(100,100);

    staticBitmap->Show(true);*/

    new imagePanel(this, wxID_ANY);

    //this->SetSizer(boxSizer);
    //boxSizer->Layout();
    this->Show(true);
}

#include "event/eventIncludeCommon.h"