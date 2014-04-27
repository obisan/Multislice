#pragma once

namespace ImageSpace {
	enum IMAGE_API fft {
		forward,
		backward
	};

	enum IMAGE_API transformationStatus {
		transformed,
		notTransformed
	};

	enum IMAGE_API copyDirection {
		copyFFTtoImage,
		copyImagetoFFT
	};
}