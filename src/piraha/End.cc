#include "Piraha.hpp"

using namespace piraha;

bool End::match(Matcher *m) {
    return m->pos == (int)m->input_size;
}
