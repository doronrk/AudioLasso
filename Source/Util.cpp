#include "Util.h"

namespace util
{
    Line<float> clipLineInsidePath(Path* p, Line<float>& line) {
        Array<float> intersections = getYIntersections(p, line);
        if (intersections.size() == 0) {
            return Line<float>{0, 0, 0, 0};
        }
        std::pair<float, float> minMax = getMinMax(intersections);
        Line<float>clipped{line};
        clipped.setStart(line.getStartX(), minMax.first);
        clipped.setEnd(line.getStartX(), minMax.second);
        return clipped;
    }
    
    Array<float> getYIntersections(Path* p, Line<float>& line)
    {
        PathFlatteningIterator i (*p, AffineTransform::identity);
        Point<float> intersection;
        Array<float> intersections;
        while (i.next())
        {
            if (line.intersects (Line<float> (i.x1, i.y1, i.x2, i.y2), intersection))
            {
                intersections.add(intersection.y);
            }
        }
        return intersections;
    }
    
    std::pair<float, float> getMinMax(Array<float> arr) {
        float max = std::numeric_limits<float>::min();
        float min = std::numeric_limits<float>::max();
        for (int i = 0; i < arr.size(); i++) {
            float data = arr.getUnchecked(i);
            if (data < min) {
                min = data;
            }
            if (data > max) {
                max = data;
            }
        }
        return std::pair<float, float>{min, max};
    }

}