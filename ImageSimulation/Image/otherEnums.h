#pragma once

namespace ImageSpace {
	enum fft {
		forward,
		backward
	};

	enum transformationStatus {
		transformed,
		notTransformed
	};

	enum copyDirection {
		copyFFTtoImage,
		copyImagetoFFT
	};

	enum mrcMode {
		BYTE	= 0,
		SHORT	= 1,
		FLOAT	= 2,
		SHORT2	= 3,
		FLOAT2	= 4,
		INT		= 6,
		CHAR3	= 16
	};
}