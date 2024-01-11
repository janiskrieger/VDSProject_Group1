#include "Manager.h"

namespace ClassProject {
    Manager::Manager() {
        init_unique_table();
    }

    /**
     * Initializes the unique table with the leaf nodes True and False.
     */
    void Manager::init_unique_table() {
        uTableEntry false_id = {.id=uniqueTableSize(), .high=False(), .low=False(), .topVar=False(), .label="false"};
        unique_table.push_back(false_id);
        unique_table_map.insert({hashFunction(false_id.topVar, false_id.high, false_id.low), false_id.id});
        uTableEntry true_id = {.id=uniqueTableSize(), .high=True(), .low=True(), .topVar=True(), .label="true"};
        unique_table.push_back(true_id);
        unique_table_map.insert({hashFunction(true_id.topVar, true_id.high, true_id.low), true_id.id});
    }

    /**
     * Creates a new variable with the given label and returns its ID.
     *
     * @param label Label of variable to be created
     * @return      The ID of the newly created variable
     */
    BDD_ID Manager::createVar(const std::string &label) {
        BDD_ID id = uniqueTableSize();
        uTableEntry node = {.id=id, .high=True(), .low=False(), .topVar=id, .label=label};
        unique_table.push_back(node);
        unique_table_map.insert({hashFunction(node.topVar, node.high, node.low), node.id});
        return id;
    }

    /**
     * Returns the ID of the True node.
     *
     * @return The ID of the True node
     */
    const BDD_ID &Manager::True() {
        static const BDD_ID val = 1;
        return val;
    }

    /**
     * Returns the ID of the False node.
     *
     * @return The ID of the False node
     */
    const BDD_ID &Manager::False() {
        static const BDD_ID val = 0;
        return val;
    }

    /**
     * Returns true, if the given ID represents a leaf node.
     *
     * @param f Node
     * @return  Returns true, if the given ID represents a leaf node
     */
    bool Manager::isConstant(BDD_ID f) {
        return f == True() || f == False();
    }

    /**
    * Returns true, if the given ID represents a variable.
     *
    * @param f  Node
    * @return   Returns true, if the given ID represents a variable
    */
    bool Manager::isVariable(BDD_ID x) {
        return x == topVar(x) && !isConstant(x);
    }

    /**
    * Returns the top variable ID of the given node.
     *
    * @param f  Node
    * @return   The top variable ID of the given node
    */
    BDD_ID Manager::topVar(BDD_ID f) {
        return unique_table[f].topVar;
    }

    /**
     * Implements the if-then-else algorithm, which most of the following functions are based on. Returns the existing
     * or new node that represents the given expression.
     *
     * @param i Node (if)
     * @param t Node (then)
     * @param e Node (else)
     * @return  The existing or new node that represents the given expression
     */
    BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) { /* NOLINT */
        // terminal case of recursion
        BDD_ID r;
        if (i == True() || t == e) return t;
        else if (i == False()) return e;
        else if (t == True() and e == False()) return i;
        //else if (t == False() and e == True()) return neg(i);
        else if (!isConstant(t) && !isConstant(e)) { // Standard Triples
            // simplification of arguments
            if (i == t) t = True();                // ite (F,F,G) = ite (F,1,G)
            else if (i == e) e = False();          // ite(F,G,F) = ite(F,G,0)
            else if (i == neg(e)) e = True();   // ite(F,G,~F) = ite (F,G,1)
            else if (i == neg(t)) t = False();  // ite(F,~F,G) = ite (F,0,G)

            // equivalent pairs
            if (t == True() && i > e)              // ite(F,1,G) = ite(G,1,F)
                swapID(&i, &e);
            else if (e == False() && i > t)        // ite(F,G,0) = ite(G,F,0)
                swapID(&i, &t);
            else if (e == True() && i > t) {       // ite(F,G,1) = ite(~G,~F,1)
                i = neg(i);
                t = neg(t);
                swapID(&i, &t);
            }else if (t == False() && i > e) {     // ite(F,0,G) = ite(~G,0,~F)
                i = neg(i);
                e = neg(e);
                swapID(&i, &e);
            }else if (t == neg(e) && i > t) {   // ite(F,G,~G) = ite(G,F,~F)
                swapID(&i, &t);
                e = neg(t);
            }

            // complement edges
            if (i != t && i != e && t != e){    // F != G != H
                if (t > e){                     // ite(F,G,H) = ite(~F,H,G)
                    i = neg(i);
                    swapID(&t, &e);
                }
            }
        }

        if (auto search = computed_table.find(hashFunction(i, t, e)); search != computed_table.end()){
            return search->second;
        }
        // let x be the top-variable of (i, t, e)
        BDD_ID x = topVar(i);
        if (topVar(t) < x && !isConstant(t)) x = topVar(t);
        if (topVar(e) < x && !isConstant(e)) x = topVar(e);

        BDD_ID r_high = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
        BDD_ID r_low = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));
        if (r_high == r_low) return r_high; // reduction is possible

        r = find_or_add_unique_table(x, r_high, r_low);
        return r;

    }

    /**
     * Returns the positive co-factor of the function represented by ID f with regards to variable x.
     *
     * @param f Function represented by ID
     * @param x Variable
     * @return  The positive co-factor
     */
    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {  /* NOLINT */
        if (isConstant(f) || isConstant(x) || topVar(f) > x) return f;   // terminal case
        if (topVar(f) == x) return coFactorTrue(f);

        BDD_ID T = coFactorTrue(coFactorTrue(f), x);
        BDD_ID F = coFactorTrue(coFactorFalse(f), x);
        return ite(topVar(f), T, F);
    }

    /**
     * Returns the negative co-factor of the function represented by ID f with regards to variable x.
     *
     * @param f Function represented by ID
     * @param x Variable
     * @return  The negative co-factor
     */
    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) { /* NOLINT */
        if (isConstant(f) || isConstant(x) || topVar(f) > x) return f;  // terminal case
        if (topVar(f) == x) return coFactorFalse(f);

        BDD_ID T = coFactorFalse(coFactorTrue(f), x);
        BDD_ID F = coFactorFalse(coFactorFalse(f), x);
        return ite(topVar(f), T, F);
    }

    /**
     * Returns the positive co-factor of the function represented by ID f with regards to its top variable.
     *
     * @param f Function represented by ID
     * @return  The positive co-factor
     */
    BDD_ID Manager::coFactorTrue(BDD_ID f) {
        return unique_table[f].high;
    }

    /**
     * Returns the positive co-factor of the function represented by ID f with regards to its top variable.
     *
     * @param f Function represented by ID
     * @return  The negative co-factor
     */
    BDD_ID Manager::coFactorFalse(BDD_ID f) {
        return unique_table[f].low;
    }

    /**
     * Returns the ID representing the resulting function of a * b.
     *
     * @param a ID of variable a
     * @param b ID of variable b
     * @return  The ID representing the resulting function of a * b
     */
    BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
        return ite(a, b, False());
    }

    /**
     * Returns the ID representing the resulting function of a + b.
     *
     * @param a ID of variable a
     * @param b ID of variable b
     * @return  The ID representing the resulting function of a + b
     */
    BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
        return ite(a, True(), b);
    }

    /**
     * Returns the ID representing the resulting function of a xor b.
     *
     * @param a ID of variable a
     * @param b ID of variable b
     * @return  The ID representing the resulting function of a xor b
     */
    BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
        return ite(a, neg(b), b);
    }

    /**
     * Returns the ID representing the negation of the given function.
     *
     * @param a ID of variable a
     * @return  The ID representing the negation of the given function
     */
    BDD_ID Manager::neg(BDD_ID a) {
        return ite(a, False(), True());
    }

    /**
     * Returns the ID representing the resulting function of ~(a * b).
     *
     * @param a ID of variable a
     * @param b ID of variable b
     * @return  The ID representing the resulting function of ~(a * b).
     */
    BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
        return ite(a, neg(b), True());
    }

    /**
     * Returns the ID representing the resulting function of ~(a + b).
     *
     * @param a ID of variable a
     * @param b ID of variable b
     * @return  The ID representing the resulting function of ~(a + b).
     */
    BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
        return ite(a, False(), neg(b));
    }

    /**
     * Returns the ID representing the resulting function of ~(a xor b).
     *
     * @param a ID of variable a
     * @param b ID of variable b
     * @return  The ID representing the resulting function of ~(a xor b).
     */
    BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
        return ite(a, b, neg(b));
    }

    /**
     * Returns the label of the top variable of root.
     *
     * @param root  ID of root variable
     * @return      The label of the top variable of root
     */
    std::string Manager::getTopVarName(const BDD_ID &root) {
        return unique_table[root].label;
    }

    /**
     * This function takes a node root and an empty set nodes of root. It returns the set of all nodes which are
     * reachable from root including itself.
     *
     * @param root          ID of root node
     * @param nodes_of_root Empty set nodes of root
     */
    void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {  /* NOLINT */
        nodes_of_root.emplace(root);
        if (root > True()) {  // terminal case of recursion
            findNodes(coFactorTrue(root), nodes_of_root);
            findNodes(coFactorFalse(root), nodes_of_root);
        }
    }

    /**
     * This function takes a node root and an empty set vars of root. It returns the set of all variables which are
     * reachable from root including itself.
     *
     * @param root          ID of root node
     * @param vars_of_root  Empty set vars of root
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

    /**
     * Returns the number of nodes currently existing in the unique table of the Manager class.
     *
     * @return The number of nodes currently existing in the unique table of the Manager class
     */
    size_t Manager::uniqueTableSize() {
        return unique_table.size();
    }

    /**
     * Creates a file that contains a visual representation of the BDD represented by the root node in the DOT format.
     *
     * @param filepath  Filepath to file
     * @param root      Root node
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
                File << itr << " -> " << coFactorFalse(itr) << " [style=dashed]\n";
                File << itr << " -> " << coFactorTrue(itr) << "\n";
            }
        }
        File << "}";
        File.close();
    }

    /**
     * Find or add unique table and eliminate isomorphic sub-graphs
     *
     * @param x     Variable
     * @param high  High successor
     * @param low   Low successor
     * @return      ID of unique table entry
     */
    BDD_ID Manager::find_or_add_unique_table(ClassProject::BDD_ID x, ClassProject::BDD_ID high,
                                             ClassProject::BDD_ID low) {
        if (auto search = unique_table_map.find(hashFunction(x, high, low)); search != unique_table_map.end()){
            return unique_table[search->second].id;
        }
        BDD_ID id = uniqueTableSize();
        uTableEntry node = {.id=id, .high=high, .low=low, .topVar=x, .label=getTopVarName(x)};
        unique_table.push_back(node);
        unique_table_map.insert({hashFunction(node.topVar, node.high, node.low), node.id});
        return id;
    }

    /**
     * Prints a formatted unique table to std.
     */
    void Manager::print_unique_table() {
        std::cout << std::setw(10) << "BDD_ID" << std::setw(10) << "Label" << std::setw(10) << "High" << std::setw(10)
                  << "Low" << std::setw(10) << "TopVar" << std::endl;
        for (BDD_ID id = False(); id < uniqueTableSize(); id++) {
            if (isConstant(id) || isVariable(id)) {
                std::cout << std::setw(10) << id << std::setw(10) << getTopVarName(id) << std::setw(10)
                          << coFactorTrue(id) << std::setw(10) << coFactorFalse(id) << std::setw(10) << topVar(id)
                          << std::endl;
            } else {
                std::cout << std::setw(10) << id << std::setw(10) << "" << std::setw(10)
                          << coFactorTrue(id) << std::setw(10) << coFactorFalse(id) << std::setw(10) << topVar(id)
                          << std::endl;
            }
        }
    }

    size_t Manager::hashFunction(BDD_ID f, BDD_ID g, BDD_ID h) {
        return (((f << 21) + g) << 21) + h;
    }

    void Manager::swapID(BDD_ID *a, BDD_ID *b) {
            BDD_ID temp = *a;
            *a = *b;
            *b = temp;
    }
}