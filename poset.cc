#ifndef NDEBUG
    constexpr bool debug = true;
#else
    constexpr bool debug = false;
#endif

#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <cassert>
#include <stddef.h>
#include <cstddef>
#include "poset.h"

using std::unordered_map;
using std::string;
using std::pair;
using std::unordered_set;
using std::vector;

/* Container Aliases */
using elements_container = unordered_map<string, unsigned long>;
using relations_container = unordered_map<unsigned long, unordered_set<unsigned long>>;
using poset = pair<elements_container, relations_container>;

/* Global Variables */
namespace
{
    unordered_map<unsigned long, poset> posets;
    unsigned long next_poset_id = 0;
    unsigned long next_element_id = 0;
}

/* Auxiliary Functions */
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

    static void poset_size(const string& function_name, unsigned long id,
                           size_t size)
    {
        std::cerr << function_name + ": poset " +
                     std::to_string(id) +
                     " contains " +
                     std::to_string(size) +
                     " element(s)" << std::endl;
    }

    static void invalid_value(const string& function_name,
                              const string& value_descriptor)
    {
        std::cerr << function_name + ": invalid value" +
                     value_descriptor + " (NULL)" << std::endl;
    }
}

static void remove_relations_with_element(relations_container& relations,
                                          const unsigned long element)
{
    relations.erase(element);
    for(auto& i : relations)
    {
        i.second.erase(element);
    }
}

static void merge(unordered_set<unsigned long>& s1,
                  const unordered_set<unsigned long>& s2)
{
    for(const auto& i : s2)
    {
        if(s1.count(i) == 0)
        {
            s1.insert(i);
        }
    }
}

static bool transitive_closure_recursion(relations_container& new_relations,
                                         unsigned long element,
                                         unordered_map<unsigned long, bool>& done)
{
    if(done[element])
    {
        return true;
    }
    unordered_set<unsigned long> initial_relations = new_relations[element];
    initial_relations.erase(element);
    unordered_set<unsigned long> relations_to_add;
    for(const auto& i : initial_relations)
    {
        if(!transitive_closure_recursion(new_relations, i, done))
        {
            return false;
        }
        relations_to_add = new_relations[i];
        for(const auto& j : relations_to_add)
        {
            if(new_relations[j].count(element) != 0)
            {
                return false;
            }
        }
        merge(new_relations[element], relations_to_add);
        relations_to_add.clear();
    }
    done[element] = true;
    return true;
}

static bool transitive_closure(relations_container& relations)
{
    unordered_map<unsigned long, bool> done;
    for(const auto& i : relations)
    {
        done[i.first] = false;
    }
    relations_container new_relations = relations;
    for(const auto& i : relations)
    {
        if(!transitive_closure_recursion(new_relations, i.first, done))
        {
            return false;
        }
    }
    relations = new_relations;
    return true;
}

static bool valid_values(char const* value1, char const* value2,
                         const string& function_name)
{
    bool invalid_data = false;
    if (value1 == nullptr)
    {
        //-----------------------------
        message::invalid_value(function_name, "1");
        //-----------------------------
        invalid_data = true;
    }
    if (value2 == nullptr)
    {
        //-----------------------------
        message::invalid_value(function_name, "2");
        //-----------------------------
        invalid_data = true;
    }
    if (invalid_data) return false;

    return true;
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
    if(posets.erase(id) == 0)//Erase returns 0 if nothing was erased.
    {
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
    if(posets.count(id) == 0)
    {
        //-----------------------------
        message::about_poset(__func__, id, "does not exist");
        //-----------------------------
        return 0;
    }
    size_t size = posets.at(id).first.size();
    //-----------------------------
    message::poset_size(__func__, id, size);
    //-----------------------------
    return size;
}

bool poset_insert(unsigned long id, char const* value)
{
    //-----------------------------
    message::function_start(__func__, id, value);
    //-----------------------------
    if(value == nullptr) {
        //-----------------------------
        message::invalid_value(__func__, "");
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
    string new_element(value);//Makes a deep copy.
    unsigned long new_element_id;

    if(elements.count(new_element) != 0)
    {
        //-----------------------------
        message::about_element(__func__, id, value, "already exists");
        //-----------------------------
        return false;

    }
    new_element_id = next_element_id++;
    elements[new_element] = new_element_id;
    relations[new_element_id].insert(new_element_id);//Realtion with itself.
    //-----------------------------
    message::about_element(__func__, id, value, "inserted");
    //-----------------------------
    return true;
}

bool poset_remove(unsigned long id, char const* value)
{
    //-----------------------------
    message::function_start(__func__, id, value);
    //-----------------------------
    if(value == nullptr) {
        //-----------------------------
        message::invalid_value(__func__, "");
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
    remove_relations_with_element(relations, element_to_remove);
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
    if (!valid_values(value1, value2, __func__)) {
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

    if(relations[element1].count(element2) != 0 ||
       relations[element2].count(element1) != 0)
    {
        //-----------------------------
        message::about_relation(__func__, id, value1, value2,
                                "cannot be added");
        //-----------------------------
        return false;
    }
    relations[element1].insert(element2);
    if(!transitive_closure(relations))
    {
        relations[element1].erase(element2);
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

bool poset_del(unsigned long id, char const* value1, char const* value2) {
    //-----------------------------
    message::function_start(__func__, id, value1, value2);
    //-----------------------------
    if (!valid_values(value1, value2, __func__)) {
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
    if (element1 == element2 || relations[element1].count(element2) == 0) {
        //-----------------------------
        message::about_relation(__func__, id, value1, value2,
                                "cannot be deleted");
        //-----------------------------
        return false;
    }

    for(const auto& i : relations[element1])
    {
        if(i != element2 && i != element1 && relations[i].count(element2) != 0)
        {
            //-----------------------------
            message::about_relation(__func__, id, value1, value2,
                                    "cannot be deleted");
            //-----------------------------
            return false;
        }
    }
    relations[element1].erase(element2);
    //-----------------------------
    message::about_relation(__func__, id, value1, value2, "deleted");
    //-----------------------------
    return true;
}

bool poset_test(unsigned long id, char const* value1, char const* value2)
{
    //-----------------------------
    message::function_start(__func__, id, value1, value2);
    //-----------------------------
    if (!valid_values(value1, value2, __func__)) {
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

    if (relations[element1].count(element2) == 0) {
        //-----------------------------
        message::about_relation(__func__, id, value1, value2,
                                "does not exist");
        //-----------------------------
        return false;
    }
    //-----------------------------
    message::about_relation(__func__, id, value1, value2,"exists");
    //-----------------------------
    return true;
}

void poset_clear(unsigned long id)
{
    //-----------------------------
    message::function_start(__func__, id);
    //-----------------------------
    if(posets.count(id) == 0)
    {
        //-----------------------------
        message::about_poset(__func__, id, "does not exist");
        //-----------------------------
    }
    else
    {
        poset& poset = posets.at(id);
        poset.first.clear();
        poset.second.clear();
        //-----------------------------
        message::about_poset(__func__, id, "cleared");
        //-----------------------------
    }
}