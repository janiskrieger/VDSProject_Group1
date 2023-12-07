#include "Manager.h"

namespace ClassProject {
    Manager::Manager() {
        // HIGH, LOW, TopVar
        // The BDD_ID of each entry is the index of the vector
        // The TopVarName is stored in another vector with the same BDD_ID

        // Create entries for leaf nodes
        uTable.push_back({0, 0, 0});
        topVarNameTable.emplace_back("false");
        uTable.push_back({1, 1, 1});
        topVarNameTable.emplace_back("true");
#ifdef DEBUG
        std::cout << std::endl << "uTable: 0, 0, 0 ,0, true";
        std::cout << std::endl << "uTable: 1, 1, 1 ,1, false";
#endif
    }

    BDD_ID Manager::createVar(const std::string &label) {
        BDD_ID id = uniqueTableSize();
        uTable.push_back({1, 0, id});
#ifdef DEBUG
        std::cout << std::endl << "uTable: " << id << ", 1, 0, " << id << ", " << label;
#endif
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

    BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
        // Terminal case of recursion (lecture slides 2-15)
        // ite(1, f, g) = ite(0, g, f) = ite(f, 1, 0) = ite(g, f, f) = f
        if (i == True()) {
            return t;
        } else if (i == False()) {
            return e;
        } else if (t == True() and e == False()) {
            return i;
        } else if (t == e) {
            return t;
        } /*else if (computed table has entry for (f, g, h)){
            return i;   // eliminate repetitions in computations
        }*/
        else {
            // let x be the top-variable of (i, t, e)
            BDD_ID x = topVar(i);
            if (topVar(t) < x && isVariable(topVar(t)))
                x = topVar(t);
            if (topVar(e) < x && isVariable(topVar(e)))
                x = topVar(e);

            BDD_ID rhigh = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
            BDD_ID rlow = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));
            if (rhigh == rlow) { // reduction is possible
                return rhigh;
            }
            // Find or add unique table and eliminate isomorphic sub-graphs
            ClassProject::BDD_ID r = -1;
            for (BDD_ID id = 0; id < uniqueTableSize(); id++) {
                if (topVar(id) == topVar(i) && high(t) == t && low(id) == e) {
                    r = id;
                    break;
                }
            }
            if (r == -1) {   // no entry was found
                r = uniqueTableSize();
                uTable.push_back({rhigh, rlow, x});
#ifdef DEBUG
                std::cout << std::endl << "uTable: " << r << ", " << rhigh << ", " << rlow << ", " << x;
#endif
            }

            //update_computed_table((f, g, h), r);
            return r;
        }
    }

    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
        if (isConstant(f) || isConstant(x) || topVar(f) > x) {    // terminal case
            return f;
        }
        if (topVar(f) == x) {
            return high(f);
        } else {
            BDD_ID T = coFactorTrue(high(f), x);
            BDD_ID F = coFactorTrue(low(f), x);
            return ite(topVar(f), T, F);
        }
    }

    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
        if (isConstant(f) || isConstant(x) || topVar(f) > x) {    // terminal case
            return f;
        }
        if (topVar(f) == x) {
            return low(f);
        } else {
            BDD_ID T = coFactorFalse(high(f), x);
            BDD_ID F = coFactorFalse(low(f), x);
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
        return ite(a, b, 0);
    }

    BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
        return ite(a, 1, b);
    }

    BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
        return ite(a, neg(b), b);
    }

    BDD_ID Manager::neg(BDD_ID a) {
        return ite(a, 0, 1);
    }

    BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
        return ite(a, neg(b), 1);
    }

    BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
        return ite(a, 0, neg(b));
    }

    BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
        return ite(a, b, neg(b));
    }

    BDD_ID Manager::topVar(BDD_ID f) {
        return uTable[f][VTOPVAR];
    }

    BDD_ID Manager::low(BDD_ID f) {
        return uTable[f][VLOW];
    }

    BDD_ID Manager::high(BDD_ID f) {
        return uTable[f][VHIGH];
    }

    std::string Manager::getTopVarName(const BDD_ID &root) {
        return topVarNameTable[root];
    }

    void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
        nodes_of_root.emplace(root);    // inserts a new element only if the element is unique
        // ensure root node is not leaf node
        if (root > 1) {  //terminal case of recursion
            findNodes(high(root), nodes_of_root);
            findNodes(low(root), nodes_of_root);
        }
        return;
    }

    void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
        // Use findNodes to get all nodes reachable from root
        std::set<BDD_ID> nodes_of_root;
        findNodes(root, nodes_of_root);

        // Extract variables from the set of nodes and add them to vars_of_root
        for (const auto &node: nodes_of_root) {
            // Check if node is not leaf node
            if (node > 1) {
                vars_of_root.emplace(topVar(node));
            }
        }
        return;
    }

    size_t Manager::uniqueTableSize() {
        return uTable.size();
    }

    void Manager::visualizeBDD(std::string filepath, BDD_ID &root) {
        //  creates a file visualizing a digraph in the DOT language
        if (root > 1) {
            std::set<BDD_ID> vars;
            std::set<BDD_ID> nodes;
            findVars(root, vars);
            findNodes(root, nodes);

            // create and open a text file
            std::ofstream File(filepath);

            // write DOT programm
            File << "digraph D {\n";
            File << getTopVarName(topVar(0)) << " [shape=box]\n";
            File << getTopVarName(topVar(1)) << " [shape=box]\n";
            for (auto itr: vars) {
                File << getTopVarName(topVar(itr)) << " [shape=circle]\n";
            }
            for (auto itr: nodes) {
                if (!isConstant(itr)) {
                    File << getTopVarName(topVar(itr)) << " -> " << getTopVarName(topVar(high(itr)))<< "\n";
                    File << getTopVarName(topVar(itr)) << " -> " << getTopVarName(topVar(low(itr))) << " [style=dashed]\n";
                }
            }
            File << "}";
            // close file
            File.close();
        }
    }
}