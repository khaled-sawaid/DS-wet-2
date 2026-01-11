//
// 234218 Data Structures.
// Semester: 2026 A (Winter).
// Wet Exercise #2 — Huntech.
//
// This main file is necessary to link and run your code.
// This file is READ ONLY: Submitting something else will not result
// in compiling your code with other than this file.
//
// DO NOT use the preprocessors in your own code files.
//

#include "Huntech26a2.h"
#include <string>
#include <iostream>

using namespace std;

void print(string cmd, StatusType res);
void print(string cmd, output_t<int> res);
void print(string cmd, output_t<NenAbility> res);

int main()
{
    int d1, d2, d3, d4;
    string nenTypeStr;

    // Init
    Huntech *obj = new Huntech();

    // Read and execute all commands from input
    string op;
    while (cin >> op)
    {
        if (!op.compare("addSquad")) {
            cin >> d1;
            print(op, obj->add_squad(d1));

        } else if (!op.compare("removeSquad")) {
            cin >> d1;
            print(op, obj->remove_squad(d1));

        } else if (!op.compare("addHunter")) {
            // addHunter <hunterId> <squadId> <nenTypeString> <aura> <fightsHad>
            cin >> d1 >> d2 >> nenTypeStr >> d3 >> d4;
            NenAbility nenAbility(nenTypeStr);
            print(op, obj->add_hunter(d1, d2, nenAbility, d3, d4));

        } else if (!op.compare("squadDuel")) {
            cin >> d1 >> d2;
            print(op, obj->squad_duel(d1, d2));

        } else if (!op.compare("getHunterFightsNumber")) {
            cin >> d1;
            print(op, obj->get_hunter_fights_number(d1));

        } else if (!op.compare("getSquadExperience")) {
            cin >> d1;
            print(op, obj->get_squad_experience(d1));

        } else if (!op.compare("getIthCollectiveAuraSquad")) {
            cin >> d1;
            print(op, obj->get_ith_collective_aura_squad(d1));

        } else if (!op.compare("getPartialNenAbility")) {
            cin >> d1;
            print(op, obj->get_partial_nen_ability(d1));

        } else if (!op.compare("forceJoin")) {
            cin >> d1 >> d2;
            print(op, obj->force_join(d1, d2));

        } else {
            cout << "Unknown command: " << op << endl;
            break;
        }
        // Verify no faults
        if (cin.fail()) {
            cout << "Invalid input format" << endl;
            break;
        }
    }

    // Quit
    delete obj;
    return 0;
}

// -------------------- Helpers --------------------

static const char *StatusTypeStr[] =
{
    "SUCCESS",
    "ALLOCATION_ERROR",
    "INVALID_INPUT",
    "FAILURE"
};

void print(string cmd, StatusType res)
{
    cout << cmd << ": " << StatusTypeStr[(int) res] << endl;
}

void print(string cmd, output_t<int> res)
{
    if (res.status() == StatusType::SUCCESS) {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()]
             << ", " << res.ans() << endl;
    } else {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << endl;
    }
}

void print(string cmd, output_t<NenAbility> res)
{
    if (res.status() == StatusType::SUCCESS) {
        // requires operator<< for NenAbility
        cout << cmd << ": " << StatusTypeStr[(int) res.status()]
             << ", " << res.ans() << endl;
    } else {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << endl;
    }
}