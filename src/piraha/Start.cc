#include "Piraha.hpp"

using namespace cctki_piraha;

bool Start::match(std::shared_ptr<Matcher> m) {
    return m->pos == 0;
}
