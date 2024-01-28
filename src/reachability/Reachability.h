#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

    class Reachability : public ReachabilityInterface {
    private:
        Manager manager = Manager();

        std::vector<BDD_ID> initial_states;
        std::vector<BDD_ID> trans_function;
        std::vector<BDD_ID> states;
        std::vector<BDD_ID> next_states;
        std::vector<BDD_ID> inputs;

        BDD_ID addState();

        BDD_ID addInput();

        BDD_ID reachableStates();   // symb_compute_reachable_states()

        BDD_ID transitionRelation(std::vector<BDD_ID> &s, std::vector<BDD_ID> &x, std::vector<BDD_ID> &sp);

        BDD_ID characteristicFunction(std::vector<BDD_ID> &vars, std::vector<BDD_ID> &values);

        BDD_ID existential_quantification(BDD_ID func, BDD_ID var);

        BDD_ID existential_quantification(BDD_ID func, const std::vector<BDD_ID> &vars);

        BDD_ID img(BDD_ID cR, BDD_ID tau);


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
