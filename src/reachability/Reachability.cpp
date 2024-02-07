#include "Reachability.h"

namespace ClassProject {
    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize) : ReachabilityInterface(
            stateSize, inputSize) {
        if (stateSize == 0)
            throw std::runtime_error("State size cannot be zero");

        // create state and input variables
        for (unsigned long state = 0; state < stateSize; state++)
            addState();
        for (unsigned long input = 0; input < inputSize; input++)
            addInput();

        // initialize initial state vector
        for (int i = 0; i < stateSize; i++)
            initial_states.push_back(Manager::False());

        // initialize transition functions
        for (BDD_ID state: current_states)
            trans_function.push_back(state); // set to identity function
    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
        iteration_results.clear();
        std::vector<BDD_ID> vec(stateVector.size());

        // check dimensions
        if (current_states.size() != stateVector.size())
            throw std::runtime_error("State space and dimension of given current_states do not match.");
        // convert boolean data type to BDD_ID boolean data type
        for (unsigned long i = 0; i < current_states.size(); i++)
            vec[i] = stateVector[i] ? True() : False();

        BDD_ID tau = transitionRelation(current_states, inputs, next_states);
        BDD_ID cs0 = characteristicFunction(current_states, initial_states);
        iteration_results.push_back(cs0);

        BDD_ID cR, cR_it;
        cR_it = cs0;
        do {
            cR = cR_it;
            // imgR(s') := ∃x ∃s cR(s) ⋅ τ(s, x, s');
            BDD_ID imgRsp = existentialQuantification(existentialQuantification(and2(cR, tau), current_states), inputs);
            // form imgR(s) by renaming of variables s' into s;
            // imgR(s) = ∃s' (s == s') ⋅ imgR(s')
            BDD_ID imgR = existentialQuantification(and2(characteristicFunction(current_states, next_states), imgRsp),
                                                    next_states);
            // calculate newly reached states
            iteration_results.push_back(and2(imgR, neg(cR)));

            cR_it = or2(cR, imgR);
        } while (cR != cR_it);

        computation_required = false;
        return evaluateCharacteristicFunction(cR, vec) == True();
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        std::vector<BDD_ID> vec(initial_states.size());

        // check dimensions
        if (current_states.size() != stateVector.size())
            throw std::runtime_error("State space and dimension of given current_states do not match.");
        // convert boolean data type to BDD_ID boolean data type
        for (int i = 0; i < current_states.size(); i++)
            vec[i] = stateVector[i] ? True() : False();
        // run reachability algorithm to create iteration table
        if (computation_required)
            isReachable(stateVector);

        for (int i = 0; i < iteration_results.size(); i++) {
            if (evaluateCharacteristicFunction(iteration_results[i], vec) == True())
                return i;
        }
        return -1;
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {
        // check dimensions
        if (transitionFunctions.size() != current_states.size())
            throw std::runtime_error("State space and dimension of transition functions do not match.");

        // check transition function ID
        for (unsigned long transitionFunction: transitionFunctions) {
            if (transitionFunction >= uniqueTableSize())
                throw std::runtime_error("Invalid transition function. Invalid BDD ID found.");
        }

        trans_function = transitionFunctions;
    }

    void Reachability::setInitState(const std::vector<bool> &stateVector) {
        // check dimensions
        if (current_states.size() != stateVector.size())
            throw std::runtime_error("State space and dimension of initial current_states do not match.");

        // convert boolean data type to BDD_ID boolean data type
        for (unsigned long i = 0; i < stateVector.size(); i++)
            initial_states[i] = stateVector[i] ? True() : False();

        computation_required = true;
    }

    const std::vector<BDD_ID> &Reachability::getStates() const {
        return current_states;
    }

    const std::vector<BDD_ID> &Reachability::getInputs() const {
        return inputs;
    }

    /**
     * Creates variables for a new current and next state
     */
    void Reachability::addState() {
        current_states.push_back(createVar(""));
        next_states.push_back(createVar(""));
    }

    /**
     * Creates variables for an input
     */
    void Reachability::addInput() {
        inputs.push_back(createVar(""));
    }

    /**
     * Evaluates a characteristic function of current_states for a given boolean state vector
     * @param c             Characteristic function
     * @param stateVector   boolean state vector
     * @return
     */
    bool Reachability::evaluateCharacteristicFunction(BDD_ID f, const std::vector<BDD_ID> &stateVector) {
        for (unsigned long i = 0; i < stateVector.size(); i++) {
            BDD_ID x = current_states[i];
            if (stateVector[i] == True())
                f = coFactorTrue(f, x);
            else
                f = coFactorFalse(f, x);
        }
        return f == True(); // convert to boolean data type
    }

    /**
     * Computes the transition relation
     * @param s     current state
     * @param x     inputs
     * @param sp    next current_states
     * @return      BDD ID of transition relation
     */
    BDD_ID Reachability::transitionRelation(std::vector<BDD_ID> &s, std::vector<BDD_ID> &x, std::vector<BDD_ID> &sp) {
        // with n-array transition function d(s,x)
        // tau(s,x,s') = product (s'i * di(s,x) + ~(s'i) * ~(di(s,x))) from i=1 to n.
        BDD_ID tau = True();
        for (unsigned long i = 0; i < s.size(); i++) {
            BDD_ID spi = sp[i];
            BDD_ID di = trans_function[i];
            BDD_ID taup = or2(and2(spi, di), and2(neg(spi), neg(di)));
            tau = and2(tau, taup);
        }
        return tau;
    }

    /**
     * Computes the characteristic function of a BDD_ID pair
     * @param a Variable 1
     * @param b Variable 2
     * @return BDD_ID of characteristic function
     */
    BDD_ID Reachability::characteristicFunction(std::vector<BDD_ID> &a, std::vector<BDD_ID> &b) {
        // f = product of (ai == bi) from i=1 to n.
        // with (a == b) = (a xnor b)
        BDD_ID f = True();
        for (unsigned long i = 0; i < a.size(); i++) {
            BDD_ID var = a[i];
            BDD_ID value = b[i];
            f = and2(f, xnor2(var, value));
        }
        return f;
    }

    /**
     * Computes the existential quantification of a function for a given set of variables
     * @param func  Function
     * @param vars  Set of variables
     * @return BDD_ID of existential quantification
     */
    BDD_ID Reachability::existentialQuantification(BDD_ID func, const std::vector<BDD_ID> &vars) {
        // ∃vi ∈V f = ∃v f = ∃v1 ∃v2 ... ∃vn f
        BDD_ID f = func;
        for (BDD_ID var: vars)
            f = or2(coFactorTrue(f, var), coFactorFalse(f, var));
        return f;
    }
}