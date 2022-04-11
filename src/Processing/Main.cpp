#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define NBT_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "nbt/nbtDriver.h"

#include "Comparison.h"
#include "Utils.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <math.h>
#include <algorithm>
#include <random>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <array>
#include <chrono>
#include <bitset>
#include <stdio.h>
#include <time.h>
#include <iomanip>

#include "psl/psl.h"

#include "GlobalVars.h"

//Moved ToLab to 'Conversion' namespace
//All the nbt related helper functions have been moved to /nbt/nbtDriver
//Comparisons here have been moved to the 'comparison' namespace.

int parseArguments(int& argc, char** argv) {
    std::vector<std::string> commandLineArgs = psl::argcvToStringVector(argc, argv);

    if (commandLineArgs.size() == 0) {
        //ADD: GUI
        return 0;
    }

    if (commandLineArgs.size() < 2 ) {
        std::cout << "ERROR: Not enough arguments, 2 are required to process an image" << std::endl;
        return 1;
    }

    //Load input image
    imageIn = stbi_load(commandLineArgs[0].c_str(), &width, &height, &channels, 0);
    if (imageIn == NULL) {
        std::cout << "ERROR: Image failed to load (only .png, .jpg, .jpeg files are acceptable)" << std::endl;
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
                StairCaseMode = flat;
            }
            else if (arg == "staircase") {
                StairCaseMode = staircase;
            }
            else if (arg == "ascending") {
                StairCaseMode = ascending;
            }
            else if (arg == "descending") {
                StairCaseMode = descending;
            }
            else if (arg == "unlimited") {
                StairCaseMode = unlimited;
            }
            else {
                std::cout << "ERROR: Invalid mode option" << std::endl;
                std::cout << "HERE-> " << arg << std::endl;
                invalidArgs = true;
            }

        //Dithering
        } else if (arg == "--nodither") {
            //ADD: other dithering modes
            noDither = true;

        //ADD: This to help text
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

int parseSettings() {
    //Open the file
    std::fstream Settings;
    Settings.open(SettingsFileName, std::fstream::in);
    //Check if the file is there
    if (!Settings) {
        //Generate new file if not
        Settings.open(SettingsFileName, std::fstream::app);
        Settings << settings_Text << std::flush;
        Settings.close();
        Settings.open(SettingsFileName, std::fstream::in);
    }

    //Parse file.
    std::string line;
    while (std::getline(Settings, line)) {
        if (line.size() >= 2 && !(line.at(0) == '/' && line.at(1) == '/')) {
            if (isdigit(line.at(0)) && isdigit(line.at(1))) {
                int blockID = std::stoi(line.substr(0, 2)) - 1;
                if (line.find("minecraft:") == std::string::npos) {
                    std::cout << "ERROR: Incorrect syntax in Settings.txt. Block given is invalid" << std::endl;
                    std::cout << "HERE-> " << line << std::endl;
                    return 1;
                }
                //TEST: Does removing block types work?
                BlockTypes[blockID] = line.substr(line.find("minecraft:") + 10);
                switch (StairCaseMode) {
                    case flat:
                        AllowedColors[blockID * 4 + 1] = true;
                        break;
                    case staircase:
                        AllowedColors[blockID * 4 + 0] = true;
                        AllowedColors[blockID * 4 + 1] = true;
                        AllowedColors[blockID * 4 + 2] = true;
                        break;
                    case ascending:
                        AllowedColors[blockID * 4 + 1] = true;
                        AllowedColors[blockID * 4 + 2] = true;
                        break;
                    case descending:
                        AllowedColors[blockID * 4 + 0] = true;
                        AllowedColors[blockID * 4 + 1] = true;
                        break;
                    case unlimited:
                        AllowedColors[blockID * 4 + 0] = true;
                        AllowedColors[blockID * 4 + 1] = true;
                        AllowedColors[blockID * 4 + 2] = true;
                        AllowedColors[blockID * 4 + 3] = true;
                        break;
                    default:
                        break;
                }
            }
            else if (line.substr(0,8) == "support:") {
                if (line.find("minecraft:") == std::string::npos) {
                    std::cout << SettingsFile_Generic_Error << "Block given is invalid" << std::endl;
                    std::cout << "HERE-> " << line << std::endl;
                    return 1;
                }
                BlockTypes[TOTAL_COLORS] = line.substr(line.find("minecraft:") + 10);
            }
            else if (line.substr(0,13) == "needsSupport:") {
                std::stringstream s_stream(line.substr(13));
                while (s_stream.good()) {
                    std::string substr;
                    std::getline(s_stream, substr, ',');
                    if (std::stoi(substr) < 1 || std::stoi(substr) > TOTAL_COLORS) {
                        std::cout << SettingsFile_Generic_Error << "Support ID is invalid" << std::endl;
                        std::cout << "HERE-> " << line << std::endl;
                        return 1;
                    }
                    needsSupport[std::stoi(substr) - 1] = true;
                }
            }
            else if (line.substr(0,3) == "min") {
                std::stringstream s_stream(line);
                int num = -100000000;
                while (s_stream.good()) {
                    std::string substr;
                    s_stream >> substr;
                    if (std::stringstream(substr) >> num) {
                        break;
                    }
                }
                if (num == -100000000) {
                    std::cout << SettingsFile_Generic_Error << "Invalid range" << std::endl;
                    std::cout << "HERE-> " << line << std::endl;
                    return 1;
                }
                if (line.at(3) == 'R') {
                    minR = num;
                }
                else if (line.at(3) == 'G') {
                    minG = num;
                }
                else {
                    minB = num;
                }
            }
            else if (line.substr(0, 3) == "max") {
                std::stringstream s_stream(line);
                int num = -100000000;
                while (s_stream.good()) {
                    std::string substr;
                    s_stream >> substr;
                    if (std::stringstream(substr) >> num) {
                        break;
                    }
                }
                if (num == -100000000) {
                    std::cout << SettingsFile_Generic_Error << "Invalid range" << std::endl;
                    std::cout << "HERE-> " << line << std::endl;
                    return 1;
                }
                if (line.at(3) == 'R') {
                    maxR = num;
                }
                else if (line.at(3) == 'G') {
                    maxG = num;
                }
                else {
                    maxB = num;
                }
            }
            else if (line.substr(0, 15) == "constMaxHeight:") {
                std::stringstream s_stream(line.substr(15));
                std::string substr;
                s_stream >> substr;
                if (substr == "true" || substr == "True") {
                    constMaxHeight = true;
                }
            }
        }
    }
    Settings.close();

    std::cout << "FINISHED PARSING SETTINGS" << std::endl;
    return -1;
}

int writeLitematic(std::vector<std::array<short, 3>> *Layers, const short& NBT_X, const short& NBT_Y, const short& NBT_Z, const int& NBT_XZ) {
    std::cout << "WRITING LITEMATIC" << std::endl;

    char blockBits = 6;
    const int BLOCK_ARRAY_SIZE = NBT_XZ * NBT_Y * blockBits / 64;

    //ADD: make these dynamic
    std::string Author = "Hunsinger";
    std::string Description = "";
    std::string NBTName = "Testing";

    nbt_tag_t* tagTop = CreateCompoundTag("");
    nbt_tag_t* tagMeta = CreateCompoundTag("Metadata");
    nbt_tag_t* tagEncSize = CreateCompoundTag("EnclosingSize");
    CreateIntTag("x", NBT_X, tagEncSize);
    CreateIntTag("y", NBT_Y, tagEncSize);
    CreateIntTag("z", NBT_Z + 1, tagEncSize);
    CloseTag(tagEncSize, tagMeta);
    CreateStringTag("Author", Author, tagMeta);
    CreateStringTag("Description", Description, tagMeta);
    CreateStringTag("Name", NBTName, tagMeta);
    CreateIntTag("RegionCount", 1, tagMeta);
    CreateLongTag("TimeCreated", time(0), tagMeta);
    CreateLongTag("TimeModified", time(0), tagMeta);
    CreateIntTag("TotalBlocks", BLOCK_ARRAY_SIZE, tagMeta);
    CreateIntTag("TotalVolume", BLOCK_ARRAY_SIZE, tagMeta);
    CloseTag(tagMeta, tagTop);
    nbt_tag_t* tagRegions = CreateCompoundTag("Regions");
    nbt_tag_t* tagMain = CreateCompoundTag(NBTName);
    nbt_tag_t* tagPos = CreateCompoundTag("Position");
    CreateIntTag("x", 0, tagPos);
    CreateIntTag("y", 0, tagPos);
    CreateIntTag("z", 0, tagPos);
    CloseTag(tagPos, tagMain);
    nbt_tag_t* tagSize = CreateCompoundTag("Size");
    CreateIntTag("x", NBT_X, tagSize);
    CreateIntTag("y", NBT_Y, tagSize);
    CreateIntTag("z", NBT_Z + 1, tagSize);
    CloseTag(tagSize, tagMain);
    nbt_tag_t* tagPalette = CreateListTag("BlockStatePalette", nbt_tag_type_t::NBT_TYPE_COMPOUND);
    nbt_tag_t* tagPalAir = CreateCompoundTag("");
    CreateStringTag("Name", "minecraft:air", tagPalAir);
    CloseTag(tagPalAir, tagPalette, false);
    for (int i = 0; i < TOTAL_COLORS + 1; i++) {
        nbt_tag_t* tagPal = CreateCompoundTag("");
        CreateStringTag("Name", "minecraft:" + BlockTypes[i], tagPal);
        CloseTag(tagPal, tagPalette, false);
    }
    CloseTag(tagPalette, tagMain);
    CreateListTag("Entities", nbt_tag_type_t::NBT_TYPE_COMPOUND, tagMain);
    CreateListTag("PendingBlockTicks", nbt_tag_type_t::NBT_TYPE_COMPOUND, tagMain);
    CreateListTag("PendingFluidTicks", nbt_tag_type_t::NBT_TYPE_COMPOUND, tagMain);
    CreateListTag("TileEntities", nbt_tag_type_t::NBT_TYPE_COMPOUND, tagMain);

    int64_t* longArr = new int64_t[BLOCK_ARRAY_SIZE]();
    int64_t* Bits = longArr;
    *Bits = 0;
    char bitIndex = 0;
    char longBitIndex = 0;

    int y = 0;
    while (y != NBT_Y && Layers[y].size() == 0) {
        y++;
    }
    short Prev[3] = { -1, 0, 0 };

    for (y; y != NBT_Y; y++) {
        for (std::vector<std::array<short, 3>>::iterator it = Layers[y].begin(); it != Layers[y].end(); it++) {
            int limit = it->at(0) - Prev[0] - 1 + (it->at(1) - Prev[1]) * NBT_X + (y - Prev[2]) * NBT_XZ;

            if (limit >= 32 - bitIndex) {
                limit -= 32 - bitIndex;
                Bits += 1 + (bitIndex < 22) + (bitIndex < 11);
                bitIndex = 0;
                longBitIndex = 0;
            }

            int shift = limit >> 5;
            Bits += 3 * shift;
            limit -= shift << 5;

            if (limit > (unsigned char)(21 - bitIndex)) {
                limit -= 22 - bitIndex;
                Bits += 1 + (bitIndex < 11);
                bitIndex = 22;
                longBitIndex = 4;
            }
            else if (limit > (unsigned char)(10 - bitIndex)) {
                limit -= 11 - bitIndex;
                Bits++;
                bitIndex = 11;
                longBitIndex = 2;
            }
            bitIndex += limit;
            longBitIndex += blockBits * limit;


            *Bits = *Bits | ((((int64_t)it->at(2) >> 2) + 1) << longBitIndex);
            bitIndex++;
            longBitIndex += blockBits;

            switch (bitIndex) {
                case 11:
                    Bits++;
                    *Bits = ((it->at(2) >> 2) + 1) >> 4;
                    longBitIndex = 2;
                    break;
                case 22:
                    Bits++;
                    *Bits = ((it->at(2) >> 2) + 1) >> 2;
                    longBitIndex = 4;
                    break;
                case 32:
                    Bits++;
                    *Bits = 0;
                    bitIndex = 0;
                    longBitIndex = 0;
                    break;
                default:
                    break;
            }

            Prev[0] = it->at(0);
            Prev[1] = it->at(1);
            Prev[2] = y;
        }
    }

    CreateLongArrayTag("BlockStates", longArr, BLOCK_ARRAY_SIZE, tagMain);

    delete[] longArr;
    longArr = nullptr;

    CloseTag(tagMain, tagRegions);
    CloseTag(tagRegions, tagTop);
    CreateIntTag("MinecraftDataVersion", 2580, tagTop);
    CreateIntTag("Version", 5, tagTop);

    //FIX: Change this file name to be dynamic
    write_nbt_file("FoxIconTest3.litematic", tagTop, NBT_WRITE_FLAG_USE_GZIP);
    std::cout << "DONE WRITING LITEMATIC" << std::endl;

    return -1;
}

int main(int argc, char** argv) {
    psl_helperFunctionRunner(parseArguments(argc, argv));

    psl_helperFunctionRunner(parseSettings());

    /*********************************************************COLOR REDUCTION AND DITHERING****************************************************************/

    ColorSpace::Lab Colors[TOTAL_COLORS * 4];
    for (int i = 0; i < TOTAL_COLORS * 4; i++) {
        ColorSpace::ToLab(BlockColors[i], &Colors[i]);
    }

    const int HEIGHT = height;
    const int WIDTH = width;
    const size_t sizeIn = WIDTH * HEIGHT * channels;
    const size_t sizeOut = WIDTH * HEIGHT * 3;
    unsigned char* imageOut = new unsigned char[sizeOut];

    const std::vector<std::vector<std::vector<double>>> DitheringAlgorithms = {
            /*Floyd-Steinberg*/ {
                {16},		//Divisor
                {1, 1, 7},	//Distributor {<total offset>, <width offset>, <multiplier>}
                {(double)WIDTH - 1, -1, 3},
                {(double)WIDTH, 0, 5},
                {(double)WIDTH + 1, 1, 1}
            },

            /*Jarvis-Judice-Ninke*/ {
                {48},
                {1, 1, 7},
                {2, 2, 5},
                {(double)WIDTH - 2, -2, 3},
                {(double)WIDTH - 1, -1, 5},
                {(double)WIDTH, 0, 7},
                {(double)WIDTH + 1, 1, 5},
                {(double)WIDTH + 2, 2, 7},
                {(double)WIDTH * 2 - 2, -2, 1},
                {(double)WIDTH * 2 - 1, -1, 3},
                {(double)WIDTH * 2, 0, 5},
                {(double)WIDTH * 2 + 1, 1, 3},
                {(double)WIDTH * 2 + 2, 2, 1}
            },

            /*Burkes*/ {
                {32},
                {1, 1, 8},
                {2, 2, 4},
                {(double)WIDTH - 2, -2, 2},
                {(double)WIDTH - 1, -1, 4},
                {(double)WIDTH, 0, 8},
                {(double)WIDTH + 1, 1, 4},
                {(double)WIDTH + 2, 2, 2}
            }
    };

    //ADD: ClI Options for ditherer
	int DitherChosen = 0;
	short Divisor = DitheringAlgorithms.at(DitherChosen).at(0).at(0);
	int DitherSize = DitheringAlgorithms.at(DitherChosen).size();

	int MaxSize = 0;
	for (int i = 1; i < DitherSize; i++) {
		if (DitheringAlgorithms.at(DitherChosen).at(i).at(0) > MaxSize) {
			MaxSize = DitheringAlgorithms.at(DitherChosen).at(i).at(0);
		}
	}
	MaxSize += HEIGHT * WIDTH;

	short* ErrorsR = new short[MaxSize]();
	short* ErrorsG = new short[MaxSize]();
	short* ErrorsB = new short[MaxSize]();
	std::vector<std::vector<std::vector<unsigned char>>> ColorLists[8] = { {},{},{},{},{},{},{},{} };

	auto start = std::chrono::high_resolution_clock::now();

	int direction = 1;
	int width_pos = 0;
	int height_pos = 0;
	int pos = 0;
	int times = 0;

	unsigned char** BlockData = new unsigned char* [WIDTH];
	for (int i = 0; i < WIDTH; i++) {
		BlockData[i] = new unsigned char [HEIGHT] {255};
	}

	short* prev_heights = new short[WIDTH];
	short* upRand = new short[WIDTH];
	short* downRand = new short[WIDTH];
	for (int i = 0; i < WIDTH; i++) {
		prev_heights[i] = 1;
		if (!constMaxHeight) {
			upRand[i] = rand() % 15 + 240;
			downRand[i] = rand() % 15 - 253;
		}
		else {
			upRand[i] = 255;
			downRand[i] = -255;
		}
	}
	int upFixes = 0;
	int downFixes = 0;

	const int BLOCKS_USED_HEIGHT = ceil(HEIGHT / 384.0f);
	const int BLOCKS_USED_WIDTH = ceil(WIDTH / 384.0f);
	int*** BlocksUsed = new int** [BLOCKS_USED_HEIGHT]();
	for (int i = 0; i < BLOCKS_USED_HEIGHT; i++) {
		BlocksUsed[i] = new int* [BLOCKS_USED_WIDTH]();
		for (int j = 0; j < BLOCKS_USED_WIDTH; j++)
			BlocksUsed[i][j] = new int[TOTAL_COLORS + 1]();
	}

	const short NBT_X = 384;
	const short NBT_Y = 256;
	const short NBT_Z = 384;
	const int NBT_XZ = NBT_X * (NBT_Z + 1);

	int qs = 0;

	//loop from the start of the input image to the end, copying RGB values over to the output image after altering them based on the algorithm
	for (unsigned char* p = imageIn, *pg = imageOut; p != imageIn + sizeIn; p += channels, pg += 3, pos++) {
		/*qs++;
		if (qs % 10000 == 0) {
			std::cout << qs << std::endl;
		}*/
		//Take the dithering errors for the current pixel and add them to the input RGB values
		short r = *p + ErrorsR[pos];
		short g = *(p + 1) + ErrorsG[pos];
		short b = *(p + 2) + ErrorsB[pos];
		short ErrR;
		short ErrG;
		short ErrB;


		//constrains the rgb values between negLimit and posLimit
		r = r < minR ? minR : r > maxR ? maxR : r;
		g = g < minG ? minG : g > maxG ? maxG : g;
		b = b < minB ? minB : b > maxB ? maxB : b;

		//Get the correct RGB->Block dictionary based on the polarity of the RGB values (8 total possible values)
		std::vector<std::vector<std::vector<unsigned char>>>* CurrColorList = &ColorLists[(r < 0) * 4 + (g < 0) * 2 + (b < 0)];
		short rIndex = abs(r);
		short gIndex = abs(g);
		short bIndex = abs(b);
		unsigned char colorIndex = 0;
		bool foundMatch = false;
		bool initError = false;

		//Search through the dictionary for an RGB match
		if (rIndex < (*CurrColorList).size() && gIndex < (*CurrColorList).at(rIndex).size() && bIndex < (*CurrColorList).at(rIndex).at(gIndex).size()) {
			colorIndex = (*CurrColorList).at(rIndex).at(gIndex).at(bIndex);
			foundMatch = colorIndex != 255;
		}
		initError = foundMatch;

		double minError;
		//Convert the RGB values to the LAB color space
		ColorSpace::Lab colorLAB;
        ColorSpace::ToLab({ (double)r, (double)g, (double)b }, &colorLAB);

		//If a match is not found
		if (!foundMatch) {
			int i = 0;
			while (!AllowedColors[i]) {
				i++;
			}

			//Set a default error to compare future errors against
			double L = colorLAB.l - Colors[i].l;
			double A = colorLAB.a - Colors[i].a;
			double B = colorLAB.b - Colors[i].b;
			minError = (L * L + A * A + B * B) * 1000;
			colorIndex = i;

			for (i++; i < (TOTAL_COLORS << 2); i++) {
				if (AllowedColors[i]) {
					//Get the error between the selected color and each of the possible block colors
					L = colorLAB.l - Colors[i].l;
					A = colorLAB.a - Colors[i].a;
					B = colorLAB.b - Colors[i].b;
					double currError = (L * L + A * A + B * B);

					//Penalties for certain blocks (decreases the likelihood of certain blocks being used)
					/*if ((i >> 2) != 3 && (i >> 2) != 6 && (i >> 2) != 11 && (i >> 2) != 7 && (i >> 2) != 28) {
						currError = (currError + 1000) * 1000;
					}*/
					/*if (i >= 35*4 && i < 51*4) {
						currError *= 2;
					}*/
					/*else if (i > 149) {
						currError *= 4;
					}*/

					//Save index if the error is the lowest found so far
					if (currError < minError) {
						minError = currError;
						colorIndex = i;
					}
				}
			}

			//Resize the dictionary of RGB->Block conversion if necessary
			if (rIndex >= (*CurrColorList).size()) {
				(*CurrColorList).resize(rIndex + 1);
			}
			if (gIndex >= (*CurrColorList).at(rIndex).size()) {
				(*CurrColorList).at(rIndex).resize(gIndex + 1);
			}
			if (bIndex >= (*CurrColorList).at(rIndex).at(gIndex).size()) {
				//Fill unused space with -1
				(*CurrColorList).at(rIndex).at(gIndex).resize(bIndex + 1, 255);
			}

			//Update the dictionary with the new addition
			(*CurrColorList).at(rIndex).at(gIndex).at(bIndex) = colorIndex;
		}

		//Check for and apply height-limit fixes when necessary based on what shade of block color was chosen (staircasing)
		if (StairCaseMode != unlimited) {
			switch (colorIndex & 3) {
			case DOWN: //Staircasing downwards
				if (prev_heights[width_pos] < 1) {
					prev_heights[width_pos] = 1;
				}
				else if (prev_heights[width_pos] == upRand[width_pos]) {
					downFixes++;
					prev_heights[width_pos] = 1;
					colorIndex += 2;
					initError = true;
					//std::cout << "DOWNWARDS FIX AT " << pos << "  " << BlockColors[colorIndex].r << "," << BlockColors[colorIndex].g << "," << BlockColors[colorIndex].b << std::endl;
				}
				else {
					prev_heights[width_pos]++;
				}
				break;
			case UP: //Staircasing upwards
				if (prev_heights[width_pos] > 1) {
					prev_heights[width_pos] = -1;
				}
				else if (prev_heights[width_pos] == downRand[width_pos]) {
					upFixes++;
					prev_heights[width_pos] = 1;
					colorIndex -= 2;
					initError = true;
					//std::cout << "UPWARDS FIX AT " << pos << "  " << BlockColors[colorIndex].r << "," << BlockColors[colorIndex].g << "," << BlockColors[colorIndex].b << std::endl;
				}
				else {
					prev_heights[width_pos]--;
				}
				break;
			default: //Case 1 is no staircasing, therefore no checks are necessary
				break;
			}
		}

		//Get the new color from the list of allowed block colors
		ColorSpace::Rgb NewColor = BlockColors[colorIndex];
		//Increment the block used
		BlockData[width_pos][height_pos] = colorIndex;
		BlocksUsed[height_pos / NBT_X][width_pos / NBT_X][colorIndex >> 2]++;
		BlocksUsed[height_pos / NBT_X][width_pos / NBT_X][TOTAL_COLORS] += needsSupport[colorIndex >> 2];
		TotalBlocksUsed[colorIndex >> 2]++;
		TotalBlocksUsed[TOTAL_COLORS] += needsSupport[colorIndex >> 2];
		//Save the RGB values to the output image and determine the RGB errors
		*pg = (unsigned char)NewColor.r;
		*(pg + 1) = (unsigned char)NewColor.g;
		*(pg + 2) = (unsigned char)NewColor.b;
		ErrR = (r - NewColor.r);
		ErrG = (g - NewColor.g);
		ErrB = (b - NewColor.b);

		//Apply the dithering based on the dithering algorithm chosen and the RGB errors found above
		if (!noDither) {
			for (int i = 1; i < DitherSize; i++) {
				if (width_pos + DitheringAlgorithms.at(DitherChosen).at(i).at(1) >= 0 && width_pos + DitheringAlgorithms.at(DitherChosen).at(i).at(1) < WIDTH) {
					int new_pos = pos + DitheringAlgorithms.at(DitherChosen).at(i).at(0);
					short multiply = DitheringAlgorithms.at(DitherChosen).at(i).at(2);
					ErrorsR[new_pos] += (ErrR * multiply) / Divisor;
					ErrorsG[new_pos] += (ErrG * multiply) / Divisor;
					ErrorsB[new_pos] += (ErrB * multiply) / Divisor;
				}
			}
		}

		//Update the position currently at in the image
		width_pos++;
		height_pos += width_pos == WIDTH;
		width_pos *= width_pos != WIDTH;
	}

	int ColorListSizes[8] = { 0,0,0,0,0,0,0,0 };
	int TotalSpace = 0;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < ColorLists[i].size(); j++) {
			for (int k = 0; k < ColorLists[i].at(j).size(); k++) {
				for (int l = 0; l < ColorLists[i].at(j).at(k).size(); l++) {
					ColorListSizes[i] += (ColorLists[i].at(j).at(k).at(l) != -1);
				}
				TotalSpace += ColorLists[i].at(j).at(k).size();
			}
		}
	}

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	int tot = 0;
	std::cout << "\nBlocks Used: " << std::endl;
	int terracotta = 0;

	for (int i = 0; i < TOTAL_COLORS + 1; i++) {
		std::cout << BlockTypes[i] << " : ";
		std::cout << TotalBlocksUsed[i] << std::endl;
		/*if (TotalBlocksUsed[i] < 1000) {
			std::cout << TotalBlocksUsed[i] << std::endl;
		}
		else if (TotalBlocksUsed[i] < 1000000) {
			std::cout << TotalBlocksUsed[i] / 1000 << "," << std::setw(3) << TotalBlocksUsed[i] % 1000 << std::setw(0) << std::endl;
		}
		else {
			std::cout << TotalBlocksUsed[i] / 1000000 << "," << std::setw(3) << TotalBlocksUsed[i] / 1000 % 1000 << "," << TotalBlocksUsed[i] % 1000 << std::setw(0) << std::endl;
		}*/
		tot += TotalBlocksUsed[i];
		if (i >= 35 && i < 51) {
			terracotta += TotalBlocksUsed[i];
		}
	}
	std::cout << "TOTAL: " << tot / 1000000 << "," << std::setw(3) << tot / 1000 % 1000 << "," << tot % 1000 << std::setw(0) << std::endl << std::endl;
	std::cout << "Terracotta: " << terracotta << std::endl;
	for (int i = 0; i < BLOCKS_USED_HEIGHT; i++) {
		for (int j = 0; j < BLOCKS_USED_WIDTH; j++) {
			int palette = 1;
			for (int k = 0; k < TOTAL_COLORS + 1; k++) {
				palette += BlocksUsed[i][j][k] > 0;
			}
			std::cout << "Palette of " << argv[1] << "_" << i << "_" << j << ": " << palette << std::endl;
		}
	}
	std::cout << std::endl;

	std::cout << "PosPosPosSize = " << ColorListSizes[0] << std::endl;
	std::cout << "PosPosNegSize = " << ColorListSizes[1] << std::endl;
	std::cout << "PosNegPosSize = " << ColorListSizes[2] << std::endl;
	std::cout << "PosNegNegSize = " << ColorListSizes[3] << std::endl;
	std::cout << "NegPosPosSize = " << ColorListSizes[4] << std::endl;
	std::cout << "NegPosNegSize = " << ColorListSizes[5] << std::endl;
	std::cout << "NegNegPosSize = " << ColorListSizes[6] << std::endl;
	std::cout << "NegNegNegSize = " << ColorListSizes[7] << std::endl;
	std::cout << "Total Size used = " << TotalSpace << std::endl << std::endl;
	std::cout << "Upwards Fixes = " << upFixes << std::endl;
	std::cout << "Downwards Fixes = " << downFixes << std::endl << std::endl;
	std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl << std::endl;
    //ADD: Switch this to specified output image
	std::string out = "First.png";
	stbi_write_png(out.c_str(), width, height, 3, imageOut, 3 * width);

	std::cout << "hi" << std::endl;

	//return 0;


	/*********************************************************MAKING IT MC FRIENDLY****************************************************************/

	short*** Height_Indeces = new short** [NBT_X];
	for (int i = 0; i < NBT_X; i++) {
		Height_Indeces[i] = new short* [NBT_Z + 1]();
		for (int j = 0; j < NBT_Z + 1; j++)
			Height_Indeces[i][j] = new short[2]();
	}
	std::vector<unsigned char> Clearance[NBT_X];
	std::vector<std::array<short, 3>> Layers[NBT_Y];


	unsigned char* yHeight = new unsigned char[NBT_X];
	unsigned char* jump = new unsigned char[NBT_X]();
	//unsigned char* dist = new unsigned char[NBT_X]();
	//short* lastDistIndex = new short[NBT_X]();

	for (short x = 0; x < NBT_X; x++) {
		if (x == 1) {
			int q = 0;
		}

		unsigned char support;
		switch (BlockData[x][0] & 3) {
		case DOWN:
		case FLAT:
			{
			jump[x] = 1;
			bool brk = false;
			for (short dz = 1; dz < NBT_Z && !brk; dz++) {
				jump[x] += (BlockData[x][dz] & 3) == DOWN;
				brk = (BlockData[x][dz] & 3) == UP;
			}
			if (x == 1) {
				int q = 0;
			}
			yHeight[x] = jump[x];
			support = jump[x] + 1;
			Clearance[x].push_back(254 - support);
			Height_Indeces[x][0][1] = 0;
			Height_Indeces[x][1][1] = 0;
			//Height_Dist[x][0][1] = 254 - support;
			//DistIndeces[x].push_back(jump[x]);
			//lastDistIndex[x] = 0;
			}
			break;
		case UP:
			yHeight[x] = 1;
			if ((BlockData[x][1] & 3) != UP) {
				jump[x] = 2;
				bool brk = false;
				for (short dz = 2; dz < NBT_Z && !brk; dz++) {
					jump[x] += (BlockData[x][dz] & 3) == DOWN;
					brk = (BlockData[x][dz] & 3) == UP;
				}
				yHeight[x] = jump[x];
				Clearance[x].push_back(254 - jump[x]);
				Height_Indeces[x][0][1] = 1;
				Height_Indeces[x][1][1] = 1;
				//Height_Dist[x][0][1] = 254 - jump[x];
				//lastDistIndex[x] = 1;
				//lastDistIndex[x] = 255 - (yHeight[x] + 1);
			}
			else {
				unsigned char upwards = 1;
				bool brk = false;
				for (short dz = 1; dz < NBT_Z && !brk; dz++) {
					upwards += (BlockData[x][dz] & 3) == UP;
					brk = (BlockData[x][dz] & 3) == DOWN;
				}
				Clearance[x].push_back(254 - upwards);
				Height_Indeces[x][0][1] = 1;
				Height_Indeces[x][1][1] = 1;
				//Height_Dist[x][0][1] = 254 - upwards;
			}
			support = 0;
			break;
		default:
			break;
		}
		Height_Indeces[x][0][0] = support;
		Height_Indeces[x][1][0] = yHeight[x];
		 //Height_Dist[x][0][1];
		//Layers[support].push_back({ x, 0, TOTAL_COLORS << 2 });
		//Layers[yHeight[x]].push_back({ x, 1, BlockData[x][0] });
	}

	std::cout << "hi2" << std::endl;

	for (short z = 1; z < NBT_Z; z++) {
		for (short x = 0; x < NBT_X; x++) {
			if (x == 383 && z == 156 - 64) {
				int q = 0;
			}
			if (x == 1 && z > 240) {
				int q = 0;
			}
			Height_Indeces[x][z + 1][1] = Height_Indeces[x][z][1];
			//Height_Dist[x][z + 1][1] = Height_Dist[x][z][1];
			switch (BlockData[x][z] & 3) {
			case DOWN:
				{
				Height_Indeces[x][z + 1][1] &= 65534;
				jump[x]--;
				yHeight[x] = jump[x];
				//Height_Dist[x][z][1] = lastDistIndex[x];
				bool stairStart = true;
				bool brk = false;
				for (short dz = 1; z - dz >= 0 && !brk && stairStart; dz--) {
					stairStart = (BlockData[x][z - dz] & 3) != DOWN;
					brk = (BlockData[x][z - dz] & 3) == UP;
				}
				if (stairStart) {
					unsigned char upwards = yHeight[x];
					brk = false;
					for (short dz = 1; z + dz < NBT_Z && !brk; dz++) {
						upwards += (BlockData[x][z + dz] & 3) == UP;
						brk = (BlockData[x][z + dz] & 3) == DOWN;
					}
					Clearance[x].push_back(254 - upwards);
					Height_Indeces[x][z + 1][1] += 2;
					//Height_Dist[x][z + 1][1] = 254 - upwards;
				}
				}
				break;
			case UP:
				{
				Height_Indeces[x][z + 1][1] |= 1;
				yHeight[x]++;
				jump[x] = 0;
				bool brk = false;
				for (short dz = 1; z + dz < NBT_Z && !brk; dz++) {
					jump[x] += (BlockData[x][z + dz] & 3) == DOWN;
					brk = (BlockData[x][z + dz] & 3) == UP;
				}
				if (x == 1) {
					int q = z;
				}
				if (jump[x] != 0) {
					Height_Indeces[x][z + 1][1] &= 65534;
				}
				if (jump[x] > yHeight[x]) {
					yHeight[x] = jump[x];
					Clearance[x].push_back(254 - yHeight[x]);
					Height_Indeces[x][z + 1][1] += 2;
					//Height_Dist[x][z + 1][1] = 254 - yHeight[x];
				}
				
				bool stairStart = true;
				brk = false;
				for (short dz = 1; z - dz >= 0 && !brk && stairStart; dz--) {
					stairStart = (BlockData[x][z - dz] & 3) != UP;
					brk = (BlockData[x][z - dz] & 3) == DOWN;
				}
				if (stairStart) {
					unsigned char upwards = yHeight[x];
					brk = false;
					for (short dz = 1; z + dz < NBT_Z && !brk; dz++) {
						upwards += (BlockData[x][z + dz] & 3) == UP;
						brk = (BlockData[x][z + dz] & 3) == DOWN;
					}
					Clearance[x].push_back(254 - upwards);
					Height_Indeces[x][z + 1][1] += 2;
					//Height_Dist[x][z + 1][1] = 254 - upwards;
				}
				//lastDistIndex[x] = 255 - (yHeight[x] + 1);
				}
				break;
			default:
				break;
			}
			Height_Indeces[x][z + 1][0] = yHeight[x];
			//Height_Dist[x][z][1] = lastDistIndex[x];
			//Layers[yHeight[x]].push_back({ x, (short)(z + 1), BlockData[x][z] });
		}
	}

	std::cout << "hi3" << std::endl;

	unsigned char* addends = new unsigned char[NBT_X]();

	/*for (short x = 0; x < NBT_X; x++) {
		bool upwards = true;
		bool brk = false;
		for (short dz = 1; dz <= NBT_Z && !brk && upwards; dz++) {
			upwards = (BlockData[x][dz] & 3) != DOWN;
			brk = (BlockData[x][dz] & 3) == UP;
		}
		if (upwards) {
			unsigned char currHeight = Height_Indeces[x][0][0];
			unsigned char currClearance = Clearance[x].at(Height_Indeces[x][0][1]);
			unsigned char max = currHeight;
			for (int i = -3; i < 0; i++) {
				if (x + i >= 0 && /*(z == NBT_Z || (BlockData[x][z + 1] & 3) == (BlockData[x + i][z + 1] & 3)) && *//* Height_Indeces[x + i][0][0] > max&& Height_Indeces[x + i][0][0] - currHeight <= currClearance) {
					max = Height_Indeces[x + i][0][0];
				}
			}
			for (int i = 1; i < 4; i++) {
				if (x + i < NBT_X && /*(z == NBT_Z || (BlockData[x][z + 1] & 3) == (BlockData[x + i][z + 1] & 3)) &&*/ /*Height_Indeces[x + i][0][0] > max && Height_Indeces[x + i][0][0] - currHeight <= currClearance) {
					max = Height_Indeces[x + i][0][0];
				}
			}
			Height_Indeces[x][0][0] = max;
			if (Height_Indeces[x][1][1] == Height_Indeces[x][0][1]) {
				Height_Indeces[x][1][0] += max - currHeight;
				prevDists[x] = currClearance - (max - currHeight);
			}
			else {
				prevDists[x] = Height_Dist[x][1][1];
			}
		}
	}*/
	//unsigned char* addends = new unsigned char[NBT_X]();
	/*for (short x = 0; x < NBT_X; x++) {
		unsigned char currHeight = Height_Dist[x][0][0];
		unsigned char currDist = Height_Dist[x][0][1];
		unsigned char max = currHeight;
		for (int i = -3; i < 0; i++) {
			if (x + i >= 0 && Height_Dist[x + i][0][0] > max && Height_Dist[x + i][0][0] - currHeight >= currDist) {
				max = Height_Dist[x + i][0][0];
			}
		}
		for (int i = 1; i < 4; i++) {
			if (x + i < NBT_X && Height_Dist[x + i][0][0] > max && Height_Dist[x + i][0][0] - currHeight >= currDist) {
				max = Height_Dist[x + i][0][0];
			}
		}
		Height_Dist[x][0][0] = max;
		Height_Dist[x][0][1] = currDist - (max - currHeight);
		//addends[x] = Height_Dist[x][0][1];
	}*/
	for (short z = 0; z <= NBT_Z; z++) {
		for (short x = 0; x < NBT_X; x++) {
			/*bool upwards = true;
			bool brk = false;
			for (short dz = 1; z + dz <= NBT_Z && !brk && upwards; dz++) {
				upwards = (BlockData[x][z + dz] & 3) != DOWN;
				brk = (BlockData[x][z + dz] & 3) == UP;
			}*/
			if ((Height_Indeces[x][z][1] & 1) == 1) { //upwards) {
				/*if ((Height_Indeces[x][z][1] & 1) != 1) {
					int q = x + z;
				}*/
				unsigned char currHeight = Height_Indeces[x][z][0] + addends[x];
				unsigned char currClearence = Clearance[x].at(Height_Indeces[x][z][1] >> 1);
				//std::cout << "(" << x << "," << z << ")  " << (int) currHeight << " : " << (int) currClearence << std::endl;
				//unsigned char currDist = Height_Dist[x][z][1];
				unsigned char max = currHeight;
				for (int i = -3; i < 0; i++) {
					if (x + i >= 0 && (Height_Indeces[x + i][z][1] & 1) == 1 && Height_Indeces[x + i][z][0] > max && Height_Indeces[x + i][z][0] - currHeight <= currClearence) {
						max = Height_Indeces[x + i][z][0];
					}
				}
				for (int i = 1; i < 4; i++) {
					if (x + i < NBT_X && (Height_Indeces[x + i][z][1] & 1) == 1 && Height_Indeces[x + i][z][0] > max && Height_Indeces[x + i][z][0] - currHeight <= currClearence) {
						max = Height_Indeces[x + i][z][0];
					}
				}
				Height_Indeces[x][z][0] = max;
				//std::cout << " -- went to: " << max << std::endl;
				if (z != NBT_Z) {
					addends[x] += max - currHeight;
					if (x == 1) {
						int q = z;
					}
					Clearance[x].at(Height_Indeces[x][z][1] >> 1) -= max - currHeight;
					//Height_Indeces[x][z + 1][0] += max - currHeight;
					//Height_Dist[x][z + 1][1] = currDist - (max - currHeight);
					if ((Height_Indeces[x][z][1] >> 1) != (Height_Indeces[x][z + 1][1] >> 1)) {
						addends[x] = 0;
					}
				}
			}
		}
		for (short x = NBT_X - 1; x >= 0; x--) {
			if ((Height_Indeces[x][z][1] & 1) == 1) {
				unsigned char currHeight = Height_Indeces[x][z][0];
				unsigned char currClearence = Clearance[x].at(Height_Indeces[x][z][1] >> 1);
				unsigned char max = currHeight;
				for (int i = 1; i < 4; i++) {
					if (x + i < NBT_X && (Height_Indeces[x + i][z][1] & 1) == 1 && Height_Indeces[x + i][z][0] > max && Height_Indeces[x + i][z][0] - currHeight <= currClearence) {
						max = Height_Indeces[x + i][z][0];
					}
				}
				Height_Indeces[x][z][0] = max;
				addends[x] += max - currHeight;
				Clearance[x].at(Height_Indeces[x][z][1] >> 1) -= max - currHeight;
			}
		}
	}
	
	std::cout << "hi4" << std::endl;

	delete[] addends;
	//unsigned char* addends = new unsigned char[NBT_X]();

	for (short z = NBT_Z; z >= 0; z--) {
		for (short x = 0; x < NBT_X; x++) {
			if ((Height_Indeces[x][z][1] & 1) == 0) {
				unsigned char currHeight = Height_Indeces[x][z][0] + addends[x];
				unsigned char currClearence = Clearance[x].at(Height_Indeces[x][z][1] >> 1);
				unsigned char max = currHeight;
				for (int i = -3; i < 0; i++) {
					if (x + i >= 0 && (Height_Indeces[x + i][z][1] & 1) == 0 && Height_Indeces[x + i][z][0] > max && Height_Indeces[x + i][z][0] - currHeight <= currClearence) {
						max = Height_Indeces[x + i][z][0];
					}
				}
				for (int i = 1; i < 4; i++) {
					if (x + i < NBT_X && (Height_Indeces[x + i][z][1] & 1) == 0 && Height_Indeces[x + i][z][0] > max && Height_Indeces[x + i][z][0] - currHeight <= currClearence) {
						max = Height_Indeces[x + i][z][0];
					}
				}
				Height_Indeces[x][z][0] = max;
				if (z != 0) {
					addends[x] += max - currHeight;
					Clearance[x].at(Height_Indeces[x][z][1] >> 1) -= max - currHeight;
					if ((Height_Indeces[x][z][1] >> 1) != (Height_Indeces[x][z - 1][1] >> 1)) {
						addends[x] = 0;
					}
				}
			}
		}
		for (short x = NBT_X - 1; x >= 0; x--) {
			if ((Height_Indeces[x][z][1] & 1) == 0) {
				unsigned char currHeight = Height_Indeces[x][z][0];
				unsigned char currClearence = Clearance[x].at(Height_Indeces[x][z][1] >> 1);
				unsigned char max = currHeight;
				for (int i = 1; i < 4; i++) {
					if (x + i < NBT_X && (Height_Indeces[x + i][z][1] & 1) == 0 && Height_Indeces[x + i][z][0] > max && Height_Indeces[x + i][z][0] - currHeight <= currClearence) {
						max = Height_Indeces[x + i][z][0];
					}
				}
				Height_Indeces[x][z][0] = max;
				addends[x] += max - currHeight;
				Clearance[x].at(Height_Indeces[x][z][1] >> 1) -= max - currHeight;
			}
		}
	}

	/*for (int z = 0; z <= NBT_Z; z++) {
		for (int x = 0; x < NBT_X; x++) {
			Height_Indeces[x][z][0] = Clearance[x][Height_Indeces[x][z][1]];
		}
	}

	/*std::cout << "at 155: " << "(height: " << (int)Height_Dist[383][155 - 63][0] << "), (dist: " << (int)Height_Dist[383][155 - 63][1] << ")" << std::endl;
	std::cout << "at 156: " << "(height: " << (int)Height_Dist[383][156-63][0] << "), (dist: " << (int)Height_Dist[383][156 - 63][1] << ")" << std::endl;
	int repeat = Height_Dist[383][0][1];
	std::cout << "------HEIGHT_DIST------" << std::endl << std::endl;
	std::cout << "at 63: " << "(height: " << (int) Height_Dist[383][0][0] << "), (dist: " << repeat << ")" << std::endl;
	for (int i = 1; i < NBT_Z + 1; i++) {
		if (Height_Dist[383][i][1] != repeat) {
			repeat = Height_Dist[383][i][1];
			std::cout << "at " << i + 63 << ": " << "(height: " << (int) Height_Dist[383][i][0] << "), (dist: " << repeat << ")" << std::endl;
		}
	}
	std::cout << "\n\n";*/

	std::cout << "hi6" << std::endl;
	
	for (short x = 0; x < NBT_X; x++) {
		Layers[Height_Indeces[x][0][0]].push_back({ x, 0, (TOTAL_COLORS << 2)});
	}
	for (short z = 1; z < NBT_Z + 1; z++) {
		for (short x = 0; x < NBT_X; x++) {
			Layers[Height_Indeces[x][z][0]].push_back({ x, z, BlockData[x][z + 1] });
		}
	}

	delete[] yHeight;
	delete[] jump;

    psl_helperFunctionRunner(writeLitematic(Layers, NBT_X, NBT_Y, NBT_Z, NBT_XZ));

    //Clean up
	for (int i = 0; i < BLOCKS_USED_HEIGHT; i++) {
		for (int j = 0; j < BLOCKS_USED_WIDTH; j++)
			delete[] BlocksUsed[i][j];
		delete[] BlocksUsed[i];
	}
	delete[] BlocksUsed;

	return 0;
}