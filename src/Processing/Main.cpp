#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <stdlib.h>

#include "json.hpp"

//#define DEBUG
#include "psl/psl.h"
#include "psl/image.h"

#include "mainConsts.h"



int parseArguments(int& argc, char** &argv, StairCaseMode &StairCaseMode, std::string &outputName, DitherMode &DitherMode, std::string &SettingsFileName, psl::Image &image) {
    std::vector<std::string> commandLineArgs = psl::argcvToStringVector(argc, argv);

    if (commandLineArgs.size() == 0) {
        //ADD: GUI
        return 0;
    }

    //FIX: Add help before this
    if (commandLineArgs.size() < 2 ) {
        std::cout << "ERROR: Not enough arguments, 2 are required to process an image" << std::endl;
        return 1;
    }

    //Load input image
    if (image.loadFile(commandLineArgs[0]) == 1) {
        std::cout << "ERROR: Image failed to load (only .png, .jpg, or .jpeg files are acceptable)" << std::endl;
        std::cout << "HERE-> " << commandLineArgs[0] << std::endl;
        return 1;
    }

    //Set output image
    outputName = commandLineArgs[1];
    std::fstream outTest(outputName);
    if (outTest) {
        std::string input;
        std::cout << "WARNING: " << outputName << " already exists! Would you like to replace it? (Y/N): ";
        std::cin >> input;
        if (std::toupper(input.at(0)) != 'Y') {
            return -1;
        }
    }

    bool invalidArgs = false;
    int i = 2;
    while(commandLineArgs.size() > i) {
        std::string arg = commandLineArgs[i];
        if(i == 0);
        else if(arg == "--help") {
            std::cout << help_Text << std::endl;
            return 0;

            //Stair casing mode
        } else if (arg == "--mode") {
            i++;
            arg = commandLineArgs[i];

            if (arg == "flat") {
                StairCaseMode = StairCaseMode::flat;
            }
            else if (arg == "staircase") {
                StairCaseMode = StairCaseMode::staircase;
            }
            else if (arg == "ascending") {
                StairCaseMode = StairCaseMode::ascending;
            }
            else if (arg == "descending") {
                StairCaseMode = StairCaseMode::descending;
            }
            else if (arg == "unlimited") {
                StairCaseMode = StairCaseMode::unlimited;
            }
            else {
                std::cout << "ERROR: Invalid mode option" << std::endl;
                std::cout << "HERE-> " << arg << std::endl;
                invalidArgs = true;
            }

        } else if (arg == "--DitherMode") {
            i++;
            arg = commandLineArgs[i];

            //ADD: other dithering modes
            if (arg == "none") {
                DitherMode = DitherMode::none;
            } else if (arg == "floyd-steinberg") {
                DitherMode = DitherMode::Floyd_Steinberg;
            }

        } else if (arg == "--config") {
            i++;
            arg = commandLineArgs[i];
            SettingsFileName = arg;
        } else {
            std::cout << "ERROR: Invalid argument" << std::endl;
            std::cout << "HERE-> " << arg << std::endl;
            invalidArgs = true;
        }
        i++;
    }

    if(invalidArgs) {
        return 1;
    }

    std::cout << "FINISHED PARSING ARGUMENTS" << std::endl;
    return -1;
}

#define Json nlohmann::json
//#define Image psl::Image
#define Color psl::image::Color

struct BlockColor {
    std::string blockName;
    bool needsSupport;
    Color color;
};

BlockColor* blockColorsArray;
ullong blockColorsSize = 0;

//ADD: Switch this to json
int parseJSONSettings(std::string fileName, std::vector<BlockColor> &blockColors) {
    //Open the file
    std::fstream settings;
    settings.open(fileName, std::fstream::in);
    //Check if the file is there
    if (!settings) {
        //Generate new file if not
        settings.open(fileName, std::fstream::app);
        settings << settings_Text << std::flush;
        settings.close();
        settings.open(fileName, std::fstream::in);
    }

    Json settingJsonContent;
    settings >> settingJsonContent;

    Json colors = settingJsonContent["colors"];

    int i = 1;
    for(Json color : colors) {
        if(color.size() < 4) {
            std::cout << "ERROR: Incorrect syntax in settings file. Color " << i << " is malformed and doesn't have four data points" << std::endl;
            std::cout << "HERE-> " << color << std::endl;
            return 1;
        }

        BlockColor blockColor;
        blockColor.color.r = color[0];
        blockColor.color.g = color[1];
        blockColor.color.b = color[2];

        Json colorMetaData = color[3];

        auto rawBlockID = colorMetaData.find("block ID");
        if (rawBlockID == colorMetaData.end()) {
            std::cout << "ERROR: Incorrect syntax in settings file. 'block ID' for color " << i << " is missing" << std::endl;
            std::cout << "HERE-> " << colorMetaData << std::endl;
            return 1;
        }

        auto rawNeedsSupport = colorMetaData.find("needs support");
        if (rawNeedsSupport == colorMetaData.end()) {
            std::cout << "ERROR: Incorrect syntax in settings file. 'needs support' for color " << i << " is missing" << std::endl;
            std::cout << "HERE-> " << colorMetaData << std::endl;
            return 1;
        }

        blockColor.needsSupport = *rawNeedsSupport;
        blockColor.blockName = *rawBlockID;

        blockColors.push_back(blockColor);

        i++;
    }

    blockColorsSize = blockColors.size();
    blockColorsArray = new BlockColor[blockColors.size()];
    i = 0;

    for(BlockColor blockColor : blockColors) {
        blockColorsArray[i] = blockColor;
        i++;
    }

    return -1;
}

struct errorPixel {
    double r;
    double g;
    double b;
};

class imageWithError : public psl::Image {
public:
    std::vector<std::vector<errorPixel>> error;
};

int polarsReduceColors(psl::Image &image, psl::Image &outImage, std::vector<BlockColor> &blockColors) {
    //Figure out the error for each pixel
    //Push that error out

    std::vector<Pixel> row;

    Color c = blockColors[0].color;

    int hight = image.getHeight();
    int width = image.getWidth();
    int ih = 0;
    int iw = 0;

    //int colors = blockColors.size();
    int i = 0;
    int delta = 1000;
    int tempDelta;
    BlockColor bestColor;

    std::vector<Pixel> vectorPixel;
    Pixel pixel;

    while (hight > ih) {
        vectorPixel = image[ih];
        row.clear();
        while(width > iw) {
            pixel = vectorPixel[iw];

            //This gets the closest color
            i = 0;
            delta = 800;
            while (blockColorsSize > i) {
                BlockColor testColor = blockColorsArray[i];
                //ADD: Color discouraging here.
                //ADD: weight to luma? (Just square each one)
                //I didn't feel like doing msvc inline asm so here's a c++ implementation of it.


                #if defined(WIN_32)
                if((tempDelta = abs(pixel.r - testColor.color.r) + abs(pixel.b - testColor.color.b) + abs(pixel.g - testColor.color.g)) < delta) {
                    delta = tempDelta;
                    bestColor = testColor;
                }
                #else
                asm(
                    //Reset some registers
                        "xor rdx, rdx;" //Normal ones
                        "xor r8, r8;"

                        "xor rax, rax;" //Outputs
                        "xor r9, r9;"
                        "xor r10, r10;"
                        "xor r11, r11;"


                        //Get the delta for r
                        "mov dl, [rbx];" //Put the r of the real color in dx
                        "mov r8b, [rcx];" //Put the r of the test color in r8

                        "mov r9b, dl;" //Put the real color's red (rcr) in the output (Just using as a register)
                        "sub r9w, r8w;" // Subtract test color red (tcr) from rcr putting it in the output. r9 is really the output.
                        "sub r8w, dx;" // Subtract rcr from tcr putting it in tcr.
                        "cmovg r9w, r8w;" //If tcr-rcr is greater than (in this case not negative) rcr-tcr put it in the output. Rcr-tcr is already in the output.

                        "xor rdx, rdx;" //Reset registers
                        "xor r8, r8;"


                        //Get the delta for g
                        "mov dl, [rbx + 1];" //Load data
                        "mov r8b, [rcx + 1];"

                        "mov r10b, dl;" //Do math
                        "sub r10w, r8w;"
                        "sub r8w, dx;"
                        "cmovg r10w, r8w;"

                        "xor rdx, rdx;" //Reset registers
                        "xor r8, r8;"


                        //Get the delta for b
                        "mov dl, [rbx + 2];"
                        "mov r8b, [rcx + 2];"

                        "mov r11b, dl;"
                        "sub r11w, r8w;"
                        "sub r8w, dx;"
                        "cmovg r11w, r8w;"

                        //Total deltas
                        "add eax, r9d;"
                        "add eax, r10d;"
                        "add eax, r11d;"

                        : "=a" ( tempDelta )
                        : "b" ( &pixel ), "c" ( &testColor.color )
                        : "r8", "dx", "r9", "r10", "r11"
                        );


                if(tempDelta < delta) {
                    delta = tempDelta;
                    bestColor = testColor;
                }
                #endif
                i++;
            }
            row.push_back(bestColor.color);



            iw++;
        }
        iw = 0;
        std::cout << ih << std::endl;
        ih++;
        outImage.push_back(row);
    }

    //writeImage(outImage, "out3.png");

    //Round to the nearest color


    return -1;
}

int main(int argc, char** argv) {
    /*********************************************************USER INPUT PARSING****************************************************************/

    //Config
    DitherMode ditherMode = DitherMode::none;
    StairCaseMode stairCaseMode = StairCaseMode::flat;
    //ADD: randomly scatter overflows.
    //This controls whether we randomly scatter overflows.
    bool scatterOverflows = false;
    std::string outputName = "out.png";
    std::string settingsFileName = "Settings.txt";

    //Input image
    psl::Image imageIn;

    psl_helperFunctionRunner(parseArguments(argc, argv, stairCaseMode, outputName, ditherMode, settingsFileName, imageIn));

    std::vector<BlockColor> blockColors;

    auto start = std::chrono::high_resolution_clock::now();
    psl_helperFunctionRunner(parseJSONSettings("newSettings.json", blockColors));
    std::cout << "Time taken by parseJSONSettings: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl << std::endl;

    //ADD: Create more block colors based on staircase preference.

    /*********************************************************COLOR REDUCTION AND DITHERING****************************************************************/

    /*psl::Image ProcessedImage;
    start = std::chrono::high_resolution_clock::now();
    psl_helperFunctionRunner(polarsReduceColors(imageIn, ProcessedImage, blockColors));
    std::cout << "Time taken by polarsReduceColors: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl << std::endl;

    ProcessedImage.writeFile("out4.png");*/

	return 0;
}