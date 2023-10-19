#include <iostream>
#include <unordered_map>
#include <vector>
#include "poset.h"

using std::unordered_map;
using std::string;
using std::vector;

static unordered_map<unsigned long, unordered_map<string, vector<unsigned long>>> posets;
static unsigned long next_id = 0;

extern "C" {
    unsigned long poset_new() {
        unordered_map<string, vector<unsigned long>> poset;
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
        catch (std::out_of_range &e) {
            size = 0;
        }
        return size;
    }
}

int main() {
    return 0;
}