#include "mainApp.h"

wxIMPLEMENT_APP(mainApp);

bool mainApp::OnInit() {
    frame = new mainFrame();
    return true;
}