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
#include "psl/threadDispatcher.h"

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

struct BlockColor : public Color {
    std::string blockName;
    bool needsSupport;
};

//ADD: Switch this to json
int parseJSONSettings(const std::string& fileName, ullong& blockColorsSize, BlockColor* &blockColorsArray) {
    std::vector<BlockColor> blockColors;
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
        blockColor.r = color[0];
        blockColor.g = color[1];
        blockColor.b = color[2];

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


    if(blockColors.size() == 0) {
        std::cout << "ERROR: Incorrect syntax in settings file. There are no colors defined" << std::endl;
        std::cout << "HERE-> " << colors << std::endl;
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

int polarsReduceColors(psl::Image &image, psl::Image &outImage, ullong blockColorsSize, BlockColor* &blockColorsArray) {
    //Figure out the error for each pixel
    //Push that error out

    std::vector<Pixel> row;

    BlockColor c = blockColorsArray[0];

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

                if((tempDelta = abs(pixel.r - testColor.r) + abs(pixel.b - testColor.b) + abs(pixel.g - testColor.g)) < delta) {
                    delta = tempDelta;
                    bestColor = testColor;
                }

                i++;
            }
            row.push_back(bestColor);

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

psl::thread::threadSafePrint *stream;

uint64_t threadFunction() {
    std::cout << "child thread ID: " << std::this_thread::get_id() << std::endl;
    std::stringstream ss;
    ss << std::this_thread::get_id();

    //stream->print(std::string("child thread ID: ") + ss.str());
    return std::stoull(ss.str());
}

int main(int argc, char** argv) {
    std::cout << "main thread ID: " << std::this_thread::get_id() << std::endl;

    stream = new psl::thread::threadSafePrint();

    psl::thread::PrioritizedThreadPool threadTest(1);

    std::vector<std::future<uint64_t>> outs;
    outs.push_back(threadTest.submit(threadFunction));
    outs.push_back(threadTest.submit(threadFunction));
    outs.push_back(threadTest.submit(threadFunction));
    outs.push_back(threadTest.submit(threadFunction));

    psl_sleep(1000);

    for(std::future<uint64_t> &id : outs) {
        std::cout << id.get() << std::endl;
    }


    delete stream;

    /*********************************************************USER INPUT PARSING****************************************************************/

    /*//Config
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

    BlockColor* blockColorsArray;
    ullong blockColorsSize = 0;

    auto start = std::chrono::high_resolution_clock::now();
    psl_helperFunctionRunner(parseJSONSettings("newSettings.json", blockColorsSize, blockColorsArray));
    std::cout << "Time taken by parseJSONSettings: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl << std::endl;

    //ADD: Create more block colors based on staircase preference.

    /*********************************************************COLOR REDUCTION AND DITHERING****************************************************************/

    /*psl::Image ProcessedImage;
    start = std::chrono::high_resolution_clock::now();
    psl_helperFunctionRunner(polarsReduceColors(imageIn, ProcessedImage, blockColorsSize, blockColorsArray));
    std::cout << "Time taken by polarsReduceColors: " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl << std::endl;

    ProcessedImage.writeFile("out4.png");*/

	return 0;
}