#include "Piraha.hpp"

using namespace cctki_piraha;

bool End::match(std::shared_ptr<Matcher> m) {
    return m->pos == (int)m->input_size;
}
