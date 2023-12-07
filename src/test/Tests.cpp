//
// Created by janis on 30.11.23.
//
#include "Tests.h"

namespace ClassProject{
    class LogicOperations : public ::testing::Test{
    protected:
        Manager *m;
        BDD_ID a;
        BDD_ID b;
        BDD_ID c;
        BDD_ID d;

        void SetUp() override{
            m = new Manager();
            a = m->createVar("a");  // ID 2
            b = m->createVar("b");  // ID 3
            c = m->createVar("c");  // ID 4
            d = m->createVar("d");  // ID 5
        }

        void TearDown() override{
            m->~Manager();
        }
    };

    TEST(ManagerTest, createVar){
        // Creates a new variable with the given label and returns its ID.
        Manager m = Manager();
        EXPECT_EQ(m.createVar("a"), 2);
        EXPECT_EQ(m.createVar("b"), 3);
    }

    TEST(ManagerTest, True) {
        // Returns the ID of the True node.
        Manager m;
        EXPECT_EQ(m.True(), 1);
    }

    TEST(ManagerTest, False) {
        // Returns the ID of the False node.
        Manager m;
        EXPECT_EQ(m.False(), 0);
    }

    TEST(ManagerTest, isConstant){
        // Returns true, if the given ID represents a leaf node.
        Manager m = Manager();
        m.createVar("a");   // ID: 2

        EXPECT_EQ(m.isConstant(0), true);
        EXPECT_EQ(m.isConstant(1), true);
        EXPECT_EQ(m.isConstant(2), false);
    }

    TEST(ManagerTest, isVariable){
        // Creates a new variable with the given label and returns its ID.
        Manager m = Manager();
        BDD_ID a = m.createVar("a");
        BDD_ID b = m.createVar("b");
        BDD_ID aandb = m.and2(a,b);

        EXPECT_EQ(m.isVariable(1), false);
        EXPECT_EQ(m.isVariable(a), true);
        EXPECT_EQ(m.isVariable(b), true);
        EXPECT_EQ(m.isVariable(aandb), false);

    }

    TEST(ManagerTest, getTopVarName){
        // Returns the label of the given BDD_ID.
        Manager m = Manager();
        EXPECT_EQ(m.getTopVarName(0), "false");
        EXPECT_EQ(m.getTopVarName(1), "true");
    }

    // AND2 operation
    TEST_F(LogicOperations, and2){
        BDD_ID id = m->and2(a,b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), b);
        EXPECT_EQ(m->coFactorFalse(id), 0);
    }

    // OR2 operation
    TEST_F(LogicOperations, or2) {
        BDD_ID id = m->or2(a, b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), 1);
        EXPECT_EQ(m->coFactorFalse(id), b);
    }

    // XOR2 operation
    TEST_F(LogicOperations, xor2) {
        BDD_ID id = m->xor2(a, b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), m->neg(b));
        EXPECT_EQ(m->coFactorFalse(id), b);
    }

    // NEG operation
    TEST_F(LogicOperations, neg) {
        BDD_ID id = m->neg(a);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), 0);
        EXPECT_EQ(m->coFactorFalse(id), 1);
    }

    // NAND2 operation
    TEST_F(LogicOperations, nand2) {
        BDD_ID id = m->nand2(a,b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), m->neg(b));
        EXPECT_EQ(m->coFactorFalse(id), 1);
    }

    // NOR2 operation
    TEST_F(LogicOperations, nor2) {
        BDD_ID id = m->nor2(a,b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id), 0);
        EXPECT_EQ(m->coFactorFalse(id), m->neg(b));
    }

    // XNOR2 operation
    TEST_F(LogicOperations, xnor2) {
        BDD_ID id = m->xnor2(a,b);
        EXPECT_EQ(m->topVar(id), a);
        EXPECT_EQ(m->coFactorTrue(id),b);
        EXPECT_EQ(m->coFactorFalse(id),m->neg(b));
    }




    TEST(ManagerTest, uniqueTableSize){
        // Returns the number of nodes currently existing in the unique table of the Manager class.
        Manager m = Manager();
        EXPECT_EQ(m.uniqueTableSize(), 2);

        m.createVar("a");
        EXPECT_EQ(m.uniqueTableSize(), 3);
    }
}