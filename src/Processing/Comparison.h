#ifndef COMPARISON_H
#define COMPARISON_H

#include "ColorSpace.h"

namespace ColorSpace {
	struct EuclideanComparison {
		static double Compare(IColorSpace *a, IColorSpace *b);
	};

	struct Cie1976Comparison {
		static double Compare(IColorSpace *a, IColorSpace *b);
	};

    double Compare76(const ColorSpace::Lab* Color1, const ColorSpace::Lab* Color2);
    double Compare00(const ColorSpace::Lab* lab_a, const ColorSpace::Lab* lab_b);
    double Compare94(const ColorSpace::Lab* lab_a, const ColorSpace::Lab* lab_b);


	struct Cie94Comparison {
		enum APPLICATION {
			GRAPHIC_ARTS = 0,
			TEXTILES
		};

		struct Application {
			double kl, k1, k2;
			Application(APPLICATION appType);
		};

		static double Compare(IColorSpace *a, IColorSpace *b, APPLICATION appType=GRAPHIC_ARTS);
	};


	struct Cie2000Comparison {
		static double Compare(IColorSpace *a, IColorSpace *b);
	};


	struct CmcComparison {
		static const double defaultLightness;
		static const double defaultChroma;
		static double Compare(IColorSpace *a, IColorSpace *b);
	};
}

#endif // COMPARISON_H

