#include "Reachability.h"

namespace ClassProject {
    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize) : ReachabilityInterface(
            stateSize, inputSize) {

    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
        return false;
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        return 0;
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {

    }

    void Reachability::setInitState(const std::vector<bool> &stateVector) {

    }

    const std::vector<BDD_ID> &Reachability::getStates() const {

    }

    const std::vector<BDD_ID> &Reachability::getInputs() const {

    }

}