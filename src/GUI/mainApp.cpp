#include "mainApp.h"

wxIMPLEMENT_APP_NO_MAIN(mainApp);

bool mainApp::OnInit() {
    frame = new mainFrame();
    return true;
}