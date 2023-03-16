#include "Piraha.hpp"

#include <iostream>

using namespace cctki_piraha;

Seq::Seq(std::initializer_list<Pattern *> patterns_) {
    for (auto p: patterns_)
        patterns.emplace_back(p);
}
Seq::Seq(std::vector<std::shared_ptr<Pattern> > p,bool ign,bool show) : patterns(p) {}

bool Seq::match(std::shared_ptr<Matcher> m) {
    typedef std::vector<std::shared_ptr<Pattern> >::iterator pattern_iter;
    for(pattern_iter p = patterns.begin();p != patterns.end();++p) {
        if(!(*p)->match(m))
            return false;
        if(m->max_pos == m->match_to)
            return true;
    }
    return true;
}
