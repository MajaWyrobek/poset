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
using std::vector;

using elements_container = set<string>;
using relation = pair<string, string>;
using relations_container = set<relation>;
using poset = pair<elements_container, relations_container>;

static unordered_map<unsigned long, poset> posets;
static unsigned long next_id = 0;

extern "C" {
    unsigned long poset_new() {
        poset poset;
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

    bool poset_insert(unsigned long id, char const* value) {
        CHECK_FOR_NULLPTR(value)
        try {
            poset poset = posets.at(id);
            elements_container poset_elements = std::get<0>(poset);
            relations_container poset_relations = std::get<1>(poset);
            string new_element(value);//makes a deep copy
            relation new_relation = std::make_pair(value, value);

            if(poset_elements.count(new_element) != 0) {
                return false;
            }
            poset_elements.insert(new_element);
            poset_relations.insert(new_relation);
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