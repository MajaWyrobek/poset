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
        // warunki bycia częściowym porządkiem: 
        // zwrotność (sam ze sobą) - jest (chyba?)
        // przechodność ((x, y) i (y, z) => (x, z))
        // antysymetryczność (nie zachodzi jednocześnie dla (x, y) i (y, x)) - nie ma jak się popsuć przez usuwanie?
        // sprawdzanie, czy nie zaburzy to przechodniości
        // czy w add nie może być tak, że zaburzy się antysymetryczność?
        if (!poset_test(id, value1, value2) || (strcmp(value1, value2) == 0)) {
            return false;
        }

        // trzeba sprawdzić, czy po usunięciu ścieżki z value1 do value2 nadal da się jakoś przejść od jednego do drugiego
        // pierdyknąć bfs i powinno pójść4
        set<pair<string, string>> container = std::get<1>(posets.at(id));
        pair<string, string> order(value1, value2);
        container.erase(order); // usuwamy żeby nie przeszkadzało, jeśli się zepsuje to dodamy z powrotem   

        std::queue<string> q;
        string temp;

        q.push(value1);

        while(!q.empty()) {
            temp = q.front();
            q.pop();

            if (temp == value2) {
                container.insert(order);
                return false;
            }

            // znajdź te do kótrych można dojść (first to temp) i wepchnij w kolejkę
            for (auto i = container.begin(); i != container.end(); i++) {
                if (i -> first == temp) {
                    q.push(i -> first);
                    // jak oznaczać odwiedzone? wektor? i czy wg trzeba, skoro nie może być pętli?
                }
            }
        }

        return true;
    }

    bool poset_test(unsigned long id, char const* value1, char const* value2) {
        // tu brakuje case, że element jest sam ze sobą w relacji;
        // implementujemy to w funkcji, czy w secie?
        try {
            pair<string, string> order(value1, value2);

            if (std::get<0>(posets.at(id)).count(value1) == 0 ||
                std::get<0>(posets.at(id)).count(value2) == 0 ||
                std::get<1>(posets.at(id)).count(order) == 0) {
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