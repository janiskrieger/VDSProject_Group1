//
// Created by janis on 30.11.23.
//
#include "Tests.h"

namespace ClassProject{
    TEST(ManagerTest, createVar){
        // Creates a new variable with the given label and returns its ID.
        ClassProject::Manager m = ClassProject::Manager();
        EXPECT_EQ(m.createVar("a"), 2);
        EXPECT_EQ(m.createVar("b"), 3);
    }

    TEST(ManagerTest, True) {
        // Returns the ID of the True node.
        ClassProject::Manager m;
        EXPECT_EQ(m.True(), 1);
    }

    TEST(ManagerTest, False) {
        // Returns the ID of the False node.
        ClassProject::Manager m;
        EXPECT_EQ(m.False(), 0);
    }

    TEST(ManagerTest, isConstant){
        // Returns true, if the given ID represents a leaf node.
        ClassProject::Manager m = ClassProject::Manager();
        m.createVar("a");   // ID: 2

        EXPECT_EQ(m.isConstant(0), true);
        EXPECT_EQ(m.isConstant(1), true);
        EXPECT_EQ(m.isConstant(2), false);
    }

    TEST(ManagerTest, isVariable){
        // Creates a new variable with the given label and returns its ID.
        ClassProject::Manager m = ClassProject::Manager();
        m.createVar("a");   // ID: 2
        m.createVar("b");   // ID: 3

        EXPECT_EQ(m.isVariable(1), false);
        EXPECT_EQ(m.isVariable(2), true);
        EXPECT_EQ(m.isVariable(3), true);
    }

    TEST(ManagerTest, getTopVarName){
        // Returns the label of the given BDD_ID.
        ClassProject::Manager m = ClassProject::Manager();
        EXPECT_EQ(m.getTopVarName(0), "false");
        EXPECT_EQ(m.getTopVarName(1), "true");
    }

    TEST(ManagerTest, uniqueTableSize){
        // Returns the number of nodes currently existing in the unique table of the Manager class.
        ClassProject::Manager m = ClassProject::Manager();
        EXPECT_EQ(m.uniqueTableSize(), 2);

        m.createVar("a");
        EXPECT_EQ(m.uniqueTableSize(), 3);
    }
}