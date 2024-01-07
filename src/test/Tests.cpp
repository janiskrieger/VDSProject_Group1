//
// Created by janis on 30.11.23.
//
#include "Tests.h"

namespace ClassProject {
    class ManagerTest : public testing::Test {  /* NOLINT */
    protected:
        Manager *m;
        BDD_ID a;
        BDD_ID b;
        BDD_ID c;
        BDD_ID d;

        BDD_ID neg_a;
        BDD_ID neg_b;
        BDD_ID a_and_b;
        BDD_ID a_or_b;
        BDD_ID a_xor_b;
        BDD_ID a_nand_b;
        BDD_ID a_nor_b;
        BDD_ID a_xnor_b;
        BDD_ID c_or_d;
        // f1 = a*b+c+d
        BDD_ID f1;

        void SetUp() override {
            m = new Manager();
            a = m->createVar("a");  // ID 2
            b = m->createVar("b");  // ID 3
            c = m->createVar("c");  // ID 4
            d = m->createVar("d");  // ID 5

            neg_a = m->neg(a);
            neg_b = m->neg(b);
            a_and_b = m->and2(a, b);
            a_or_b = m->or2(a, b);
            a_xor_b = m->xor2(a, b);
            a_nand_b = m->nand2(a, b);
            a_nor_b = m->nor2(a, b);
            a_xnor_b = m->xnor2(a, b);
            c_or_d = m->or2(c, d);
            f1 = m->or2(a_and_b, c_or_d);
        }

        void TearDown() override {
            m->~Manager();
        }
    };

    TEST_F(ManagerTest, createVar) {
        // creates a new variable with the given label and returns its ID
        EXPECT_EQ(a, 2);
        EXPECT_EQ(b, 3);
    }

    TEST_F(ManagerTest, True) {
        // returns the ID of the True node
        EXPECT_EQ(m->True(), m->True());
    }

    TEST_F(ManagerTest, False) {
        // returns the ID of the False node
        EXPECT_EQ(m->False(), m->False());
    }

    TEST_F(ManagerTest, isConstant) {
        // returns true, if the given ID represents a leaf node
        EXPECT_EQ(m->isConstant(m->False()), m->True());
        EXPECT_EQ(m->isConstant(m->True()), m->True());
        EXPECT_EQ(m->isConstant(a), m->False());
        EXPECT_EQ(m->isConstant(a_or_b), m->False());
    }

    TEST_F(ManagerTest, isVariable) {
        // creates a new variable with the given label and returns its ID
        EXPECT_EQ(m->isVariable(m->True()), m->False());
        EXPECT_EQ(m->isVariable(a), m->True());
        EXPECT_EQ(m->isVariable(a_and_b), m->False());
    }

    TEST_F(ManagerTest, ite) {
        EXPECT_EQ(m->ite(m->True(), m->False(), m->True()), m->False());
        EXPECT_EQ(m->ite(m->False(), m->False(), m->True()), m->True());

        EXPECT_EQ(m->ite(m->False(), a, b), b);
        EXPECT_EQ(m->ite(m->True(), a, b), a);
        EXPECT_EQ(m->ite(a, m->True(), m->False()), a);
        EXPECT_EQ(m->ite(c, d, d), d);
    }

    TEST_F(ManagerTest, coFactorTrue) {
        EXPECT_EQ(m->coFactorTrue(m->True()), m->True());
        EXPECT_EQ(m->coFactorTrue(a), m->True());

        //f1 = a*b+c+d
        EXPECT_EQ(m->coFactorTrue(f1, a), m->or2(b, c_or_d));
        EXPECT_EQ(m->coFactorTrue(f1, a), m->coFactorTrue(f1));
        EXPECT_EQ(m->coFactorTrue(f1, b), m->or2(a, c_or_d));
        EXPECT_EQ(m->coFactorTrue(f1, c), m->True());
        EXPECT_EQ(m->coFactorTrue(f1, d), m->True());
    }

    TEST_F(ManagerTest, coFactorFalse) {
        EXPECT_EQ(m->coFactorFalse(m->False()), m->False());
        EXPECT_EQ(m->coFactorFalse(a), m->False());

        //f1 = a*b+c+d
        EXPECT_EQ(m->coFactorFalse(f1, a), c_or_d);
        EXPECT_EQ(m->coFactorFalse(f1, a), m->coFactorFalse(f1));
        EXPECT_EQ(m->coFactorFalse(f1, b), c_or_d);
        EXPECT_EQ(m->coFactorFalse(f1, c), m->or2(a_and_b, d));
        EXPECT_EQ(m->coFactorFalse(f1, d), m->or2(a_and_b, c));
    }

    TEST_F(ManagerTest, and2) {

        //truth table
        EXPECT_EQ(m->and2(m->False(), m->False()), m->False());
        EXPECT_EQ(m->and2(m->False(), m->True()), m->False());
        EXPECT_EQ(m->and2(m->True(), m->False()), m->False());
        EXPECT_EQ(m->and2(m->True(), m->True()), m->True());

        //misc test
        EXPECT_EQ(m->topVar(a_and_b), a);
        EXPECT_EQ(m->coFactorTrue(a_and_b), b);
        EXPECT_EQ(m->coFactorFalse(a_and_b), m->False());

        BDD_ID f2 = m->and2(a_and_b, c);
        EXPECT_EQ(m->topVar(f2), a);
        EXPECT_EQ(m->coFactorTrue(f2), m->and2(b, c));
        EXPECT_EQ(m->coFactorFalse(f2), m->False());
    }

    TEST_F(ManagerTest, or2) {

        //truth table
        EXPECT_EQ(m->or2(m->False(), m->False()), m->False());
        EXPECT_EQ(m->or2(m->False(), m->True()), m->True());
        EXPECT_EQ(m->or2(m->True(), m->False()), m->True());
        EXPECT_EQ(m->or2(m->True(), m->True()), m->True());

        //misc test
        EXPECT_EQ(m->topVar(a_or_b), a);
        EXPECT_EQ(m->coFactorTrue(a_or_b), m->True());
        EXPECT_EQ(m->coFactorFalse(a_or_b), b);

        BDD_ID f2 = m->or2(a_or_b, c);
        EXPECT_EQ(m->topVar(f2), a);
        EXPECT_EQ(m->coFactorTrue(f2), m->True());
        EXPECT_EQ(m->coFactorFalse(f2), m->or2(b, c));
    }

    TEST_F(ManagerTest, xor2) {

        //truth table
        EXPECT_EQ(m->xor2(m->False(), m->False()), m->False());
        EXPECT_EQ(m->xor2(m->False(), m->True()), m->True());
        EXPECT_EQ(m->xor2(m->True(), m->False()), m->True());
        EXPECT_EQ(m->xor2(m->True(), m->True()), m->False());

        //misc test
        EXPECT_EQ(m->topVar(a_xor_b), a);
        EXPECT_EQ(m->coFactorTrue(a_xor_b), neg_b);
        EXPECT_EQ(m->coFactorFalse(a_xor_b), b);

        BDD_ID f2 = m->xor2(a_xor_b, c);
        EXPECT_EQ(m->topVar(f2), a);
        EXPECT_EQ(m->coFactorTrue(f2, c), a_xnor_b);
        EXPECT_EQ(m->coFactorFalse(f2, c), a_xor_b);
    }

    TEST_F(ManagerTest, neg) {
        // returns the ID representing the negation of the given function.

        //truth table
        EXPECT_EQ(m->neg(m->True()), m->False());
        EXPECT_EQ(m->neg(m->False()), m->True());

        //misc test
        EXPECT_EQ(m->neg(a_and_b), a_nand_b);
        EXPECT_EQ(m->neg(a_or_b), a_nor_b);
        EXPECT_EQ(m->neg(a_xor_b), a_xnor_b);

        EXPECT_EQ(m->neg(m->and2(a_and_b, c)), m->or2(a_nand_b, m->neg(c)));

        BDD_ID f2 = m->xor2(a_xor_b, c);
        EXPECT_EQ(m->topVar(f2), a);
        EXPECT_EQ(m->coFactorTrue(f2, a), m->neg(m->xor2(b, c)));
        EXPECT_EQ(m->coFactorTrue(f2, b), m->neg(m->xor2(a, c)));
        EXPECT_EQ(m->coFactorTrue(f2, c), m->neg(a_xor_b));
        EXPECT_EQ(m->coFactorFalse(f2, a), m->xor2(b, c));
        EXPECT_EQ(m->coFactorFalse(f2, b), m->xor2(a, c));
        EXPECT_EQ(m->coFactorFalse(f2, c), a_xor_b);
    }

    TEST_F(ManagerTest, nand2) {

        //truth table
        EXPECT_EQ(m->nand2(m->False(), m->False()), m->True());
        EXPECT_EQ(m->nand2(m->False(), m->True()), m->True());
        EXPECT_EQ(m->nand2(m->True(), m->False()), m->True());
        EXPECT_EQ(m->nand2(m->True(), m->True()), m->False());

        //misc test
        EXPECT_EQ(m->topVar(a_nand_b), a);
        EXPECT_EQ(m->coFactorTrue(a_nand_b), neg_b);
        EXPECT_EQ(m->coFactorFalse(a_nand_b), m->True());

        EXPECT_EQ(m->nand2(a_nand_b, c), m->or2(a_and_b, m->neg(c)));    //-(-(a*b)*c) = (a*b)+-c
    }

    TEST_F(ManagerTest, nor2) {

        //truth table
        EXPECT_EQ(m->nor2(m->False(), m->False()), m->True());
        EXPECT_EQ(m->nor2(m->False(), m->True()), m->False());
        EXPECT_EQ(m->nor2(m->True(), m->False()), m->False());
        EXPECT_EQ(m->nor2(m->True(), m->True()), m->False());

        //misc test
        EXPECT_EQ(m->topVar(a_nor_b), a);
        EXPECT_EQ(m->coFactorTrue(a_nor_b), m->False());
        EXPECT_EQ(m->coFactorFalse(a_nor_b), neg_b);

        EXPECT_EQ(m->nor2(a_nor_b, c), m->and2(a_or_b, m->neg(c)));
    }

    TEST_F(ManagerTest, xnor2) {

        //truth table
        EXPECT_EQ(m->xnor2(m->False(), m->False()), m->True());
        EXPECT_EQ(m->xnor2(m->False(), m->True()), m->False());
        EXPECT_EQ(m->xnor2(m->True(), m->False()), m->False());
        EXPECT_EQ(m->xnor2(m->True(), m->True()), m->True());

        //misc test
        EXPECT_EQ(m->xnor2(a, m->False()), neg_a);
        EXPECT_EQ(m->xnor2(a, m->True()), a);
        EXPECT_EQ(m->xnor2(m->False(), a), neg_a);
        EXPECT_EQ(m->xnor2(m->True(), a), a);
        EXPECT_EQ(m->topVar(a_xnor_b), a);
        EXPECT_EQ(m->coFactorTrue(a_xnor_b), b);
        EXPECT_EQ(m->coFactorFalse(a_xnor_b), neg_b);
        EXPECT_EQ(m->xnor2(neg_b, neg_a), a_xnor_b);

        EXPECT_EQ(m->xnor2(a_xnor_b, c), m->neg(m->xor2(m->neg(a_xor_b), c)));
    }

    TEST_F(ManagerTest, getTopVarName) {
        // returns the label of the given BDD_ID
        EXPECT_EQ(m->getTopVarName(m->False()), "false");
        EXPECT_EQ(m->getTopVarName(m->True()), "true");
        EXPECT_EQ(m->getTopVarName(a), "a");

        EXPECT_EQ(m->getTopVarName(a_and_b), "a");
        EXPECT_EQ(m->getTopVarName(c_or_d), "c");
        EXPECT_EQ(m->getTopVarName(f1), "a");
    }

    MATCHER_P(SetEq, expected, "") {
        // matcher set comparison
        return std::equal(arg.begin(), arg.end(), expected.begin(), expected.end());
    }

    TEST_F(ManagerTest, findNodes) {
        // This function takes a node root and an empty set nodes of root.
        // It returns the set of all nodes which are reachable from root
        // including itself.
        // f = (a + b) * c * d
        ClassProject::BDD_ID aorb = m->or2(a, b);
        ClassProject::BDD_ID candd = m->and2(c, d);
        ClassProject::BDD_ID f = m->and2(aorb, candd);

        std::set<BDD_ID> expected = {m->and2(b, candd), candd, d, m->True(), m->False()};
        std::set<BDD_ID> nodes;
        m->findNodes(m->and2(b, candd), nodes);

        EXPECT_THAT(nodes, SetEq(expected));
    }

    TEST_F(ManagerTest, findVars) {
        // This function takes a node root and an empty set nodes of root.
        // It returns the set of all variables which are reachable from root
        // including itself. It essentially returns the set of top variables
        // of findNodes.
        // f = (a + b) * c * d
        ClassProject::BDD_ID aorb = m->or2(a, b);
        ClassProject::BDD_ID candd = m->and2(c, d);
        ClassProject::BDD_ID f = m->and2(aorb, candd);

        std::set<BDD_ID> expected = {m->topVar(b), m->topVar(c), m->topVar(d)};
        std::set<BDD_ID> vars;
        m->findVars(m->and2(b, candd), vars);

        EXPECT_THAT(vars, SetEq(expected));
    }

    TEST_F(ManagerTest, uniqueTableSize) {
        // returns the number of nodes currently existing in the unique table of the Manager class.
        BDD_ID size = m->uniqueTableSize();
        m->createVar("e");
        EXPECT_EQ(m->uniqueTableSize(), size + 1);
    }
}