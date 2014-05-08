// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <complex>

#include <assert.h>
#include <io.h>
#include <iomanip>


// TODO: ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы

#include <AModel.h>
#include <BaseStructures.h>
#include <Model.h>
#include <ModelAce.h>
#include <ModelCoo.h>

#include <fftw3.h>

#include "../pugixml-1.2/src/pugixml.hpp"

#include "../Image/Image.h"
#include "../Image/MRC.h"

using namespace ImageSpace;
using namespace pugi;