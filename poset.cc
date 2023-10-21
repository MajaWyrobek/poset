#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>
#include "poset.h"

using std::unordered_map;
using std::string;
using std::pair;
using std::set;
using std::vector;
using std::make_pair;

#define CHECK_FOR_NULLPTR(ptr) \
            if(ptr == NULL)    \
                return false;

/* Container Aliases */
using elements_container = set<string>;
using relation = pair<const string&, const string&>;
using relations_container = set<relation>;
using poset = pair<elements_container, relations_container>;

/* Global Variables */
static unordered_map<unsigned long, poset> posets;
static unsigned long next_id = 0;

/* Auxiliary Functions */
static void poset_remove_relations(relations_container& relations,
                                   const vector<relation>& relations_to_remove)
{
    for(const auto& relation : relations_to_remove)
    {
        relations.erase(relation);
    }
}

static vector<relation> poset_relations_with_element(
        const relations_container& relations,
        const string& element)
{
    vector<relation> relations_with_element;
    for(const auto& relation : relations)
    {
        if(element == relation.first ||
           element == relation.second)
        {
            relations_with_element.push_back(relation);
        }
    }
    return relations_with_element;
}

static bool poset_transitive_closure(relations_container& relations,
                                     vector<relation>& added_relations)
{
    vector<relation> missing_relations;
    do
    {
        missing_relations.clear();
        for(auto i = relations.begin(); i != relations.end(); i++)
        {
            for (auto j = relations.begin(); j != relations.end(); j++)
            {
                if (j->first == i->second
                    && relations.count(make_pair(i->first, j->second)) == 0)
                {
                    if(relations.count(make_pair(j->second, i->first)) != 0)
                    {
                        return false;
                    }
                    missing_relations.emplace_back(i->first, j->second);
                }
            }
        }
        for (const relation& relation : missing_relations)
        {
            relations.insert(relation);
            added_relations.push_back(relation);
        }
    } while(!missing_relations.empty());
    return true;
}

/* Main Functions */
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
            //If a poset with a given id doesn't exist in posets,
            //at will throw an out_of_range exception.
            size = posets.at(id).first.size();
        }
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
            elements_container elements = poset.first;
            relations_container relations = poset.second;
            string new_element(value);//Makes a deep copy.

            if(elements.count(new_element) != 0)
            {
                return false;
            }
            //relation of an element with itself is held implicitely.
            elements.insert(new_element);
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
            elements_container elements = poset.first;
            relations_container relations = poset.second;
            string element_to_remove(value);
            vector<relation> relations_to_remove =
                    poset_relations_with_element(relations,element_to_remove);

            poset_remove_relations(relations, relations_to_remove);
            //Erase returns 0 if nothing was removed.
            if(elements.erase(element_to_remove) == 0)
            {
                return false;
            }
            return true;
        }
        catch (std::exception& e)
        {
            return false;
        }
    }

    bool poset_add(unsigned long id, char const* value1, char const* value2)
    {
        CHECK_FOR_NULLPTR(value1)
        CHECK_FOR_NULLPTR(value2)
        try
        {
            poset poset = posets.at(id);
            elements_container elements = poset.first;
            relations_container relations = poset.second;
            string element1(value1);
            string element2(value2);
            vector<relation> added_relations;

            if(elements.count(element1) == 0 ||
               elements.count(element2) == 0 ||
               relations.count(make_pair(element1, element2)) != 0 ||
               relations.count(make_pair(element2, element1)) != 0)
            {
                return false;
            }
            relations.insert(make_pair(element1, element2));
            if(!poset_transitive_closure(relations, added_relations))
            {
                poset_remove_relations(relations, added_relations);
                return false;
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