#include "Piraha.hpp"

using namespace piraha;

bool NegLookAhead::match(Matcher *m) {
    int pos = m->pos;
    bool b = pattern->match(m);
    m->pos = pos;
    return !b;
}
