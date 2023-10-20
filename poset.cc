#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include "poset.h"

#define CHECK_FOR_NULLPTR(ptr) \
            if(ptr == NULL)    \
                return false;

using std::unordered_map;
using std::string;
using std::pair;
using std::set;
using std::vector;

/* Container Aliases */
using elements_container = set<string>;
using relation = pair<string, string>;
using relations_container = set<relation>;
using poset = pair<elements_container, relations_container>;

/* Global Variables */
static unordered_map<unsigned long, poset> posets;
static unsigned long next_id = 0;

/* Library Functions */
extern "C"
{
    unsigned long poset_new()
    {
        poset poset;
        unsigned long poset_id = next_id;
        next_id++;
        posets[poset_id] = poset;
        return poset_id;
    }

    void poset_delete(unsigned long id)
    {
        posets.erase(id);
    }

    size_t poset_size(unsigned long id)
    {
        size_t size;
        try
        {
            size = std::get<0>(posets.at(id)).size();
        }
        //If a poset with a given id doesn't exist in posets,
        //at will throw an out_of_range exception.
        catch (std::exception& e)
        {
            size = 0;
        }
        return size;
    }

    bool poset_insert(unsigned long id, char const* value)
    {
        CHECK_FOR_NULLPTR(value)
        try
        {
            poset poset = posets.at(id);
            elements_container elements = std::get<0>(poset);
            relations_container relations = std::get<1>(poset);
            string new_element(value);//makes a deep copy
            relation new_relation = std::make_pair(new_element, new_element);

            if(elements.count(new_element) != 0)
            {
                return false;
            }
            elements.insert(new_element);
            relations.insert(new_relation);
            return true;
        }
        catch (std::exception& e)
        {
            return false;
        }
    }

    bool poset_remove(unsigned long id, char const* value)
    {
        CHECK_FOR_NULLPTR(value)
        try
        {
            poset poset = posets.at(id);
            elements_container elements = std::get<0>(poset);
            relations_container relations = std::get<1>(poset);
            string element_to_remove(value);
            vector<relation> relations_to_remove;

            //erase returns 0 if nothing was removed
            if(elements.erase(element_to_remove) == 0)
            {
                return false;
            }
            for(const auto& relation : relations)
            {
                if(element_to_remove == std::get<0>(relation) ||
                   element_to_remove == std::get<1>(relation))
                {
                    relations_to_remove.push_back(relation);
                }
            }
            for(const auto& relation : relations_to_remove)
            {
                relations.erase(relation);
            }
            return true;
        }
        catch (std::exception& e)
        {
            return false;
        }
    }
}

int main() {
    return 0;
}