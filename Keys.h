//
// Created by khaled-sawaid on 11/01/2026.

#ifndef DS_WET2_WINTER_2026_01_KEYS_H
#define DS_WET2_WINTER_2026_01_KEYS_H

// Key used for ordering squads by collective aura.
// Primary key: total aura
// Secondary key: squadId (tie-breaker)

struct AuraKey {
    long long aura;
    int squadId;

    AuraKey(long long a = 0, int id = 0)
        : aura(a), squadId(id) {}
};

// Comparator for AuraKey
// Orders by (aura ASC, squadId ASC)
struct AuraKeyLess {
    bool operator()(const AuraKey& x, const AuraKey& y) const {
        if (x.aura < y.aura) return true;
        if (x.aura > y.aura) return false;
        return x.squadId < y.squadId;
    }
};

#endif // DS_WET2_WINTER_2026_01_KEYS_H
