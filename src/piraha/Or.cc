#include "Piraha.hpp"

#include <iostream>

using namespace cctki_piraha;

Or::Or(std::initializer_list<Pattern*> patterns_) {
    for (auto p: patterns_)
        patterns.emplace_back(p);
}

bool Or::match(std::shared_ptr<Matcher> m) {
    typedef std::vector<std::shared_ptr<Pattern> >::iterator pattern_iter;
    int save = m->pos;
    int chSave = m->children->size();
    for(pattern_iter p = patterns.begin();p != patterns.end();++p) {
        m->pos = save;
        m->children->resize(chSave);
        if((*p)->match(m))
            return true;
    }
    return false;
}
