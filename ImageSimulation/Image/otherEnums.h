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
		mrc_BYTE	= 0,
		mrc_SHORT	= 1,
		mrc_FLOAT	= 2,
		mrc_SHORT2	= 3,
		mrc_FLOAT2	= 4,
		mrc_INT		= 6,
		mrc_CHAR3	= 16
	};
}