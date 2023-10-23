#ifdef NDEBUG
  bool constexpr debug = false;
#else
  bool constexpr debug = true;
#endif


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

namespace {
    /* Global Variables */
    static unordered_map<unsigned long, poset> posets;
    static unsigned long next_poset_id = 0;
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

    static bool poset_find(unsigned long id, char const* value1, 
                           char const* value2, const string& func) 
    {
        try {
            relation order(value1, value2);

            if (posets.at(id).first.count(value1) == 0) {
                //-----------------------------
                if constexpr (debug) 
                message::about_element(func, id, value1, "does not exist");
                //-----------------------------
                return false;
            }
            else if (posets.at(id).first.count(value2) == 0) {
                //-----------------------------
                if constexpr (debug) 
                message::about_element(func, id, value2, "does not exist");
                //-----------------------------
                return false;
            }
            else if (posets.at(id).second.count(order) == 0) {
                //-----------------------------
                if constexpr (debug) 
                message::about_relation(func, id, value1, value2, 
                                        "does not exist");
                //-----------------------------
                return false;
            }
            else {
                return true;
            }
        }
        catch (std::out_of_range &e) {
            //-----------------------------
            if constexpr (debug) 
                message::about_poset(func, id, "does not exist");
            //-----------------------------
            return false;
        }
    }

/* Main functions */
    unsigned long poset_new()
    {
        //-----------------------------
        if constexpr (debug) message::function_start(__func__);
        //-----------------------------
        poset poset;
        unsigned long poset_id = next_poset_id;
        next_poset_id++;
        posets[poset_id] = poset;
        //-----------------------------
        if constexpr (debug) 
            message::about_poset(__func__, poset_id, "created");
        //-----------------------------
        return poset_id;
    }

    void poset_delete(unsigned long id)
    {
        //-----------------------------
        if constexpr (debug) message::function_start(__func__, id);
        //-----------------------------
        if(posets.erase(id) == 0) {
            //-----------------------------
            if constexpr (debug) 
                message::about_poset(__func__, id, "does not exist");
            //-----------------------------
        }
        else
        {
            //-----------------------------
            if constexpr (debug) message::about_poset(__func__, id, "deleted");
            //-----------------------------
        }
    }

    size_t poset_size(unsigned long id)
    {
        //-----------------------------
        if constexpr (debug) message::function_start(__func__, id);
        //-----------------------------
        try
        {
            //If a poset with a given id doesn't exist in posets,
            //at will throw an out_of_range exception.
            size_t size = posets.at(id).first.size();
            //-----------------------------
            if constexpr (debug) message::about_poset_size(__func__, id, size);
            //-----------------------------
            return size;
        }
        catch (std::out_of_range& e)
        {
            //-----------------------------
            if constexpr (debug) 
                message::about_poset(__func__, id, "does not exist");
            //-----------------------------
            return 0;
        }
    }

    bool poset_del(unsigned long id, char const *value1, char const *value2) 
    {
        //-----------------------------
        if constexpr (debug) 
        message::function_start(__func__, id, value1, value2);
        //-----------------------------
        if (strcmp(value1, value2) == 0) {
            return false;
        }

        if (!poset_find(id, value1, value2, __func__)) {
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

    bool poset_test(unsigned long id, char const* value1, char const* value2) 
    {
        //-----------------------------
        if constexpr (debug) 
        message::function_start(__func__, id, value1, value2);
        //-----------------------------
        if (value1 == value2) {
            return true;
        }
        else if (poset_find(id, value1, value2, __func__)) {
            return true;
        }
        else {
            return false;
        }
    }

    void poset_clear(unsigned long id) 
    {
        //-----------------------------
        if constexpr (debug) message::function_start(__func__, id);
        //-----------------------------
        if (posets.erase(id)) {
            poset poset;
            unsigned long poset_id = id;
            posets[poset_id] = poset;
        }
        else {
            // tutaj co jak nie pyknie
        } 
    }

int main() {
    return 0;
}