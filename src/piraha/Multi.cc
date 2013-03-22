#include <iostream>
#include "Piraha.hpp"

using namespace piraha;

bool Multi::match(Matcher *m) {
    unsigned int save;
    int iter = 0;
    vector<smart_ptr<Group> > chSave;
    while(true) {
        save = m->pos;
        chSave = m->children;
        if(!pattern->match(m)) {
            m->children = chSave;
            m->pos = save;
            break;
        }
        if(save - m->pos == 0) {
            std::cout << "ZERO ADVANCE IN MULTI!" << std::endl;
            return minv <= iter && iter <= maxv;
        }
        iter++;
        if(iter == maxv)
            break;
    }
    return minv <= iter && iter <= maxv;
}
