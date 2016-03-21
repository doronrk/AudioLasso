#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

// miscellanious functions
namespace util
{
    Line<float> clipLineInsidePath(Path* p, Line<float>& line);
    Array<float> getYIntersections(Path* p, Line<float>& line);
    std::pair<float, float> getMinMax(Array<float> arr);
}



#endif  // UTIL_H_INCLUDED
