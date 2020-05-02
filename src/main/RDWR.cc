#include "cctk.h"
#include "cctk_Schedule.h"
#include "cctk_Parameters.h"

#include <cstdio>
#include <cstring>
#include <set>
#include <sstream>
#include <iostream>

namespace cctki {

enum rdwr_t { reads_t, writes_t, invalidates_t };

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

void add_entry(int vi,int tl,rdwr_t rdwr,int where,std::set<RDWR_entry,EntryComp>& s) {
    RDWR_entry entry;
    entry.var_id = vi;
    entry.time_level = tl;
    auto iter = s.find(entry);
    if(iter == s.end()) {
        entry.where_wr = 0;
        entry.where_rd = 0;
        entry.where_inv = 0;
    } else {
        if(rdwr == writes_t && iter->where_wr != WH_NOWHERE) {
            CCTK_VError(__LINE__,__FILE__,"Cactus",
                        "Duplicate write specification for %s in function %s::%s",
                        CCTK_FullVarName(vi),func->thorn,func->routine);
        }
        if(rdwr == reads_t && iter->where_rd != WH_NOWHERE) {
            CCTK_VError(__LINE__,__FILE__,"Cactus",
                        "Duplicate reads specification for %s in function %s::%s",
                        CCTK_FullVarName(vi),func->thorn,func->routine);
        }
        if(rdwr == invalidates_t && iter->where_inv != WH_NOWHERE) {
            CCTK_VError(__LINE__,__FILE__,"Cactus",
                        "Duplicate invalidates specification for %s in function %s::%s",
                        CCTK_FullVarName(vi),func->thorn,func->routine);
        }
        entry.where_wr = iter->where_wr;
        entry.where_rd = iter->where_rd;
        entry.where_inv = iter->where_inv;
        s.erase(iter);
    }
    if(rdwr == writes_t) {
        entry.where_wr |= where;
    } else if(rdwr == reads_t) {
        entry.where_rd |= where;
    } else {
        entry.where_inv |= where;
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
void parse(const char *str,rdwr_t rdwr,std::set<RDWR_entry,EntryComp>& s) {
    DECLARE_CCTK_PARAMETERS;

    const char* rdwr_s = rdwr == reads_t ? "READS" : rdwr == writes_t ?
                                          "WRITES" : "INVALIDATES";

    char varbuf[256], where[256];
    int vecnum = -1;
    if(sscanf(str, "%256[^[][%d](%256[^)])", varbuf, &vecnum, where) != 3 and
       sscanf(str, "%256[^(](%256[^)])", varbuf, where) != 2) {
        CCTK_VError(__LINE__,__FILE__,"Cactus",
                    "Could not parse specification '%s' when parsing %s statement in schedule for %s::%s",
                    str,rdwr_s,func->thorn,func->routine);
    }

    // strip off _p's and compute timelevel
    int tl = 0;
    for(int pos = strlen(varbuf)-2 ; pos >= 0 ; pos -= 2) {
      if(varbuf[pos] == '_' && varbuf[pos+1] == 'p') {
        tl += 1;
        varbuf[pos] = '\0';
      } else {
        break;
      }
    }

    // re-add vector index
    char fullvar[300];
    if(vecnum >= 0) {
      const size_t written = snprintf(fullvar, sizeof(fullvar), "%s[%d]", varbuf, vecnum);
      assert(written < sizeof(fullvar));
    } else {
      const size_t written = snprintf(fullvar, sizeof(fullvar), "%s", varbuf);
      assert(written < sizeof(fullvar));
    }

    // decode where
    int wh = -1;
    if(CCTK_EQUALS(where,"everywhere") || CCTK_EQUALS(where,"all"))
        wh = WH_EVERYWHERE;
    else if(CCTK_EQUALS(where,"interior") || CCTK_EQUALS(where,"in"))
        wh = WH_INTERIOR;
    else if(CCTK_EQUALS(where,"interiorwithboundary"))
        wh = WH_INTERIOR | WH_BOUNDARY;
    else if(CCTK_EQUALS(where,"boundary"))
        wh = WH_BOUNDARY;
    else {
        CCTK_VError(__LINE__, __FILE__, "Cactus",
                    "Invalid where specification '%s' while parsing %s statement  '%s' in schedule for %s::%s",
                    where,rdwr_s,str,func->thorn,func->routine);
    }
    assert(wh != -1);

    const int vi = CCTK_VarIndex(fullvar);
    if(vi >= 0) {
        add_entry(vi,tl,rdwr,wh,s);
    } else {
        const int gi = CCTK_GroupIndex(fullvar);
        if(gi >= 0) {
            int i0 = CCTK_FirstVarIndexI(gi);
            int iN = i0+CCTK_NumVarsInGroupI(gi);
            for(int vi=i0;vi<iN;vi++) {
                add_entry(vi,tl,rdwr,wh,s);
            }
        } else if(use_psync) {
            CCTK_VError(__LINE__, __FILE__, "Cactus",
                        "Invalid variable or group name '%s' in %s for routine %s::%s",
                        fullvar,rdwr_s,func->thorn,func->routine);
        }
    }
}

extern "C"
void CCTKi_CreateRDWRData(cFunctionData *f)
{
    std::set<RDWR_entry,EntryComp> s;
    func = f;

    for(int i=0;i<f->n_WritesClauses;i++) {
        parse(f->WritesClauses[i],writes_t,s);
    }

    for(int i=0;i<f->n_ReadsClauses;i++) {
        parse(f->ReadsClauses[i],reads_t,s);
    }

    for(int i=0;i<f->n_InvalidatesClauses;i++) {
        parse(f->InvalidatesClauses[i],invalidates_t,s);
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
