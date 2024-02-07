#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

    class Reachability : public ReachabilityInterface {
    private:
        Manager manager = Manager();

        bool computation_required = true;

        std::vector<BDD_ID> initial_states;
        std::vector<BDD_ID> trans_function;
        std::vector<BDD_ID> current_states;
        std::vector<BDD_ID> next_states;
        std::vector<BDD_ID> inputs;

        std::vector<BDD_ID> iteration_results;

        void addState();

        void addInput();

        bool evaluateCharacteristicFunction(BDD_ID f, const std::vector<BDD_ID> &stateVector);

        BDD_ID transitionRelation(std::vector<BDD_ID> &s, std::vector<BDD_ID> &x, std::vector<BDD_ID> &sp);

        BDD_ID characteristicFunction(std::vector<BDD_ID> &a, std::vector<BDD_ID> &b);

        BDD_ID existentialQuantification(BDD_ID func, const std::vector<BDD_ID> &vars);

    public:
        Reachability(unsigned int stateSize, unsigned int inputSize = 0);

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
