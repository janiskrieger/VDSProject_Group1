#include "Manager.h"

#define DEBUG

namespace ClassProject {
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

    /**
     * Find or add unique table and eliminate isomorphic sub-graphs
     *
     * @param x     variable
     * @param high  high successor
     * @param low   low successor
     * @return      BDD ID of unique table entry
     */
    BDD_ID Manager::find_or_add_unique_table(ClassProject::BDD_ID x, ClassProject::BDD_ID high,
                                             ClassProject::BDD_ID low) {
        ClassProject::BDD_ID r = -1;
        for (BDD_ID id = False(); id < uniqueTableSize(); id++) {
            if (topVar(id) == x && highSuccessor(id) == high && lowSuccessor(id) == low) {
                r = id;
                break;
            }
        }
        if (r == -1) {   // no entry was found
            r = uniqueTableSize();
            uTable.push_back({high, low, x});
#ifdef DEBUG
            std::cout << std::setw(10) << r << std::setw(10) << "" << std::setw(10) << highSuccessor(r)
                      << std::setw(10) << lowSuccessor(r) << std::setw(10) << topVar(r) << std::endl;
#endif
        }
        return r;
    }

    /**
     * Prints contents of uTable to std.
     */
    void Manager::printuTable() {
        std::cout << std::setw(10) << "BDD_ID" << std::setw(10) << "Label" << std::setw(10) << "High" << std::setw(10)
                  << "Low" << std::setw(10) << "TopVar" << std::endl;
        for (BDD_ID id = False(); id < uniqueTableSize(); id++) {
            if (isConstant(id) || isVariable(id)) {
                std::cout << std::setw(10) << id << std::setw(10) << getTopVarName(id) << std::setw(10)
                          << highSuccessor(id) << std::setw(10) << lowSuccessor(id) << std::setw(10) << topVar(id)
                          << std::endl;
            } else {
                std::cout << std::setw(10) << id << std::setw(10) << "" << std::setw(10)
                          << highSuccessor(id) << std::setw(10) << lowSuccessor(id) << std::setw(10) << topVar(id)
                          << std::endl;
            }
        }
    }

    /**
     * Initializes unique table
     *
     * @param
     * @return
     */
    Manager::Manager() {
        // uTable order: HIGH, LOW, TopVar
        uTable.push_back({False(), False(), False()});
        topVarNameTable.emplace_back("false");
        uTable.push_back({True(), True(), True()});
        topVarNameTable.emplace_back("true");
#ifdef DEBUG
        printuTable();
#endif
    }

    BDD_ID Manager::createVar(const std::string &label) {
        BDD_ID id = uniqueTableSize();
        uTable.push_back({True(), False(), id});
        topVarNameTable.emplace_back(label);
#ifdef DEBUG
        std::cout << std::setw(10) << id << std::setw(10) << getTopVarName(id) << std::setw(10)
                  << highSuccessor(id) << std::setw(10) << lowSuccessor(id) << std::setw(10) << topVar(id)
                  << std::endl;
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
        return x == topVar(x) && !isConstant(x);
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
        // terminal case of recursion
        // a) ite(1, f, g) = ite(0, g, f) = ite(f, 1, 0) = ite(g, f, f) = f
        // b) ite(f, 0, 1) = neg(f)
        if (i == True() || t == e) {
            return t;
        } else if (i == False()) {
            return e;
        } else if (t == True() and e == False()) {
            return i;
        } /*else if (computed table has entry for (f, g, h)){
            return i;   // eliminate repetitions in computations
        }*/

        // let x be the top-variable of (i, t, e)
        BDD_ID x = topVar(i);
        if (topVar(t) < x && !isConstant(t))
            x = topVar(t);
        if (topVar(e) < x && !isConstant(e))
            x = topVar(e);

        // ite (7, 0, 1) = neg(7) = 3 = b
        BDD_ID high = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
        BDD_ID low = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));
        if (high == low)    // reduction is possible
            return high;

        BDD_ID r = find_or_add_unique_table(x, high, low);
        //update_computed_table((f, g, h), r);
        return r;

    }

    /**
     * Returns the positive co-factor of the function f with regard to variable x.
     *
     * @param f Function represented by ID
     * @param x Co-factor variable
     * @return Positive co-factor
     */
    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
        if (isConstant(f) || isConstant(x) || topVar(f) > x)    // terminal case
            return f;
        if (topVar(f) == x)
            return highSuccessor(f);

        BDD_ID T = coFactorTrue(highSuccessor(f), x);
        BDD_ID F = coFactorTrue(lowSuccessor(f), x);
        return ite(topVar(f), T, F);
    }

    /**
     * Returns the negative co-factor of the function f with regard to variable x.
     *
     * @param f Function represented by ID
     * @param x Co-factor variable
     * @return Negative co-factor
     */
    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
        if (isConstant(f) || isConstant(x) || topVar(f) > x)    // terminal case
            return f;
        if (topVar(f) == x)
            return lowSuccessor(f);

        BDD_ID T = coFactorFalse(highSuccessor(f), x);
        BDD_ID F = coFactorFalse(lowSuccessor(f), x);
        return ite(topVar(f), T, F);
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
        return ite(a, b, False());
    }

    BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
        return ite(a, True(), b);
    }

    BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
        return ite(a, neg(b), b);
    }

    BDD_ID Manager::neg(BDD_ID a) {
        return ite(a, False(), True());
        /*if (isConstant(a)) {
            if (topVar(a) == True())
                return False();
            return True();
        }
        BDD_ID high = neg(highSuccessor(a));
        BDD_ID low = neg(lowSuccessor(a));
        return ite(topVar(a), high, low);*/
    }

    BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
        return ite(a, neg(b), True());
    }

    BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
        return ite(a, False(), neg(b));
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
        if (root > True()) {  // terminal case of recursion
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
            if (node > True()) {
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
        std::set<BDD_ID> nodes;
        findNodes(root, nodes);
        std::ofstream File(filepath);

        File << "digraph D {\n";
        File << False() << " [shape=box label=\"" << False() << "\"]\n";
        File << True() << " [shape=box label=\"" << True() << "\"]\n";
        for (auto itr: nodes) {
            if (!isConstant(itr)) {
                File << itr << " [shape=circle label=\"" << getTopVarName(topVar(itr)) << "\"]\n";
                File << itr << " -> " << lowSuccessor(itr) << " [style=dashed]\n";
                File << itr << " -> " << highSuccessor(itr) << "\n";
            }
        }
        File << "}";
        File.close();
    }
}