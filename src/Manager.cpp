#include "Manager.h"

namespace ClassProject{
    Manager::Manager() {
        // HIGH, LOW, TopVar
        // The BDD_ID of each entry is the index of the vector
        // The TopVarName is stored in another vector with the same BDD_ID

        // Create entries for leaf nodes
        uTable.push_back({0,0,0});
        topVarNameTable.emplace_back("false");
        uTable.push_back({1,1,1});
        topVarNameTable.emplace_back("true");
    }

    BDD_ID Manager::createVar(const std::string &label) {
        BDD_ID id = uniqueTableSize();
        uTable.push_back({1,0,id});
        topVarNameTable.emplace_back(label);
        return id;
    }

    const BDD_ID &Manager::True() {
        static const BDD_ID val = 1;
        return val;
    }

    const BDD_ID &Manager::False() {
        static const BDD_ID val = 0;
        return val;
    }

    bool Manager::isConstant(BDD_ID f) {
        return f == True() || f == False();
    }

    bool Manager::isVariable(BDD_ID x) {
        if (x == topVar(x) && !isConstant(x))
            return true;
        else
            return false;
    }

    BDD_ID Manager::topVar(BDD_ID f) {
        return uTable[f][VTOPVAR];
    }

    std::string Manager::getTopVarName(const BDD_ID &root) {
        return topVarNameTable[root];
    }

    size_t Manager::uniqueTableSize() {
        return uTable.size();
    }

}
