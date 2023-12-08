#include "Manager.h"

namespace ClassProject {
    /**
     * Initializes unique table
     *
     * @param
     * @return
     */
    Manager::Manager() {
        // uTable order: HIGH, LOW, TopVar
        uTable.push_back({0, 0, 0});
        topVarNameTable.emplace_back("false");
        uTable.push_back({1, 1, 1});
        topVarNameTable.emplace_back("true");
    }

    BDD_ID Manager::createVar(const std::string &label) {
        BDD_ID id = uniqueTableSize();
        uTable.push_back({1, 0, id});
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

    /**
     * Implements the if-then-else algorithm.
     *
     * @param i if
     * @param t then
     * @param e else
     * @return Returns the existing or new node representing the ite expression
     */
    BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
        // terminal case of recursion  ite(1, f, g) = ite(0, g, f) = ite(f, 1, 0) = ite(g, f, f) = f
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

            BDD_ID high = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
            BDD_ID low = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));
            if (high == low) { // reduction is possible
                return high;
            }
            // find or add unique table and eliminate isomorphic sub-graphs
            ClassProject::BDD_ID r = -1;
            for (BDD_ID id = 0; id < uniqueTableSize(); id++) {
                if (topVar(id) == topVar(i) && highSuccessor(t) == t && lowSuccessor(id) == e) {
                    r = id;
                    break;
                }
            }
            if (r == -1) {   // no entry was found
                r = uniqueTableSize();
                uTable.push_back({high, low, x});
            }
            //update_computed_table((f, g, h), r);
            return r;
        }
    }

    /**
     * Returns the positive co-factor of the function f with regard to variable x.
     *
     * @param f Function represented by ID
     * @param x Co-factor variable
     * @return Positive co-factor
     */
    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
        if (isConstant(f) || isConstant(x) || topVar(f) > x) {    // terminal case
            return f;
        }
        if (topVar(f) == x) {
            return highSuccessor(f);
        } else {
            BDD_ID T = coFactorTrue(highSuccessor(f), x);
            BDD_ID F = coFactorTrue(lowSuccessor(f), x);
            return ite(topVar(f), T, F);
        }
    }

    /**
     * Returns the negative co-factor of the function f with regard to variable x.
     *
     * @param f Function represented by ID
     * @param x Co-factor variable
     * @return Negative co-factor
     */
    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
        if (isConstant(f) || isConstant(x) || topVar(f) > x) {    // terminal case
            return f;
        }
        if (topVar(f) == x) {
            return lowSuccessor(f);
        } else {
            BDD_ID T = coFactorFalse(highSuccessor(f), x);
            BDD_ID F = coFactorFalse(lowSuccessor(f), x);
            return ite(topVar(f), T, F);
        }
    }

    /**
     * Returns the positive co-factor of the function f with regard to its top variable.
     *
     * @param f Function represented by ID
     * @return Positive co-factor
     */
    BDD_ID Manager::coFactorTrue(BDD_ID f) {
        return coFactorTrue(f, topVar(f));
    }

    /**
     * Returns the negative co-factor of the function f with regard to its top variable.
     *
     * @param f Function represented by ID
     * @return Negative co-factor
     */
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

    /**
     * Returns the ID of the top variable.
     *
     * @param f
     * @return ID of the top variable
     */
    BDD_ID Manager::topVar(BDD_ID f) {
        return uTable[f][2];
    }

    /**
     * Returns the ID of the low successor
     *
     * @param f
     * @return ID of the low successor
     */
    BDD_ID Manager::lowSuccessor(BDD_ID f) {
        return uTable[f][1];
    }

    /**
     * Returns the ID of the high successor
     *
     * @param f
     * @return ID of the high successor
     */
    BDD_ID Manager::highSuccessor(BDD_ID f) {
        return uTable[f][0];
    }

    std::string Manager::getTopVarName(const BDD_ID &root) {
        return topVarNameTable[root];
    }

    /**
     * This function takes a node root and an empty set nodes of root. It returns the set of all nodes which are
     * reachable from root including itself.
     *
     * @param root Root node
     * @param nodes_of_root Empty set nodes of root
     */
    void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
        nodes_of_root.emplace(root);
        if (root > 1) {  // terminal case of recursion
            findNodes(highSuccessor(root), nodes_of_root);
            findNodes(lowSuccessor(root), nodes_of_root);
        }
    }

    /**
     * This function takes a node root and an empty set vars of root. It returns the set of all variables which are
     * reachable from root including itself.
     *
     * @param root Root node
     * @param vars_of_root Empty set vars of root
     */
    void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
        std::set<BDD_ID> nodes_of_root;
        findNodes(root, nodes_of_root);
        for (const auto &node: nodes_of_root) {
            if (node > 1) {
                vars_of_root.emplace(topVar(node));
            }
        }
   }

    size_t Manager::uniqueTableSize() {
        return uTable.size();
    }

    /**
     * Creates a file that contains a visual representation of the BDD represented by the root node in the DOT format.
     *
     * @param filepath Filepath
     * @param root Root node
     */
    void Manager::visualizeBDD(std::string filepath, BDD_ID &root) {
        std::set<BDD_ID> vars;
        std::set<BDD_ID> nodes;
        findVars(root, vars);
        findNodes(root, nodes);
        std::ofstream File(filepath);

        File << "digraph D {\n";
        File << getTopVarName(topVar(0)) << " [shape=box]\n";
        File << getTopVarName(topVar(1)) << " [shape=box]\n";
        for (auto itr: vars) {
            File << getTopVarName(topVar(itr)) << " [shape=circle]\n";
        }
        for (auto itr: nodes) {
            if (!isConstant(itr)) {
                File << getTopVarName(topVar(itr)) << " -> " << getTopVarName(topVar(highSuccessor(itr))) << "\n";
                File << getTopVarName(topVar(itr)) << " -> " << getTopVarName(topVar(lowSuccessor(itr))) << " [style=dashed]\n";
            }
        }
        File << "}";
        File.close();
    }
}