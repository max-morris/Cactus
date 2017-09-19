#include <cctk.h>
#include <cctk_Schedule.h>
#include <set>
#include <sstream>

namespace cctki {

cFunctionData *func;

struct EntryComp {
    bool operator()(const RDWR_entry& v1,const RDWR_entry& v2) const {
        if(v1.var_id < v2.var_id) return true;
        if(v1.var_id > v2.var_id) return false;
        if(v1.time_level < v2.time_level) return true;
        return false;
    }
};

inline void tolower(std::string& s) {
  for(auto si=s.begin();si != s.end();++si) {
    if(*si >= 'A' && *si <= 'Z') {
      *si = *si + 'a' - 'A';
    }
  }
}

void add_entry(int vi,int tl,bool is_write,int where,std::set<RDWR_entry,EntryComp>& s) {
    RDWR_entry entry;
    entry.var_id = vi;
    entry.time_level = tl;
    auto iter = s.find(entry);
    if(iter == s.end()) {
        entry.where_wr = 0;
        entry.where_rd = 0;
    } else {
        if(is_write && iter->where_wr != 0) {
            std::ostringstream msg;
            msg << "Duplicate write specification for " << CCTK_FullName(vi) << " in function " << func->routine << std::endl;
            CCTK_Error(-1,0,func->thorn,msg.str().c_str());
        }
        if(!is_write && iter->where_rd != 0) {
            std::ostringstream msg;
            msg << "Duplicate read specification for " << CCTK_FullName(vi) << " in function " << func->routine << std::endl;
            CCTK_Error(-1,0,func->thorn,msg.str().c_str());
        }
        entry.where_wr = iter->where_wr;
        entry.where_rd = iter->where_rd;
        s.erase(iter);
    }
    if(is_write) {
        entry.where_wr |= where;
    } else {
        entry.where_rd |= where;
    }
    s.insert(entry);
}

/**
 * The following parser assumes that all values for "str" will be
 * of the form IMPL::VAR_OR_GROUP(WHERE), where WHERE must be either everywhere,
 * interior, or boundary. The name IMPL refers to a thorn or implemenation name,
 * and VAR_OR_GROUP refers to a variable or group name. In either case, a suffix
 * of _p indicates a past time level, i.e. "foo_p" refers to "foo" at time level 1.
 */
void parse(const char *str,bool is_write,std::set<RDWR_entry,EntryComp>& s) {
    std::string fstr{str};
    std::string imp;
    std::string var;
    std::string where;
    int tl=0;
    unsigned int i=0;
    for(;i<fstr.size();++i) {
        if(fstr[i] == ':')
          break;
        imp += fstr[i];
    }
    i += 2;
    for(;i<fstr.size();++i) {
        if(fstr[i] == '(')
          break;
        var += fstr[i];
    }
    i += 1;
    for(;i<fstr.size();++i) {
        if(fstr[i] == ')')
          break;
        where += fstr[i];
    }
    int n = var.size()-2;
    while(var[n]=='_' && var[n+1]=='p') {
      tl ++;
      n -= 2;
    }
    tolower(where);
    int wh = 0;
    if(where == "everywhere")
        wh = WH_EVERYWHERE;
    else if(where == "interior")
        wh = WH_INTERIOR;
    else if(where == "boundary")
        wh = WH_BOUNDARY;
    else {
        std::ostringstream msg;
        msg << "Invalid where specification '" << where << "'" << std::endl;
        CCTK_Error(-1,0,imp.c_str(),msg.str().c_str());
    }
    var.resize(n+2);
    std::string full_name = imp + "::" + var;
    int vi = CCTK_VarIndex(full_name.c_str());
    if(vi < 0) {
        int gi = CCTK_GroupIndex(full_name.c_str());
/*        if(gi < 0 and !strcmp(var,test_parameter)) {
            std::ostringstream msg;
            msg << "Invalid variable or group name " << full_name << std::endl;
            CCTK_Error(-1,0,imp.c_str(),msg.str().c_str());
        }*/
        int i0 = CCTK_FirstVarIndexI(gi);
        int iN = i0+CCTK_NumVarsInGroupI(gi);
        for(vi=i0;vi<iN;vi++) {
            add_entry(vi,tl,is_write,wh,s);
        }
    } else {
        add_entry(vi,tl,is_write,wh,s);
    }
}

extern "C"
void CCTKi_CreateRDWRData(cFunctionData *f)
{
    std::set<RDWR_entry,EntryComp> s;
    func = f;

    for(int i=0;i<f->n_WritesClauses;i++) {
        parse(f->WritesClauses[i],true,s);
    }

    for(int i=0;i<f->n_ReadsClauses;i++) {
        parse(f->ReadsClauses[i],false,s);
    }

    f->n_RDWR = s.size();
    f->RDWR = new RDWR_entry[s.size()];
    int n = 0;
    for(auto i=s.begin();i != s.end();++i)
        f->RDWR[n++] = *i;
}

extern "C"
void CCTKi_FreeRDWRData(cFunctionData *f)
{
    delete[] f->RDWR;
    f->RDWR = nullptr;
    f->n_RDWR = 0;
}
}
