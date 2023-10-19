#include <iostream>
#include <unordered_map>
#include <vector>
#include "poset.h"

#define CHECK_FOR_NULLPTR(ptr) \
            if(ptr == NULL)    \
                return false;

using std::unordered_map;
using std::string;
using std::vector;

static unordered_map<unsigned long, unordered_map<string, vector<string>>> posets;
static unsigned long next_id = 0;

extern "C" {
    unsigned long poset_new() {
        unordered_map<string, vector<string>> poset;
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
            size = posets.at(id).size();
        }
        catch (std::exception &e) {
            size = 0;
        }
        return size;
    }

    bool poset_insert(unsigned long id, char const* value) {
        CHECK_FOR_NULLPTR(value)
        try {
            unordered_map<string, vector<string>> poset = posets.at(id);
            string new_element(value);//makes a deep copy
            if(poset.count(new_element) != 0) {
                return false;
            }
            vector<string> relations;
            poset[new_element] = relations;
            return true;
        }
        catch (std::exception &e) {
            return false;
        }
    }
}

int main() {
    return 0;
}