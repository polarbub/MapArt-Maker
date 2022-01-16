#include "includes.h"
#include "GUI/mainApp.h"

int main(int argc, char** argv) {
    std::cout << "test" << std::endl;
    //This starts the GUI
    wxEntry(argc, argv);
    return 0;
}