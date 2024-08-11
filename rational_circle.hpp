#ifndef INCLUDED_RATIONAL_CIRCLE
#define INCLUDED_RATIONAL_CIRCLE

namespace theocad {
    
struct FIII {
    float a;
    int b, c, d;
};

extern const FIII rational_circle[];
FIII find_rational_angle(float angle);

};

#endif