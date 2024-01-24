#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

    class Reachability : public ReachabilityInterface {
    private:
        Manager manager = Manager();

        std::vector<BDD_ID> transition_functions;
        std::vector<bool> initial_state;
        std::vector<BDD_ID> states;
        std::vector<BDD_ID> inputs;

        BDD_ID addState(const std::string &label);
        BDD_ID addInput(const std::string &label);


    public:
        Reachability(unsigned int stateSize, unsigned int inputSize);

        ~Reachability() override = default;

        const std::vector<BDD_ID> &getStates() const override;

        const std::vector<BDD_ID> &getInputs() const override;

        bool isReachable(const std::vector<bool> &stateVector) override;

        int stateDistance(const std::vector<bool> &stateVector) override;

        void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override;

        void setInitState(const std::vector<bool> &stateVector) override;

    };

}
#endif
