#pragma once
#include "includes.h"
#include "mainFrame/mainFrame.h"

class mainApp : public wxApp {

    mainFrame *frame;

public:
    bool OnInit() override;

    //app() {};
    //~app() {};
};

