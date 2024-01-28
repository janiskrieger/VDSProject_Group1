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

        // initialize transition functions //TODO: change to states
        for (BDD_ID var = Manager::True() + 1;
             var < manager.uniqueTableSize(); var++) {   // start after constants True() and False()
            trans_function.push_back(var); // set to identity function
        }
        // set initial state to false
        for (unsigned long state = 0; state < stateSize + inputSize; state++) {
            initial_states.push_back(false);
        }

    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
        // check dimensions
        std::vector<BDD_ID> values;
        if (states.size() != stateVector.size())
            throw std::runtime_error("State space and dimension of given states do not match.");

        // convert boolean data type to BDD_ID boolean data type
        for (unsigned long i = 0; i < states.size(); i++)
            values.push_back(stateVector[i] ? True() : False());

        // evaluate characteristic function of FSM for values given in stateVector
        BDD_ID c = reachableStates();
        for (unsigned long i = 0; i < stateVector.size(); i++) {
            if (stateVector[i] == True())
                c = coFactorTrue(c, states[i]);
            else
                c = coFactorFalse(c, states[i]);
        }
        return c == True(); // convert to boolean data type
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        return 0;   // TODO: implement
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

    BDD_ID Reachability::addState() {
        static unsigned long state = 1;
        BDD_ID id = createVar("s" + std::to_string(state));
        states.push_back(id);
        id = createVar("s'" + std::to_string(state));
        next_states.push_back(id);
        state++;
        return id;
    }

    BDD_ID Reachability::addInput() {
        static unsigned long input = 1;
        BDD_ID id = createVar("x" + std::to_string(input));
        inputs.push_back(id);
        input++;
        return id;
    }

    BDD_ID Reachability::reachableStates() {
        // slide 5-10
        BDD_ID tau = transitionRelation(states, inputs, next_states);
        BDD_ID cs0 = characteristicFunction(states, initial_states);

        BDD_ID cR, cR_it;
        cR_it = cs0;
        do {
            cR = cR_it;
            BDD_ID imgRsp = img(cR, tau);
            // form imgR(s) by renaming of variables s' into s; // TODO: change s' into s
            cR_it = or2(cR, imgRsp);
        } while (cR != cR_it);

        return cR;
    }

    BDD_ID Reachability::transitionRelation(std::vector<BDD_ID> &s, std::vector<BDD_ID> &x, std::vector<BDD_ID> &sp) {
        // slide 5-4
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

    BDD_ID Reachability::characteristicFunction(std::vector<BDD_ID> &vars, std::vector<BDD_ID> &values) {
        // c = product of (si == vi) from i=1 to n.
        // with (a == b) = (a xnor b)
        BDD_ID c;
        for (unsigned long i = 0; i < vars.size(); i++) {
            BDD_ID var = vars[i];
            BDD_ID value = values[i];
            c = and2(c, xnor2(var, value));
        }
        return c;
    }

    BDD_ID Reachability::existential_quantification(BDD_ID func, BDD_ID var) {
        // slide 5-5
        // ∃xf = f|x=1 + f|x=0
        BDD_ID f = or2(coFactorTrue(func, var), coFactorFalse(func, var));
        return f;
    }

    BDD_ID Reachability::existential_quantification(BDD_ID func, const std::vector<BDD_ID> &vars) {
        // slide 5-6
        // ∃vi ∈V f = ∃v f = ∃v1 ∃v2 ... ∃vn f
        BDD_ID f = func;
        for (BDD_ID var: vars) {
            f = existential_quantification(f, var);
        }
        return f;
    }

    BDD_ID Reachability::img(BDD_ID cR, BDD_ID tau) {
        // slide 5-8
        // imgR(s') := ∃x ∃s cR(s) ⋅ τ(s, x, s')
        BDD_ID f = and2(cR, tau);
        BDD_ID img = existential_quantification(existential_quantification(f, states), inputs);

        return img;
    }

}