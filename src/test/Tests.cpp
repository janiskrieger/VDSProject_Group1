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
}