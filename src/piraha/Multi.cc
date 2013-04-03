#include <iostream>
#include "Piraha.hpp"

using namespace piraha;

bool Multi::match(Matcher *m) {
    vector<smart_ptr<Group> > chSave;
    for(int i=0;i<maxv;i++) {
        int save = m->pos;
        chSave = m->children;
        if(!pattern->match(m) || m->pos == save) {
            m->pos = save;
            m->children = chSave;
            return i >= minv;
        }
    }
    return true;
}
