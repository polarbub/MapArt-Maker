const int TOTAL_COLORS = 61;
const int DOWN = 0;
const int FLAT = 1;
const int UP = 2;

//ADD: make these controllable from settings.txt
ColorSpace::Rgb BlockColors[TOTAL_COLORS * 4] = {
        {  89, 125,  39 }, { 109, 153,  48 }, { 127, 178,  56 }, {  67,  94,  29 }, //GRASS:					1
        { 174, 164, 115 }, { 213, 201, 140 }, { 247, 233, 163 }, { 130, 123,  86 }, //SAND:						2
        { 140, 140, 140 }, { 171, 171, 171 }, { 199, 199, 199 }, { 105, 105, 105 }, //WOOL:						3
        { 180,   0,   0 }, { 220,   0,   0 }, { 255,   0,   0 }, { 135,   0,   0 }, //FIRE:						4
        { 112, 112, 180 }, { 138, 138, 220 }, { 160, 160, 255 }, {  84,  84, 135 }, //ICE:						5
        { 117, 117, 117 }, { 144, 144, 144 }, { 167, 167, 167 }, {  88,  88,  88 }, //METAL:					6
        {   0,  87,   0 }, {   0, 106,   0 }, {   0, 124,   0 }, {   0,  65,   0 }, //PLANT:					7
        { 180, 180, 180 }, { 220, 220, 220 }, { 255, 255, 255 }, { 135, 135, 135 }, //SNOW:						8
        { 115, 118, 129 }, { 141, 144, 158 }, { 164, 168, 184 }, {  86,  88,  97 }, //CLAY:						9
        { 106,  76,  54 }, { 130,  94,  66 }, { 151, 109,  77 }, {  79,  57,  40 }, //DIRT:						10
        {  79,  79,  79 }, {  96,  96,  96 }, { 112, 112, 112 }, {  59,  59,  59 }, //STONE:					11
        {  45,  45, 180 }, {  55,  55, 220 }, {  64,  64, 255 }, {  33,  33, 135 }, //WATER:					12
        { 100,  84,  50 }, { 123, 102,  62 }, { 143, 119,  72 }, {  75,  63,  38 }, //WOOD:						13
        { 180, 177, 172 }, { 220, 217, 211 }, { 255, 252, 245 }, { 135, 133, 129 }, //QUARTZ:					14
        { 152,  89,  36 }, { 186, 109,  44 }, { 216, 127,  51 }, { 114,  67,  27 }, //COLOR_ORANGE:				15
        { 125,  53, 152 }, { 153,  65, 186 }, { 178,  76, 216 }, {  94,  40, 114 }, //COLOR_MAGENTA:			16
        {  72, 108, 152 }, {  88, 132, 186 }, { 102, 153, 216 }, {  54,  81, 114 }, //COLOR_LIGHT_BLUE:			17
        { 161, 161,  36 }, { 197, 197,  44 }, { 229, 229,  51 }, { 121, 121,  27 }, //COLOR_YELLOW:				18
        {  89, 144,  17 }, { 109, 176,  21 }, { 127, 204,  25 }, {  67, 108,  13 }, //COLOR_LIGHT_GREEN:		19
        { 170,  89, 116 }, { 208, 109, 142 }, { 242, 127, 165 }, { 128,  67,  87 }, //COLOR_PINK:				20
        {  53,  53,  53 }, {  65,  65,  65 }, {  76,  76,  76 }, {  40,  40,  40 }, //COLOR_GRAY:				21
        { 108, 108, 108 }, { 132, 132, 132 }, { 153, 153, 153 }, {  81,  81,  81 }, //COLOR_LIGHT_GRAY:			22
        {  53,  89, 108 }, {  65, 109, 132 }, {  76, 127, 153 }, {  40,  67,  81 }, //COLOR_CYAN:				23
        {  89,  44, 125 }, { 109,  54, 153 }, { 127,  63, 178 }, {  67,  33,  94 }, //COLOR_PURPLE:				24
        {  36,  53, 125 }, {  44,  65, 153 }, {  51,  76, 178 }, {  27,  40,  94 }, //COLOR_BLUE:				25
        {  72,  53,  36 }, {  88,  65,  44 }, { 102,  76,  51 }, {  54,  40,  27 }, //COLOR_BROWN:				26
        {  72,  89,  36 }, {  88, 109,  44 }, { 102, 127,  51 }, {  54,  67,  27 }, //COLOR_GREEN:				27
        { 108,  36,  36 }, { 132,  44,  44 }, { 153,  51,  51 }, {  81,  27,  27 }, //COLOR_RED:				28
        {  17,  17,  17 }, {  21,  21,  21 }, {  25,  25,  25 }, {  13,  13,  13 }, //COLOR_BLACK:				29
        { 176, 168,  54 }, { 215, 205,  66 }, { 250, 238,  77 }, { 132, 126,  40 }, //GOLD:						30
        {  64, 154, 150 }, {  79, 188, 183 }, {  92, 219, 213 }, {  48, 115, 112 }, //DIAMOND:					31
        {  52,  90, 180 }, {  63, 110, 220 }, {  74, 128, 255 }, {  39,  67, 135 }, //LAPIS:					32
        {   0, 153,  40 }, {   0, 187,  50 }, {   0, 217,  58 }, {   0, 114,  30 }, //EMERALD:					33
        {  91,  60,  34 }, { 111,  74,  42 }, { 129,  86,  49 }, {  68,  45,  25 }, //PODZOL:					34
        {  79,   1,   0 }, {  96,   1,   0 }, { 112,   2,   0 }, {  59,   1,   0 }, //NETHER:					35
        { 147, 124, 113 }, { 180, 152, 138 }, { 209, 177, 161 }, { 110,  93,  85 }, //TERRACOTTA_WHITE:			36
        { 112,  57,  25 }, { 137,  70,  31 }, { 159,  82,  36 }, {  84,  43,  19 }, //TERRACOTTA_ORANGE:		37
        { 105,  61,  76 }, { 128,  75,  93 }, { 149,  87, 108 }, {  78,  46,  57 }, //TERRACOTTA_MAGENTA:		38
        {  79,  76,  97 }, {  96,  93, 119 }, { 112, 108, 138 }, {  59,  57,  73 }, //TERRACOTTA_LIGHT_BLUE:	39
        { 131,  93,  25 }, { 160, 114,  31 }, { 186, 133,  36 }, {  98,  70,  19 }, //TERRACOTTA_YELLOW:		40
        {  72,  82,  37 }, {  88, 100,  45 }, { 103, 117,  53 }, {  54,  61,  28 }, //TERRACOTTA_LIGHT_GREEN:	41
        { 112,  54,  55 }, { 138,  66,  67 }, { 160,  77,  78 }, {  84,  40,  41 }, //TERRACOTTA_PINK:			42
        {  40,  28,  24 }, {  49,  35,  30 }, {  57,  41,  35 }, {  30,  21,  18 }, //TERRACOTTA_GRAY:			43
        {  95,  75,  69 }, { 116,  92,  84 }, { 135, 107,  98 }, {  71,  56,  51 }, //TERRACOTTA_LIGHT_GRAY:	44
        {  61,  64,  64 }, {  75,  79,  79 }, {  87,  92,  92 }, {  46,  48,  48 }, //TERRACOTTA_CYAN:			45
        {  86,  51,  62 }, { 105,  62,  75 }, { 122,  73,  88 }, {  64,  38,  46 }, //TERRACOTTA_PURPLE:		46
        {  53,  43,  64 }, {  65,  53,  79 }, {  76,  62,  92 }, {  40,  32,  48 }, //TERRACOTTA_BLUE:			47
        {  53,  35,  24 }, {  65,  43,  30 }, {  76,  50,  35 }, {  40,  26,  18 }, //TERRACOTTA_BROWN:			48
        {  53,  57,  29 }, {  65,  70,  36 }, {  76,  82,  42 }, {  40,  43,  22 }, //TERRACOTTA_GREEN:			49
        { 100,  42,  32 }, { 122,  51,  39 }, { 142,  60,  46 }, {  75,  31,  24 }, //TERRACOTTA_RED:			50
        {  26,  15,  11 }, {  31,  18,  13 }, {  37,  22,  16 }, {  19,  11,   8 }, //TERRACOTTA_BLACK:			51
        { 133,  33,  34 }, { 163,  41,  42 }, { 189,  48,  49 }, { 100,  25,  25 }, //CRIMSON_NYLIUM:			52
        { 104,  44,  68 }, { 127,  54,  83 }, { 148,  63,  97 }, {  78,  33,  51 }, //CRIMSON_STEM:				53
        {  64,  17,  20 }, {  79,  21,  25 }, {  92,  25,  29 }, {  48,  13,  15 }, //CRIMSON_HYPHAE:			54
        {  15,  88,  94 }, {  18, 108, 115 }, {  22, 126, 134 }, {  11,  66,  70 }, //WARPED_NYLIUM:			55
        {  40, 100,  98 }, {  50, 122, 120 }, {  58, 142, 140 }, {  30,  75,  74 }, //WARPED_STEM:				56
        {  60,  31,  43 }, {  74,  37,  53 }, {  86,  44,  62 }, {  45,  23,  32 }, //WARPED_HYPHAE:			57
        {  14, 127,  93 }, {  17, 155, 114 }, {  20, 180, 133 }, {  10,  95,  70 }, //WARPED_WART_BLOCK:		58
        {  71,  71,  71 }, {  86,  86,  86 }, { 100, 100, 100 }, {  53,  53,  53 }, //DEEPSLATE:				59
        { 153, 123, 103 }, { 186, 150, 126 }, { 216, 175, 147 }, { 114,  92,  77 }, //RAW_IRON:					60
        {  89, 117, 105 }, { 109, 144, 129 }, { 127, 167, 150 }, {  67,  88,  79 }  //GLOW_LICHEN:				61
};

std::string BlockTypes[TOTAL_COLORS + 1];
bool AllowedColors[TOTAL_COLORS * 4] = { 0 };
bool needsSupport[TOTAL_COLORS] = { 0 };
int TotalBlocksUsed[TOTAL_COLORS + 1] = { 0 };

unsigned char* imageIn;
//FIX: Specify this on the command line
std::string outputName = "out.png";
int width, height, channels;

bool noDither = false;
bool constMaxHeight = false;
enum mode{
    none = 0,
    flat = 1,
    staircase = 2,
    ascending = 3,
    descending = 4,
    unlimited = 5
};
mode StairCaseMode = flat;
int minR = 0;
int minG = 0;
int minB = 0;
int maxR = 255;
int maxG = 255;
int maxB = 255;

#define settings_Text "//Hello User, this is the settings file, wherein you can change the main settings for the MapArt Maker" << std::endl\
			<< "//If you make changes to the file, be aware that if a line begins with \"//\", it will be ignored:" << std::endl\
			<< "//The top section is the blocks that will be used for the given color ID. For info on what blocks go with what color IDs, visit this page:" << std::endl\
			<< "//https://minecraft.fandom.com/wiki/Map_item_format#Color_table" << std::endl << std::endl\
			<< "01: minecraft:slime_block" << std::endl << "02: minecraft:birch_slab" << std::endl << "03: minecraft:mushroom_stem[down=true, east=true, north=true, south=true, up=true, west=true]" << std::endl\
			<< "04: minecraft:redstone_block" << std::endl << "05: minecraft:ice" << std::endl << "06: minecraft:iron_trapdoor" << std::endl\
			<< "07: minecraft:oak_leaves[persistent=true]" << std::endl << "08: minecraft:white_carpet" << std::endl << "09: minecraft:clay" << std::endl\
			<< "10: minecraft:jungle_slab" << std::endl << "11: minecraft:cobblestone_slab" << std::endl << "12: minecraft:water" << std::endl\
			<< "13: minecraft:oak_slab" << std::endl << "14: minecraft:sea_lantern" << std::endl << "15: minecraft:orange_carpet" << std::endl\
			<< "16: minecraft:magenta_carpet" << std::endl << "17: minecraft:light_blue_carpet" << std::endl << "18: minecraft:yellow_carpet" << std::endl\
			<< "19: minecraft:lime_carpet" << std::endl << "20: minecraft:pink_carpet" << std::endl << "21: minecraft:gray_carpet" << std::endl\
			<< "22: minecraft:light_gray_carpet" << std::endl << "23: minecraft:cyan_carpet" << std::endl << "24: minecraft:purple_carpet" << std::endl\
			<< "25: minecraft:blue_carpet" << std::endl << "26: minecraft:brown_carpet" << std::endl << "27: minecraft:green_carpet" << std::endl\
			<< "28: minecraft:red_carpet" << std::endl << "29: minecraft:black_carpet" << std::endl << "30: minecraft:light_weighted_pressure_plate" << std::endl\
			<< "31: minecraft:prismarine_slab" << std::endl << "32: minecraft:lapis_block" << std::endl << "33: minecraft:emerald_block" << std::endl\
			<< "34: minecraft:spruce_slab" << std::endl << "35: minecraft:nether_brick_slab" << std::endl << "36: minecraft:white_terracotta" << std::endl\
			<< "37: minecraft:orange_terracotta" << std::endl << "38: minecraft:magenta_terracotta" << std::endl << "39: minecraft:light_blue_terracotta" << std::endl\
			<< "40: minecraft:yellow_terracotta" << std::endl << "41: minecraft:lime_terracotta" << std::endl << "42: minecraft:pink_terracotta" << std::endl\
			<< "43: minecraft:gray_terracotta" << std::endl << "44: minecraft:light_gray_terracotta" << std::endl << "45: minecraft:cyan_terracotta" << std::endl\
			<< "46: minecraft:purple_terracotta" << std::endl << "47: minecraft:blue_terracotta" << std::endl << "48: minecraft:brown_terracotta" << std::endl\
			<< "49: minecraft:green_terracotta" << std::endl << "50: minecraft:red_terracotta" << std::endl << "51: minecraft:black_terracotta" << std::endl\
			<< "52: minecraft:crimson_nylium" << std::endl << "53: minecraft:crimson_slab" << std::endl << "54: minecraft:crimson_hyphae" << std::endl\
			<< "55: minecraft:warped_nylium" << std::endl << "56: minecraft:warped_slab" << std::endl << "57: minecraft:warped_hyphae" << std::endl\
			<< "58: minecraft:warped_wart_block" << std::endl << "59: minecraft:cobbled_deepslate_slab" << std::endl << "60: minecraft:raw_iron_block" << std::endl\
			<< "61: minecraft:glow_lichen[down=true]" << std::endl << std::endl\
			<< "//Additional settings:" << std::endl\
			<< "support: minecraft:cobblestone   //support block to place under blocks that require support, such as carpet" << std::endl\
			<< "needsSupport: 08, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 61   //blocks that require support" << std::endl << std::endl << std::endl\
			<< "//Advanced settings:" << std::endl\
			<< "minR: 0 	//the minimum that any R, G, or B value can be while dithering" << std::endl << "minG: 0" << std::endl << "minB: 0" << std::endl\
			<< "maxR: 255	//the maximum that any R, G, or B value can be while dithering" << std::endl << "maxG: 255" << std::endl << "maxB: 255" << std::endl\
			<< "//NOTE: In general, the further apart the min and max values are, the more effect the dithering will have and the colors will be more accurate." << std::endl\
			<< "//      However this comes at the cost of losing the sharpness of the original. There is no setting that is best for all images." << std::endl\
			<< "//      It is suggested to tweak these values for each image until you get what you want." << std::endl\
			<< "constMaxHeight: false   //if true, blocks will always go to the maximum build height before being grounded. If false, max height will be randomized, creating a dithered effect"

//FIX: This is bound to be wrong
#define help_Text "Usage: " << argv[0] << " input_image [output] [options]" << std::endl\
			<< "     [output] can either be an image, .nbt, or .litematica output" << std::endl\
			<< "     if not provided, default is output.png in same folder as input_image" << std::endl << std::endl\
			<< "Options:" << std::endl\
			<< "--nodither	prevents dithering of the image (not suggested)" << std::endl\
			<< "--mode <mode>	  sets the type of map to create. Options for <mode> are:" << std::endl\
			<< "		     FLAT, STAIRCASE, ASCENDING, DESCENDING, UNLIMITED" << std::endl\
			<< "		     default is FLAT" << std::endl\
			<< "		     UNLIMITED gives access to 4th shade but can't be put into Minecraft"

