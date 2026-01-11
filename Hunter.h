//
// Created by khaled-sawaid on 11/01/2026.
//

#ifndef DS_WET2_WINTER_2026_01_HUNTER_H
#define DS_WET2_WINTER_2026_01_HUNTER_H

#include "wet2util.h"
#include "Squad.h"

// A Hunter object is stored permanently (even if its squad is removed).
// It never moves between squads; instead, squads are DSU-linked.
//
// Fights:
//   totalFights = baseFights + fightPotential(blockSquad)
//
// Partial Nen Ability:
//   localPrefixAtJoin + nenShiftToRoot(blockSquad) + ability

struct Hunter {
    int id;
    NenAbility ability;
    int aura;

    // Base fights relative to squad root at insertion time
    int baseFights;

    // Nen prefix inside the squad at join time (chronological order)
    NenAbility localPrefixAtJoin;

    // The squad-block this hunter originally joined (DSU node)
    Squad* blockSquad;

    Hunter(int hunterId,
           const NenAbility& nen,
           int aura_,
           int baseF,
           const NenAbility& localPrefix,
           Squad* squadBlock)
        : id(hunterId),
          ability(nen),
          aura(aura_),
          baseFights(baseF),
          localPrefixAtJoin(localPrefix),
          blockSquad(squadBlock)
    {}
};

#endif // DS_WET2_WINTER_2026_01_HUNTER_H
