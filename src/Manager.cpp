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
#ifdef DEBUG
        std::cout << std::endl << "Created new variable " + label + " with ID: " << id;
#endif
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

    BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
#ifdef DEBUG
        std::cout << std::endl << "ITE(" << i << ", " << t << ", " << e << ")";
#endif
        if (isConstant(i) || isVariable(i)){   // terminal case
            return i;
        } /*else if (computed table has entry for (f, g, h)){
            return i;   // eliminate repetitions in computations
        }*/
        else{
            BDD_ID rhigh = ite(coFactorTrue(i), coFactorTrue(t), coFactorTrue(e));
            BDD_ID rlow = ite(coFactorFalse(i), coFactorFalse(t), coFactorFalse(e));
            if (rhigh == rlow){ // reduction is possible
                return rhigh;
            }
            // Find or add unique table and eliminate isomorphic sub-graphs
            ClassProject::BDD_ID r = -1;
            for (BDD_ID id=0; id < uniqueTableSize(); id++){
                if (uTable[id][VTOPVAR] == topVar(i) && uTable[id][VHIGH] == t && uTable[id][VLOW] == e){
                    r = id;
#ifdef DEBUG
                    std::cout << std::endl << "Found existing entry in unique table for ID: " << id;
#endif
                    break;
                }
            }
            if (r == -1){   // no entry was found
                r = uniqueTableSize();
                uTable.push_back({i,t,e});
#ifdef DEBUG
                std::cout << std::endl << "Adding new entry to unique table with ID: " << r;
#endif
            }

            //update_computed_table((f, g, h), r);
            return r;
        }
    }

    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
#ifdef DEBUG
      std::cout << std::endl << "coFactorTrue(" << f << ", " << x << ")";
#endif
        if (isConstant(f) || isConstant(x) || topVar(f) > x){    // terminal case
            return f;
        }
        if (topVar(f) == x){
            return uTable[f][VHIGH];
        }
        else{
            BDD_ID T = coFactorTrue(uTable[f][VHIGH], x);
            BDD_ID F = coFactorTrue(uTable[f][VLOW], x);
            return ite(topVar(f), T, F);
        }
    }

    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
#ifdef DEBUG
        std::cout << std::endl << "coFactorFalse(" << f << ", " << x << ")";
#endif
        if (isConstant(f) || isConstant(x) || topVar(f) > x){    // terminal case
            return f;
        }
        if (topVar(f) == x){
            return uTable[f][VLOW];
        }
        else{
            BDD_ID T = coFactorFalse(uTable[f][VHIGH], x);
            BDD_ID F = coFactorFalse(uTable[f][VLOW], x);
            return ite(topVar(f), T, F);
        }
    }

    BDD_ID Manager::coFactorTrue(BDD_ID f) {
        return coFactorTrue(f, topVar(f));
    }

    BDD_ID Manager::coFactorFalse(BDD_ID f) {
        return coFactorFalse(f, topVar(f));
    }

    BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
        return ite(a,b,0);
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
