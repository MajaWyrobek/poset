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

    bool poset_del(unsigned long id, char const *value1, char const *value2) {

    }

    bool poset_test(unsigned long id, char const* value1, char const* value2) {
        try {
            pair<string, string> order(value1, value2);
            if (std::get<1>(posets.at(id)).find(order) != std::get<1>(posets.at(id)).end()) {
                return true;
            }
            else {
                return false;
            }
        }
        catch (std::exception &e) {
            return false;
        }
    }

    void poset_clear(unsigned long id) {
        try {
            posets.erase(id);
            pair<set<string>, set<pair<string, string>>> poset;
            unsigned long poset_id = id;
            posets[poset_id] = poset;
        }
        catch (std::exception &e) {
            std::cerr << "poset_clear: poset " << id << " does not exist\n";
        }
    }

}

int main() {
    return 0;
}