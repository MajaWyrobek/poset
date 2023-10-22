#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <cassert>
#include "poset.h"

using std::unordered_map;
using std::string;
using std::pair;
using std::unordered_set;
using std::vector;
using std::make_pair;

//There is no hashing function for pairs in the standard library
struct pair_hush
{
    size_t operator()(const pair<unsigned long, unsigned long> &pair) const
    {
        size_t h1 = std::hash<unsigned long>{}(pair.first);
        size_t h2 = std::hash<unsigned long>{}(pair.second);
        h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        return h1;
    }
};

/* Container Aliases */
using elements_container = unordered_map<string, unsigned long>;
//unsigned longs will be quicker to compare than strings longer than 4
using relation = pair<unsigned long, unsigned long>;
using relations_container = unordered_set<relation, pair_hush>;
using poset = pair<elements_container, relations_container>;

/* Global Variables */
namespace
{
    unordered_map<unsigned long, poset> posets;
    unsigned long next_poset_id = 0;
    unsigned long next_element_id = 0;
}

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
        unsigned long element)
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
        for(auto i = relations.cbegin(); i != relations.cend(); i++)
        {
            for (auto j = relations.cbegin(); j != relations.cend(); j++)
            {
                if (j->first == i->second &&
                    relations.count(make_pair(i->first, j->second)) == 0)
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

namespace message
{
    static string print_value(const char* value)
    {
        if(value == nullptr)
        {
            return "NULL";
        }
        else
        {
            return "\"" + string(value) + "\"";
        }
    }

    static void function_start(const string& function_name)
    {
        std::cerr << function_name + "()" << std::endl;
    }

    static void function_start(const string& function_name, unsigned long id)
    {
        std::cerr << function_name + "(" +
                     std::to_string(id) +
                     ")" << std::endl;
    }

    static void function_start(const string& function_name, unsigned long id,
                               const char* value)
    {
        std::cerr << function_name + "(" +
                     std::to_string(id) +
                     ", " +
                     print_value(value) +
                     ")" << std::endl;
    }

    static void function_start(const string& function_name, unsigned long id,
                               const char* value1, const char* value2)
    {
        std::cerr << function_name + "(" +
                     std::to_string(id) +
                     ", " +
                     print_value(value1) +
                     ", " +
                     print_value(value2) +
                     ")" << std::endl;
    }

    static void about_poset(const string& function_name, unsigned long id,
                            const string& message)
    {
        std::cerr << function_name + ": poset " +
                     std::to_string(id) +
                     " " +
                     message << std::endl;
    }

    static void about_element(const string& function_name, unsigned long id,
                              const char* value,
                              const string& message)
    {
        std::cerr << function_name + ": poset " +
                     std::to_string(id) +
                     ", element " +
                     print_value(value) +
                     " " +
                     message << std::endl;
    }

    static void about_relation(const string& function_name, unsigned long id,
                              const char* value1, const char* value2,
                              const string& message)
    {
        std::cerr << function_name + ": poset " +
                     std::to_string(id) +
                     ", relation (" +
                     print_value(value1) +
                     ", " +
                     print_value(value2) +
                     ") " +
                     message << std::endl;
    }

    static void about_poset_size(const string& function_name, unsigned long id,
                                 size_t size)
    {
        std::cerr << function_name + ": poset " +
                     std::to_string(id) +
                     " contains " +
                     std::to_string(size) +
                     " element(s)" << std::endl;
    }

    static void invalid_value(const string& function_name, const char* value,
                              const string& value_descriptor)
    {
        std::cerr << function_name + ": invalid value" +
                     value_descriptor + " (" +
                     print_value(value) +
                     ")" << std::endl;
    }
}

/* Main Functions */
unsigned long poset_new()
{
    //-----------------------------
    message::function_start(__func__);
    //-----------------------------
    poset poset;
    unsigned long poset_id = next_poset_id;
    next_poset_id++;
    posets[poset_id] = poset;
    //-----------------------------
    message::about_poset(__func__, poset_id, "created");
    //-----------------------------
    return poset_id;
}

void poset_delete(unsigned long id)
{
    //-----------------------------
    message::function_start(__func__, id);
    //-----------------------------
    if(posets.erase(id) == 0) {
        //-----------------------------
        message::about_poset(__func__, id, "does not exist");
        //-----------------------------
    }
    else
    {
        //-----------------------------
        message::about_poset(__func__, id, "deleted");
        //-----------------------------
    }
}

size_t poset_size(unsigned long id)
{
    //-----------------------------
    message::function_start(__func__, id);
    //-----------------------------
    try
    {
        //If a poset with a given id doesn't exist in posets,
        //at will throw an out_of_range exception.
        size_t size = posets.at(id).first.size();
        //-----------------------------
        message::about_poset_size(__func__, id, size);
        //-----------------------------
        return size;
    }
    catch (std::out_of_range& e)
    {
        //-----------------------------
        message::about_poset(__func__, id, "does not exist");
        //-----------------------------
        return 0;
    }
}

bool poset_insert(unsigned long id, char const* value)
{
    //-----------------------------
    message::function_start(__func__, id, value);
    //-----------------------------
    if(value == nullptr) {
        //-----------------------------
        message::invalid_value(__func__, value, "");
        //-----------------------------
        return false;
    }
    try
    {
        poset& poset = posets.at(id);
        elements_container& elements = poset.first;
        string new_element(value);//Makes a deep copy.

        if(elements.count(new_element) != 0)
        {
            //-----------------------------
            message::about_element(__func__, id, value, "already exists");
            //-----------------------------
            return false;

        }
        //Relation of an element with itself is held implicitely.
        elements[new_element] = next_element_id++;
        //-----------------------------
        message::about_element(__func__, id, value, "inserted");
        //-----------------------------
        return true;
    }
    catch (std::out_of_range& e)
    {
        //-----------------------------
        message::about_poset(__func__, id, "does not exist");
        //-----------------------------
        return false;
    }
}

bool poset_remove(unsigned long id, char const* value)
{
    //-----------------------------
    message::function_start(__func__, id, value);
    //-----------------------------
    if(value == nullptr) {
        //-----------------------------
        message::invalid_value(__func__, value, "");
        //-----------------------------
        return false;
    }
    if(posets.count(id) == 0)
    {
        //-----------------------------
        message::about_poset(__func__, id, "does not exist");
        //-----------------------------
        return false;
    }
    poset& poset = posets.at(id);
    elements_container& elements = poset.first;
    relations_container& relations = poset.second;
    if(elements.count(value) == 0)
    {
        //-----------------------------
        message::about_element(__func__, id, value, "does not exist");
        //-----------------------------
        return false;
    }
    auto element_to_remove = elements.at(value);
    elements.erase(value);
    vector<relation> relations_to_remove =
            poset_relations_with_element(relations, element_to_remove);
    poset_remove_relations(relations, relations_to_remove);
    //-----------------------------
    message::about_element(__func__, id, value, "removed");
    //-----------------------------
    return true;
}

bool poset_add(unsigned long id, char const* value1, char const* value2)
{
    //-----------------------------
    message::function_start(__func__, id, value1, value2);
    //-----------------------------
    bool invalid_data = false;
    if(value1 == nullptr) {
        //-----------------------------
        message::invalid_value(__func__, value1, "1");
        //-----------------------------
        invalid_data = true;
    }
    if(value2 == nullptr) {
        //-----------------------------
        message::invalid_value(__func__, value2, "2");
        //-----------------------------
        invalid_data = true;
    }
    if(invalid_data) return false;

    if(posets.count(id) == 0)
    {
        //-----------------------------
        message::about_poset(__func__, id, "does not exist");
        //-----------------------------
        return false;
    }
    poset& poset = posets.at(id);
    elements_container& elements = poset.first;
    relations_container& relations = poset.second;
    if(elements.count(value1) == 0)
    {
        //-----------------------------
        message::about_element(__func__, id, value1, "does not exist");
        //-----------------------------
        return false;
    }
    auto element1 = elements.at(value1);
    if(elements.count(value2) == 0)
    {
        //-----------------------------
        message::about_element(__func__, id, value2, "does not exist");
        //-----------------------------
        return false;
    }
    auto element2 = elements.at(value2);
    vector<relation> added_relations;

    if(element1 == element2 ||
       relations.count(make_pair(element1, element2)) != 0)
    {
        //-----------------------------
        message::about_relation(__func__, id, value1, value2,
                                "already exists");
        //-----------------------------
        return false;
    }
    if(relations.count(make_pair(element2, element1)) != 0)
    {
        //-----------------------------
        message::about_relation(__func__, id, value1, value2,
                                "cannot be added");
        //-----------------------------
        return false;
    }
    relations.insert(make_pair(element1, element2));
    added_relations.emplace_back(element1, element2);
    if(!poset_transitive_closure(relations, added_relations))
    {
        poset_remove_relations(relations, added_relations);
        //-----------------------------
        message::about_relation(__func__, id, value1, value2,
                                "cannot be added");
        //-----------------------------
        return false;
    }
    //-----------------------------
    message::about_relation(__func__, id, value1, value2, "added");
    //-----------------------------
    return true;
}

int main() {
    unsigned long p1;
    p1 = poset_new();
    assert(poset_size(p1) == 0);
    assert(poset_size(p1 + 1) == 0);
    assert(!poset_insert(p1, NULL));
    assert(poset_insert(p1, "A"));
    assert(!poset_insert(p1, "A"));
    assert(!poset_insert(p1 + 1, "B"));
    assert(poset_size(p1) == 1);
    assert(!poset_remove(p1 + 1, "A"));
    assert(poset_remove(p1, "A"));
    assert(!poset_remove(p1, "A"));
    assert(poset_insert(p1, "B"));
    assert(poset_insert(p1, "C"));
    assert(poset_add(p1, "B", "C"));
    assert(!poset_remove(p1, "A"));
    assert(!poset_add(p1, NULL, "X"));
    assert(!poset_add(p1, "X", NULL));
    assert(!poset_add(p1, NULL, NULL));
    assert(!poset_add(p1, "C", "D"));
    assert(!poset_add(p1, "D", "C"));
    assert(!poset_add(p1, "D", "D"));
    assert(!poset_add(p1, "E", "D"));
    assert(!poset_add(p1 + 1, "C", "D"));
    return 0;
}