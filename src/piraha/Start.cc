#include "Piraha.hpp"

using namespace piraha;

bool Start::match(Matcher *m) {
    return m->pos == 0;
}
