#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <string.h>
#include <queue>
#include "poset.h"

#define CHECK_FOR_NULLPTR(ptr) \
            if(ptr == NULL)    \
                return false;

using std::unordered_map;
using std::string;
using std::pair;
using std::set;

/* Container Aliases */
using elements_container = set<string>;
using relation = pair<string, string>;
using relations_container = set<relation>;
using poset = pair<elements_container, relations_container>;

/* Global Variables */
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
            size = posets.at(id).first.size();
        }
        catch (std::exception &e) {
            size = 0;
        }
        return size;
    }

    bool poset_del(unsigned long id, char const *value1, char const *value2) {
        if (!poset_test(id, value1, value2) || (strcmp(value1, value2) == 0)) {
            return false;
        }

        relations_container relations = posets.at(id).second;
        string less = value1;
        string greater = value2;
        relation order(less, greater);
        relations.erase(order); // usuwamy żeby nie przeszkadzało, jeśli się zepsuje to dodamy z powrotem   

        for (auto i = relations.cbegin(); i != relations.cend(); i++) {
            if (i -> second == greater) {
                string temp_less = i -> first;
                auto j = relations.cbegin();
                while (j != relations.cend()) {
                    if (j -> second == temp_less && j -> first == less) {
                        relations.insert(order);
                        return false;
                    }
                    j++;
                }
            }
        }

        return true;
    }

    bool poset_test(unsigned long id, char const* value1, char const* value2) {
        try {
            relation order(value1, value2);

            if (value1 == value2) {
                return true;
            }
            else if (posets.at(id).first.count(value1) == 0 ||
                posets.at(id).first.count(value2) == 0 ||
                posets.at(id).second.count(order) == 0) {
                return false;
            }
            else {
                return true;
            }
        }
        catch (std::exception &e) {
            return false;
        }
    }

    void poset_clear(unsigned long id) {
        if (posets.erase(id)) {
            poset poset;
            unsigned long poset_id = id;
            posets[poset_id] = poset;
        }
        else {
            // tutaj co jak nie pyknie
        } 
    }

}

int main() {
    return 0;
}