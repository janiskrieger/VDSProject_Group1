#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>
#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm2 = std::make_unique<ClassProject::Reachability>(2, 0);

    std::vector<BDD_ID> stateVars2 = fsm2->getStates();
    std::vector<BDD_ID> transitionFunctions;
};

TEST_F(ReachabilityTest, HowTo_Example) {
    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm2->neg(s1)); // s1' = not(s1)
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false, false});

    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({false, true}));
    ASSERT_FALSE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));
}


TEST_F(ReachabilityTest, ConstructorRuntimeErrorTest) {
    /*
     * Test if runtime errors are thrown whenever invalid inputs are given
     */
    EXPECT_THROW(std::make_unique<ClassProject::Reachability>(0, 0), std::runtime_error);
    EXPECT_THROW(std::make_unique<ClassProject::Reachability>(0, 1), std::runtime_error);
    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    // test for one missing transition function
    transitionFunctions.push_back(fsm2->neg(s0));
    EXPECT_THROW(fsm2->setTransitionFunctions(transitionFunctions), std::runtime_error);
    // test invalid BDD_ID
    transitionFunctions.push_back((BDD_ID) 999);
    EXPECT_THROW(fsm2->setTransitionFunctions(transitionFunctions), std::runtime_error);
    transitionFunctions.pop_back();

    transitionFunctions.push_back(fsm2->neg(s1));

    // test for one additional transition function
    transitionFunctions.push_back(fsm2->neg(s1));
    EXPECT_THROW(fsm2->setTransitionFunctions(transitionFunctions), std::runtime_error);
    transitionFunctions.pop_back();

    fsm2->setTransitionFunctions(transitionFunctions);

    // test for one missing initial state
    EXPECT_THROW(fsm2->setInitState({false}), std::runtime_error);
    // test for one additional initial state
    EXPECT_THROW(fsm2->setInitState({false, false, false}), std::runtime_error);

    fsm2->setInitState({false, false});

    // test for one missing state
    EXPECT_THROW(fsm2->isReachable({false}), std::runtime_error);
    // test for one additional state
    EXPECT_THROW(fsm2->isReachable({false, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, ConstructorIdentityTest) {
    /*
     * Test if only the initial state is reachable by default
     */
    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({false, true}));
    ASSERT_FALSE(fsm2->isReachable({true, false}));
    ASSERT_FALSE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, ConstructorVarTest) {
    /*
     * Test if the constructor generates the given states
     */
    int stateSize = 1;
    int inputSize = 5;
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(stateSize,
                                                                                                            inputSize);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    EXPECT_EQ(stateVars.size(), stateSize);   // current state and next state
    EXPECT_EQ(inputVars.size(), inputSize);
}

TEST_F(ReachabilityTest, StateDistanceTest) {
    /*
     * Test if the stateDistance is calculated correctly
     */
    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s1)); // s0' = not(s1)
    transitionFunctions.push_back(s0);               // s1' = s0
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false, false});

    EXPECT_EQ(fsm2->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm2->stateDistance({true, false}), 1);
    EXPECT_EQ(fsm2->stateDistance({true, true}), 2);
    EXPECT_EQ(fsm2->stateDistance({false, true}), 3);
}

struct InputReachabilityTest : testing::Test {
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(1, 1);

    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    std::vector<BDD_ID> transitionFunctions;

    BDD_ID s1 = stateVars[0];
    BDD_ID x1 = inputVars[0];
};

TEST_F(InputReachabilityTest, InputTest) {
    transitionFunctions.push_back(fsm->and2(fsm->neg(s1), x1));
    fsm->setTransitionFunctions(transitionFunctions);

    fsm->setInitState({false, false});

    EXPECT_TRUE(fsm->isReachable({false}));
    EXPECT_FALSE(fsm->isReachable({true}));

    fsm->setInitState({false, true});
    EXPECT_TRUE(fsm->isReachable({false}));
    EXPECT_TRUE(fsm->isReachable({true}));
}

#endif