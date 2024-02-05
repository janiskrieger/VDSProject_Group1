#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>
#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {
    // test fixture
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(2, 0);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID s0 = stateVars.at(0);
    BDD_ID s1 = stateVars.at(1);
};

TEST_F(ReachabilityTest, ConstructorRuntimeErrorTest) {
    // throws std::runtime_error if stateSize is zero
    EXPECT_THROW(std::make_unique<ClassProject::Reachability>(0, 0), std::runtime_error);
    EXPECT_THROW(std::make_unique<ClassProject::Reachability>(0, 1), std::runtime_error);
}

TEST_F(ReachabilityTest, isReachableRuntimeErrorTest) {
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(fsm->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm->neg(s1)); // s1' = not(s1)
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});

    // test for one missing state
    EXPECT_THROW(fsm->isReachable({false}), std::runtime_error);
    // test for one additional state
    EXPECT_THROW(fsm->isReachable({false, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, stateDistanceRuntimeErrorTest) {
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(fsm->neg(s0));
    transitionFunctions.push_back(fsm->neg(s1));
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});
}

TEST_F(ReachabilityTest, TransistionRuntimeErrorTest) {
    // throws std::runtime_error if size does not match the number of state bits
    transitionFunctions.push_back(fsm->neg(s0));
    EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions), std::runtime_error);
    // test invalid BDD_ID
    transitionFunctions.push_back((BDD_ID) 999);
    EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions), std::runtime_error);
    transitionFunctions.pop_back();

    transitionFunctions.push_back(fsm->neg(s1));

    // test for one additional transition function
    transitionFunctions.push_back(fsm->neg(s1));
    EXPECT_THROW(fsm->setTransitionFunctions(transitionFunctions), std::runtime_error);
}

TEST_F(ReachabilityTest, setInitialStateRuntimeErrorTest) {
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(fsm->neg(s0));
    transitionFunctions.push_back(fsm->neg(s1));
    fsm->setTransitionFunctions(transitionFunctions);

    // test for one missing initial state
    EXPECT_THROW(fsm->setInitState({false}), std::runtime_error);
    // test for one additional initial state
    EXPECT_THROW(fsm->setInitState({false, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, ConstructorIdentityTest) {
    // test if identity function is used as default transition function
    ASSERT_TRUE(fsm->isReachable({false, false}));
    ASSERT_FALSE(fsm->isReachable({false, true}));
    ASSERT_FALSE(fsm->isReachable({true, false}));
    ASSERT_FALSE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, ConstructorVarTest) {
    // test if correct number of state and input variables are created
    int stateSize = 13;
    int inputSize = 7;
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(stateSize,
                                                                                                            inputSize);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    EXPECT_EQ(stateVars.size(), stateSize);
    EXPECT_EQ(inputVars.size(), inputSize);
}

TEST_F(ReachabilityTest, isReachableTest) {
    // test if correct states are reachable
    transitionFunctions.push_back(fsm->neg(s0));
    transitionFunctions.push_back(fsm->neg(s1));
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});

    ASSERT_TRUE(fsm->isReachable({false, false}));
    ASSERT_FALSE(fsm->isReachable({false, true}));
    ASSERT_FALSE(fsm->isReachable({true, false}));
    ASSERT_TRUE(fsm->isReachable({true, true}));
}

TEST_F(ReachabilityTest, StateDistanceTest) {
    // test if computed distance between states matches
    transitionFunctions.push_back(fsm->neg(s1));
    transitionFunctions.push_back(s0);
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});

    EXPECT_EQ(fsm->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm->stateDistance({true, false}), 1);
    EXPECT_EQ(fsm->stateDistance({true, true}), 2);
    EXPECT_EQ(fsm->stateDistance({false, true}), 3);
}

TEST_F(ReachabilityTest, SingleInputTest) {
    // test if state machine works wih input variables
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(1, 1);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID s0 = stateVars.at(0);
    BDD_ID x0 = inputVars.at(0);

    transitionFunctions.push_back(fsm->and2(fsm->neg(s0), x0));
    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, false});

    EXPECT_TRUE(fsm->isReachable({false}));
    EXPECT_FALSE(fsm->isReachable({true}));
    fsm->setInitState({false, true});
    EXPECT_TRUE(fsm->isReachable({false}));
    EXPECT_TRUE(fsm->isReachable({true}));
}

TEST_F(ReachabilityTest, DualInputTest) {
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm = std::make_unique<ClassProject::Reachability>(2, 2);
    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID s1 = stateVars.at(0);
    BDD_ID s2 = stateVars.at(1);
    BDD_ID x1 = inputVars.at(0);
    BDD_ID x2 = inputVars.at(1);

    const int NOT_REACHABLE = -1;
    // d1 = (!s1)s2 + s1(!s2) = s1 xor s2
    transitionFunctions.push_back(fsm->xor2(s1, s2));
    // d2 = (!s1)(!s2)x1 + (!x1)s2x2 + s1(!s2)
    BDD_ID f1 = fsm->and2(fsm->neg(s1), fsm->and2(fsm->neg(s2), x1));
    BDD_ID f2 = fsm->and2(fsm->neg(s1), fsm->and2(s2, x2));
    BDD_ID f3 = fsm->and2(s1, fsm->neg(s2));
    transitionFunctions.push_back(fsm->or2(f1, fsm->or2(f2, f3)));
    fsm->setTransitionFunctions(transitionFunctions);

    fsm->setInitState({false, false, false, false});
    EXPECT_TRUE(fsm->isReachable({false, false}));
    EXPECT_FALSE(fsm->isReachable({false, true}));
    EXPECT_FALSE(fsm->isReachable({true, false}));
    EXPECT_FALSE(fsm->isReachable({true, true}));
    EXPECT_EQ(fsm->stateDistance({true, false}), NOT_REACHABLE);
    EXPECT_EQ(fsm->stateDistance({true, true}), NOT_REACHABLE);

    fsm->setInitState({false, false, false, true});
    EXPECT_TRUE(fsm->isReachable({false, false}));
    EXPECT_FALSE(fsm->isReachable({false, true}));
    EXPECT_FALSE(fsm->isReachable({true, false}));
    EXPECT_FALSE(fsm->isReachable({true, true}));
    EXPECT_EQ(fsm->stateDistance({true, false}), NOT_REACHABLE);
    EXPECT_EQ(fsm->stateDistance({true, true}), NOT_REACHABLE);

    fsm->setInitState({false, false, true, false});
    EXPECT_TRUE(fsm->isReachable({false, false}));
    EXPECT_TRUE(fsm->isReachable({false, true}));
    EXPECT_TRUE(fsm->isReachable({true, false}));
    EXPECT_TRUE(fsm->isReachable({true, true}));
    EXPECT_EQ(fsm->stateDistance({true, false}), 2);
    EXPECT_EQ(fsm->stateDistance({true, true}), 3);

    fsm->setInitState({false, false, true, true});
    EXPECT_TRUE(fsm->isReachable({false, false}));
    EXPECT_TRUE(fsm->isReachable({false, true}));
    EXPECT_FALSE(fsm->isReachable({true, false}));
    EXPECT_TRUE(fsm->isReachable({true, true}));
    EXPECT_EQ(fsm->stateDistance({true, false}), NOT_REACHABLE);
    EXPECT_EQ(fsm->stateDistance({true, true}), 2);
}

#endif