#include "Reachability.h"

namespace ClassProject {
    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize) : ReachabilityInterface(
            stateSize, inputSize) {
        iter_cnt = 0;
        min_dist = -1;
        state_vector = {};
        compute_distance = false;

        if (stateSize == 0)
            throw std::runtime_error("State size cannot be zero");

        // create state and input variables
        for (unsigned long state = 0; state < stateSize; state++)
            addState();
        for (unsigned long input = 0; input < inputSize; input++)
            addInput();
        states_and_inputs.insert(states_and_inputs.end(), states.begin(), states.end());
        states_and_inputs.insert(states_and_inputs.end(), inputs.begin(), inputs.end());

        // initialize transition functions
        for (BDD_ID state: states)
            trans_function.push_back(state); // set to identity function

        // set initial state to false
        for (unsigned long state = 0; state < stateSize + inputSize; state++)
            initial_states.push_back(Manager::False());
    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
        // check dimensions
        std::vector<BDD_ID> BDDstateVector;
        if (states.size() != stateVector.size())
            throw std::runtime_error("State space and dimension of given states do not match.");

        // convert boolean data type to BDD_ID boolean data type
        for (unsigned long i = 0; i < states.size(); i++)
            BDDstateVector.push_back(stateVector[i] ? True() : False());

        BDD_ID f = reachableStates();
        return eval(f, BDDstateVector);
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        min_dist = -1;
        iter_cnt = 0;
        state_vector = {};

        // check dimensions
        if (states.size() != stateVector.size())
            throw std::runtime_error("State space and dimension of given states do not match.");

        // convert boolean data type to BDD_ID boolean data type
        for (unsigned long i = 0; i < states.size(); i++)
            state_vector.push_back(stateVector[i] ? True() : False());

        compute_distance = true;
        reachableStates();
        compute_distance = false;

        return min_dist;
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {
        // check dimensions
        if (transitionFunctions.size() != states.size())
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
        if (initial_states.size() != stateVector.size())
            throw std::runtime_error("State space and dimension of initial states do not match.");

        // convert boolean data type to BDD_ID boolean data type
        for (unsigned long i = 0; i < initial_states.size(); i++)
            initial_states[i] = stateVector[i] ? True() : False();
    }

    const std::vector<BDD_ID> &Reachability::getStates() const {
        return states;
    }

    const std::vector<BDD_ID> &Reachability::getInputs() const {
        return inputs;
    }

    /**
     * Creates variables for a new current and next state
     */
    void Reachability::addState() {
        states.push_back(createVar(""));
        next_states.push_back(createVar(""));
    }

    /**
     * Creates variables for an input
     */
    void Reachability::addInput() {
        inputs.push_back(createVar(""));
    }

    /**
     * Evaluates a characteristic function of states for a given boolean state vector
     * @param c             Characteristic function
     * @param stateVector   boolean state vector
     * @return
     */
    bool Reachability::eval(BDD_ID f, const std::vector<BDD_ID> &stateVector) {
        for (unsigned long i = 0; i < stateVector.size(); i++) {
            BDD_ID x = states[i];
            if (stateVector[i] == True())
                f = coFactorTrue(f, x);
            else
                f = coFactorFalse(f, x);
        }
        return f == True(); // convert to boolean data type
    }

    /**
     * Implements the symb_compute_reachable_states and symbolically computes all reachable states
     * @return characteristic function of reachable states
     */
    BDD_ID Reachability::reachableStates() {
        BDD_ID tau = transitionRelation(states, inputs, next_states);
        BDD_ID cs0 = charactFunc(states_and_inputs, initial_states);

        BDD_ID cR, cR_it;
        cR_it = cs0;
        do {
            cR = cR_it;
            // compute state distance
            // Checks initial state. Last state is also taken into account because of loop saturation
            if (compute_distance)
                computeDistance(cR);

            // imgR(s') := ∃x ∃s cR(s) ⋅ τ(s, x, s');
            BDD_ID imgRsp = existQuant(existQuant(and2(cR, tau), states), inputs);
            // form imgR(s) by renaming of variables s' into s;
            // imgR(s) = ∃s' (s == s') ⋅ imgR(s')
            BDD_ID imgR = existQuant(and2(charactFunc(states, next_states), imgRsp), next_states);
            cR_it = or2(cR, imgR);
            iter_cnt++;
        } while (cR != cR_it);
        return cR;
    }

    /**
     * Computes the transition relation
     * @param s     current state
     * @param x     inputs
     * @param sp    next states
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
    BDD_ID Reachability::charactFunc(std::vector<BDD_ID> &a, std::vector<BDD_ID> &b) {
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
     * Computes the existential quantification of a function for a given variable
     * @param func  Function
     * @param var   Variable
     * @return BDD_ID of existential quantification
     */
    BDD_ID Reachability::existQuant(BDD_ID func, BDD_ID var) {
        // ∃xf = f|x=1 + f|x=0
        BDD_ID f = or2(coFactorTrue(func, var), coFactorFalse(func, var));
        return f;
    }

    /**
     * Computes the existential quantification of a function for a given set of variables
     * @param func  Function
     * @param vars  Set of variables
     * @return BDD_ID of existential quantification
     */
    BDD_ID Reachability::existQuant(BDD_ID func, const std::vector<BDD_ID> &vars) {
        // ∃vi ∈V f = ∃v f = ∃v1 ∃v2 ... ∃vn f
        BDD_ID f = func;
        for (BDD_ID var: vars)
            f = or2(coFactorTrue(f, var), coFactorFalse(f, var));
        return f;
    }

    /**
     * Called iteratively by the reachableStates methods. Checks if desired state is reachable and updates the shortest
     * distance.
     * @param cR    Characteristic function
     * @return      distance
     */
    int Reachability::computeDistance(BDD_ID cR) {
        bool isReachable = eval(cR, state_vector);
        if (isReachable) {
            int distance = iter_cnt;
            if (min_dist < 0 || distance < min_dist)
                min_dist = distance;
        }
        return min_dist;
    }
}