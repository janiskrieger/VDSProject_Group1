#include "Reachability.h"

namespace ClassProject {
    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize) : ReachabilityInterface(
            stateSize, inputSize) {
        if (stateSize == 0)
            throw std::runtime_error("State size cannot be zero");

        // create state and input variables
        for (int state = 0; state < stateSize; state++)
            addState("s" + std::to_string(state));
        for (int state = 0; state < stateSize; state++)
            addState("s'" + std::to_string(state));
        for (int input = 0; input < inputSize; input++)
            addInput("x" + std::to_string(input));

        // initialize transition functions
        for (BDD_ID var = 2; var < manager.uniqueTableSize(); var++) {   // start after constants True() and False()
            transition_functions.push_back(var); // set to identity function
        }
        // set initial state to false
        for (int state = 0; state < stateSize + inputSize; state++) {
            initial_state.push_back(false);
        }
    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
        return false;
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        return 0;
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {
        transition_functions = transitionFunctions;
    }

    void Reachability::setInitState(const std::vector<bool> &stateVector) {
        initial_state = stateVector;
    }

    const std::vector<BDD_ID> &Reachability::getStates() const {
        return states;
    }

    const std::vector<BDD_ID> &Reachability::getInputs() const {
        return inputs;
    }

    BDD_ID Reachability::addState(const std::string &label) {
        BDD_ID id = manager.createVar(label);
        states.push_back(id);
        return id;
    }

    BDD_ID Reachability::addInput(const std::string &label) {
        BDD_ID id = manager.createVar(label);
        inputs.push_back(id);
        return id;
    }

}