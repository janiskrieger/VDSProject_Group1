#include "Manager.h"

namespace ClassProject{
    Manager::Manager() {
        // HIGH, LOW, TopVar, TopVarName
        // The BDD_ID of each entry is the index of the vector

        // Create entries for leaf nodes
        uTable.push_back({0,0,0});
        topVarNameTable.emplace_back("false");
        uTable.push_back({1,1,1});
        topVarNameTable.emplace_back("true");
    }

    const BDD_ID &Manager::True() {
        static const BDD_ID val = 1;
        return val;
    }

    const BDD_ID &Manager::False() {
        static const BDD_ID val = 0;
        return val;
    }
}
