//
// Created by khaled-sawaid on 11/01/2026.
//

#ifndef DS_WET2_WINTER_2026_01_SQUAD_H
#define DS_WET2_WINTER_2026_01_SQUAD_H

#include "wet2util.h"

// A Squad object is both:
// 1) the entity stored in active squad trees
// 2) a DSU node (for force-join chaining without updating all hunters)
//
// DSU "potentials":
// - fightOffsetToParent: integer offset so hunters keep correct fights after joins
// - nenOffsetToParent: NenAbility offset representing the prefix added before this block
//
// Only DSU roots use fightsAddRoot (lazy +1 to all hunters in that root).

struct Squad {
    int id;
    bool alive;      // false means the squad (and all its hunters) are "dead"
    int experience;

    int huntersCount;
    long long auraSum;
    NenAbility nenSum;

    // DSU:
    Squad* parent;
    int fightOffsetToParent;
    NenAbility nenOffsetToParent;

    // only meaningful at DSU root:
    int fightsAddRoot;

    explicit Squad(int squadId)
        : id(squadId),
          alive(true),
          experience(0),
          huntersCount(0),
          auraSum(0),
          nenSum(NenAbility::zero()),
          parent(nullptr),
          fightOffsetToParent(0),
          nenOffsetToParent(NenAbility::zero()),
          fightsAddRoot(0)
    {}

    int effectiveNen() const {
        return nenSum.getEffectiveNenAbility();
    }
};

#endif // DS_WET2_WINTER_2026_01_SQUAD_H
