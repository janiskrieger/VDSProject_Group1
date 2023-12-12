//
// Created by Carolina P. Nogueira 2016
//

#include <iostream>
#include <string>
#include "Manager.h"


int main(int argc, char *argv[]) {
    // ROBDD Construction Example
    ClassProject::Manager m = ClassProject::Manager();

    // create variables
    ClassProject::BDD_ID a = m.createVar("a");
    ClassProject::BDD_ID b = m.createVar("b");
    ClassProject::BDD_ID c = m.createVar("c");
    ClassProject::BDD_ID d = m.createVar("d");

    // f = (a + b) * c * d
    ClassProject::BDD_ID f = m.and2(m.or2(a, b), m.and2(c, d));

    // create DOT language graph representation
    m.visualizeBDD("ROBDD.txt", f);
}
