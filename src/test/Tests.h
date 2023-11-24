//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"

namespace ClassProject {
    TEST(ConstructorTest, ReturnIDTrue) {
        ClassProject::Manager m;
        EXPECT_EQ(m.True(), 1);
    }

    TEST(ConstructorTest, ReturnIDFalse) {
        ClassProject::Manager m;
        EXPECT_EQ(m.False(), 0);
    }
}

#endif
