//
// Created by janis on 30.11.23.
//
#include "Tests.h"

namespace ClassProject{
    TEST(ManagerTest, ReturnIDTrue) {
        ClassProject::Manager m;
        EXPECT_EQ(m.True(), 1);
    }

    TEST(ManagerTest, ReturnIDFalse) {
        ClassProject::Manager m;
        EXPECT_EQ(m.False(), 0);
    }
    TEST(ConstructorTest, IsConstant){
        ClassProject::Manager m = ClassProject::Manager();
        EXPECT_EQ(m.isConstant(0), true);
        EXPECT_EQ(m.isConstant(1), true);
    }

    TEST(ConstructorTest, TopVarName){
        ClassProject::Manager m = ClassProject::Manager();
        EXPECT_EQ(m.getTopVarName(0), "false");
        EXPECT_EQ(m.getTopVarName(1), "true");
    }
}