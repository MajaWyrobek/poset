#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include "poset.h"

#define CHECK_FOR_NULLPTR(ptr) \
            if(ptr == NULL)    \
                return false;

using std::unordered_map;
using std::string;
using std::pair;
using std::set;

static unordered_map<unsigned long, pair<set<string>, set<pair<string, string>>>> posets;
static unsigned long next_id = 0;

extern "C" {
    unsigned long poset_new() {
        pair<set<string>, set<pair<string, string>>> poset;
        unsigned long poset_id = next_id;
        next_id++;
        posets[poset_id] = poset;
        return poset_id;
    }

    void poset_delete(unsigned long id) {
        posets.erase(id);
    }

    size_t poset_size(unsigned long id) {
        size_t size;
        try {
            size = std::get<0>(posets.at(id)).size();
        }
        catch (std::exception &e) {
            size = 0;
        }
        return size;
    }

}

int main() {
    return 0;
}