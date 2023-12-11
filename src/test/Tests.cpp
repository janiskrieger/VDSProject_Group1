//
// Created by janis on 30.11.23.
//
#include "Tests.h"

namespace ClassProject {
    class ManagerTest : public ::testing::Test {
    protected:
        Manager *m;
        BDD_ID a;
        BDD_ID b;
        BDD_ID c;
        BDD_ID d;

        void SetUp() override {
            m = new Manager();
            a = m->createVar("a");  // ID 2
            b = m->createVar("b");  // ID 3
            c = m->createVar("c");  // ID 4
            d = m->createVar("d");  // ID 5
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
        EXPECT_EQ(m->True(), 1);
    }

    TEST_F(ManagerTest, False) {
        // returns the ID of the False node
        EXPECT_EQ(m->False(), 0);
    }

    TEST_F(ManagerTest, isConstant) {
        // returns true, if the given ID represents a leaf node
        BDD_ID aorb = m->or2(a, b);
        EXPECT_EQ(m->isConstant(0), true);
        EXPECT_EQ(m->isConstant(1), true);
        EXPECT_EQ(m->isConstant(a), false);
        EXPECT_EQ(m->isConstant(aorb), false);
    }

    TEST_F(ManagerTest, isVariable) {
        // creates a new variable with the given label and returns its ID
        BDD_ID aandb = m->and2(a, b);
        EXPECT_EQ(m->isVariable(1), false);
        EXPECT_EQ(m->isVariable(a), true);
        EXPECT_EQ(m->isVariable(b), true);
        EXPECT_EQ(m->isVariable(aandb), false);
    }

    TEST_F(ManagerTest, ite) {
        EXPECT_EQ(m->ite(0, a, b), b);
        EXPECT_EQ(m->ite(1, a, b), a);
        EXPECT_EQ(m->ite(a, 1, 0), a);
        EXPECT_EQ(m->ite(c, d, d), d);
    }

    TEST_F(ManagerTest, coFactorTrue) {
        BDD_ID aorb = m->or2(a, b);
        BDD_ID candd = m->and2(c, d);
        BDD_ID f = m->and2(aorb, candd);
        //f1 = (a+b)cd
        EXPECT_EQ(m->coFactorTrue(f, a), candd);
        EXPECT_EQ(m->coFactorTrue(f, a), m->coFactorTrue(f));
        EXPECT_EQ(m->coFactorTrue(f, b), candd);
        EXPECT_EQ(m->coFactorTrue(f, c), m->and2(d, aorb));
        EXPECT_EQ(m->coFactorTrue(f, d), m->and2(c, aorb));
    }

    TEST_F(ManagerTest, coFactorFalse) {
        BDD_ID aorb = m->or2(a, b);
        BDD_ID candd = m->and2(c, d);
        BDD_ID f = m->and2(aorb, candd);
        //f1 = (a+b)cd
        EXPECT_EQ(m->coFactorFalse(f, a), m->and2(b, candd));
        EXPECT_EQ(m->coFactorFalse(f, a), m->coFactorFalse(f));
        EXPECT_EQ(m->coFactorFalse(f, b), m->and2(a, candd));
        EXPECT_EQ(m->coFactorFalse(f, c), 0);
        EXPECT_EQ(m->coFactorFalse(f, d), 0);
    }

    TEST_F(ManagerTest, and2) {
        BDD_ID id = m->and2(a, b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), b);
        EXPECT_EQ(m->coFactorFalse(id), 0);
    }

    TEST_F(ManagerTest, or2) {
        BDD_ID id = m->or2(a, b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), 1);
        EXPECT_EQ(m->coFactorFalse(id), b);
        BDD_ID id2 = m->or2(d, c);
        EXPECT_EQ(m->coFactorTrue(id2, c), 1);
        EXPECT_EQ(m->coFactorFalse(id2), d);
    }

    TEST_F(ManagerTest, xor2) {
        BDD_ID id = m->xor2(a, b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), m->neg(b));
        EXPECT_EQ(m->coFactorFalse(id), b);
    }

    TEST_F(ManagerTest, neg) {
        // returns the ID representing the negation of the given function.
        EXPECT_EQ(m->neg(m->True()), m->False());
        EXPECT_EQ(m->neg(m->False()), m->True());
        BDD_ID nor1 = m->neg(m->or2(a, b));
        BDD_ID nor2 = m->nor2(a, b);
        EXPECT_EQ(nor1, nor2);
        EXPECT_EQ(m->topVar(nor1), m->topVar(nor2));
        EXPECT_EQ(m->coFactorTrue(nor1), m->coFactorTrue(nor2));
        EXPECT_EQ(m->coFactorFalse(nor1), m->coFactorFalse(nor2));

        ClassProject::BDD_ID f = m->xor2(a, m->xor2(b, c));
        EXPECT_EQ(m->topVar(f), a);
        EXPECT_EQ(m->coFactorTrue(f, a), m->neg(m->xor2(b, c)));
        EXPECT_EQ(m->coFactorTrue(f, b), m->neg(m->xor2(a, c)));
        EXPECT_EQ(m->coFactorTrue(f, c), m->neg(m->xor2(a, b)));
        EXPECT_EQ(m->coFactorFalse(f, a), m->xor2(b, c));
        EXPECT_EQ(m->coFactorFalse(f, b), m->xor2(a, c));
        EXPECT_EQ(m->coFactorFalse(f, c), m->xor2(a, b));
    }

    TEST_F(ManagerTest, nand2) {
        BDD_ID id = m->nand2(a, b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), m->neg(b));
        EXPECT_EQ(m->coFactorFalse(id), 1);
    }

    TEST_F(ManagerTest, nor2) {
        BDD_ID id = m->nor2(a, b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), 0);
        EXPECT_EQ(m->coFactorFalse(id), m->neg(b));
    }

    TEST_F(ManagerTest, xnor2) {
        BDD_ID nota = m->neg(a);
        BDD_ID notb = m->neg(b);
        BDD_ID axnorb = m->xnor2(a, b);

        EXPECT_EQ(m->xnor2(0, 0), 1);
        EXPECT_EQ(m->xnor2(0, 1), 0);
        EXPECT_EQ(m->xnor2(1, 0), 0);
        EXPECT_EQ(m->xnor2(1, 1), 1);
        EXPECT_EQ(m->xnor2(a, 0), nota);
        EXPECT_EQ(m->xnor2(a, 1), a);
        EXPECT_EQ(m->xnor2(0, a), nota);
        EXPECT_EQ(m->xnor2(1, a), a);
        EXPECT_EQ(m->topVar(axnorb), a);
        EXPECT_EQ(m->coFactorTrue(axnorb), b);
        EXPECT_EQ(m->coFactorFalse(axnorb), notb);
        EXPECT_EQ(m->xnor2(notb, nota), axnorb);
    }

    TEST_F(ManagerTest, getTopVarName) {
        // returns the label of the given BDD_ID
        EXPECT_EQ(m->getTopVarName(0), "false");
        EXPECT_EQ(m->getTopVarName(1), "true");
        EXPECT_EQ(m->getTopVarName(a), "a");
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

        std::set<BDD_ID> expected = {8, 7, 5, 1, 0};
        std::set<BDD_ID> nodes;
        m->findNodes(8, nodes);

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
        m->findVars(8, vars);

        EXPECT_THAT(vars, SetEq(expected));
    }

    TEST_F(ManagerTest, uniqueTableSize) {
        // returns the number of nodes currently existing in the unique table of the Manager class.
        EXPECT_EQ(m->uniqueTableSize(), 6);

        m->createVar("e");
        EXPECT_EQ(m->uniqueTableSize(), 7);
    }
}