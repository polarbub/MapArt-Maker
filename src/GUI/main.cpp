#include "includes.h"
#include "mainApp.h"

wxIMPLEMENT_APP_NO_MAIN(mainApp);

int main(int argc, char** argv) {
    std::cout << "test" << std::endl;
    //This starts the GUI
    wxEntry(argc, argv);
    return 0;
}