// You can edit anything you want in this file.
// However, you need to implement all public Huntech functions, which are provided below as a template.

#include "Huntech26a2.h"

Huntech::Huntech()
    : squadsById(),
      squadsByAura(),
      huntersById(),
      allSquads(nullptr),
      allHunters(nullptr)
{}

Huntech::~Huntech() {
    freeAll();
}

void Huntech::freeAll() {
    // Clear trees (deletes only tree nodes, not the Squad*/Hunter* themselves)
    squadsById.clear();
    squadsByAura.clear();

    // Delete allocated hunters
    while (allHunters) {
        HunterNode* n = allHunters;
        allHunters = allHunters->next;
        delete n->h;
        delete n;
    }

    // Delete allocated squads
    while (allSquads) {
        SquadNode* n = allSquads;
        allSquads = allSquads->next;
        delete n->s;
        delete n;
    }
}

// ---------- DSU helpers (with potentials) ----------

Squad* Huntech::findSquad(Squad* x) {
    if (!x) return nullptr;
    if (!x->parent) return x;

    Squad* p = x->parent;
    Squad* r = findSquad(p);

    // Path compression with potential accumulation:
    x->fightOffsetToParent += p->fightOffsetToParent;
    x->nenOffsetToParent += p->nenOffsetToParent;
    x->parent = r;

    return r;
}

int Huntech::fightPotential(Squad* x) {
    Squad* r = findSquad(x);
    // after compression, x->fightOffsetToParent is offset-to-root
    return r->fightsAddRoot + x->fightOffsetToParent;
}

NenAbility Huntech::nenShiftToRoot(Squad* x) {
    (void)findSquad(x);
    // after compression, x->nenOffsetToParent is shift-to-root
    return x->nenOffsetToParent;
}

// ---------- Required API ----------

StatusType Huntech::add_squad(int squadId) {
    if (squadId <= 0) return StatusType::INVALID_INPUT;

    try {
        if (squadsById.find(squadId) != nullptr) return StatusType::FAILURE;

        Squad* s = new Squad(squadId);
        allSquads = new SquadNode(s, allSquads);

        if (!squadsById.insert(squadId, s)) return StatusType::FAILURE;

        AuraKey k(s->auraSum, s->id);
        (void)squadsByAura.insert(k, s);

        return StatusType::SUCCESS;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Huntech::remove_squad(int squadId) {
    if (squadId <= 0) return StatusType::INVALID_INPUT;

    try {
        Squad** ps = squadsById.find(squadId);
        if (!ps) return StatusType::FAILURE;

        Squad* s = *ps;

        // remove from aura-rank tree
        AuraKey key(s->auraSum, s->id);
        (void)squadsByAura.remove(key);

        // remove from id tree (active squads)
        (void)squadsById.remove(squadId);

        // mark DSU root as dead (kills all hunters under it)
        Squad* r = findSquad(s);
        r->alive = false;

        return StatusType::SUCCESS;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType Huntech::add_hunter(int hunterId,
                               int squadId,
                               const NenAbility &nenType,
                               int aura,
                               int fightsHad)
{
    if (hunterId <= 0 || squadId <= 0 || !nenType.isValid() || aura < 0 || fightsHad < 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        if (huntersById.find(hunterId) != nullptr) return StatusType::FAILURE;

        Squad** ps = squadsById.find(squadId);
        if (!ps) return StatusType::FAILURE;

        Squad* s = *ps;
        Squad* r = findSquad(s);
        if (!r->alive) return StatusType::FAILURE;

        // Update aura tree: remove old aura key for root
        AuraKey oldKey(r->auraSum, r->id);
        (void)squadsByAura.remove(oldKey);

        // base fights relative to current root lazy fights
        int fightsNow = fightPotential(r); // r is root => fightsAddRoot
        int baseF = fightsHad - fightsNow;

        // local prefix at join time: current full nenSum (append at end)
        NenAbility localPrefix = r->nenSum;

        Hunter* h = new Hunter(hunterId, nenType, aura, baseF, localPrefix, r);
        allHunters = new HunterNode(h, allHunters);

        if (!huntersById.insert(hunterId, h)) return StatusType::FAILURE;

        // update squad aggregates
        r->huntersCount += 1;
        r->auraSum += (long long)aura;
        r->nenSum += nenType;

        // reinsert updated aura key
        AuraKey newKey(r->auraSum, r->id);
        (void)squadsByAura.insert(newKey, r);

        return StatusType::SUCCESS;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}

output_t<int> Huntech::squad_duel(int squadId1, int squadId2) {
    if (squadId1 <= 0 || squadId2 <= 0 || squadId1 == squadId2) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    try {
        Squad** p1 = squadsById.find(squadId1);
        Squad** p2 = squadsById.find(squadId2);
        if (!p1 || !p2) return output_t<int>(StatusType::FAILURE);

        Squad* s1 = findSquad(*p1);
        Squad* s2 = findSquad(*p2);

        if (!s1->alive || !s2->alive) return output_t<int>(StatusType::FAILURE);
        if (s1->huntersCount == 0 || s2->huntersCount == 0) return output_t<int>(StatusType::FAILURE);

        long long eff1 = (long long)s1->experience + s1->auraSum;
        long long eff2 = (long long)s2->experience + s2->auraSum;

        int res = 0;

        if (eff1 > eff2) {
            s1->experience += 3;
            res = 1;
        } else if (eff2 > eff1) {
            s2->experience += 3;
            res = 3;
        } else {
            if (s1->nenSum > s2->nenSum) {
                s1->experience += 3;
                res = 2;
            } else if (s2->nenSum > s1->nenSum) {
                s2->experience += 3;
                res = 4;
            } else {
                s1->experience += 1;
                s2->experience += 1;
                res = 0;
            }
        }

        // every hunter in both squads fought +1 (lazy at root)
        s1->fightsAddRoot += 1;
        s2->fightsAddRoot += 1;

        return output_t<int>(res);
    } catch (const std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> Huntech::get_hunter_fights_number(int hunterId) {
    if (hunterId <= 0) return output_t<int>(StatusType::INVALID_INPUT);

    try {
        Hunter** ph = huntersById.find(hunterId);
        if (!ph) return output_t<int>(StatusType::FAILURE);

        Hunter* h = *ph;
        int fights = h->baseFights + fightPotential(h->blockSquad);
        return output_t<int>(fights);
    } catch (const std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> Huntech::get_squad_experience(int squadId) {
    if (squadId <= 0) return output_t<int>(StatusType::INVALID_INPUT);

    try {
        Squad** ps = squadsById.find(squadId);
        if (!ps) return output_t<int>(StatusType::FAILURE);

        Squad* r = findSquad(*ps);
        if (!r->alive) return output_t<int>(StatusType::FAILURE);

        return output_t<int>(r->experience);
    } catch (const std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> Huntech::get_ith_collective_aura_squad(int i) {
    try {
        int n = squadsByAura.size();
        if (i < 1 || i > n) return output_t<int>(StatusType::FAILURE);

        const AVLTree<AuraKey, Squad*, AuraKeyLess>::Node* node = squadsByAura.select(i);
        if (!node) return output_t<int>(StatusType::FAILURE);

        return output_t<int>(node->value->id);
    } catch (const std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<NenAbility> Huntech::get_partial_nen_ability(int hunterId) {
    if (hunterId <= 0) return output_t<NenAbility>(StatusType::INVALID_INPUT);

    try {
        Hunter** ph = huntersById.find(hunterId);
        if (!ph) return output_t<NenAbility>(StatusType::FAILURE);

        Hunter* h = *ph;

        Squad* r = findSquad(h->blockSquad);
        if (!r->alive) return output_t<NenAbility>(StatusType::FAILURE);

        NenAbility shift = nenShiftToRoot(h->blockSquad);
        NenAbility ans = h->localPrefixAtJoin + shift + h->ability;

        return output_t<NenAbility>(ans);
    } catch (const std::bad_alloc&) {
        return output_t<NenAbility>(StatusType::ALLOCATION_ERROR);
    }
}

StatusType Huntech::force_join(int forcingSquadId, int forcedSquadId) {
    if (forcingSquadId <= 0 || forcedSquadId <= 0 || forcingSquadId == forcedSquadId) {
        return StatusType::INVALID_INPUT;
    }

    try {
        Squad** pA = squadsById.find(forcingSquadId);
        Squad** pB = squadsById.find(forcedSquadId);
        if (!pA || !pB) return StatusType::FAILURE;

        Squad* A = findSquad(*pA);
        Squad* B = findSquad(*pB);

        if (!A->alive || !B->alive) return StatusType::FAILURE;

        // forcing squad cannot be empty
        if (A->huntersCount == 0) return StatusType::FAILURE;

        // If B is not empty, must satisfy the force condition
        if (B->huntersCount != 0) {
            long long left  = (long long)A->experience + A->auraSum + (long long)A->effectiveNen();
            long long right = (long long)B->experience + B->auraSum + (long long)B->effectiveNen();
            if (!(left > right)) return StatusType::FAILURE;
        }

        // remove both from aura tree before changing A's aura
        AuraKey keyA(A->auraSum, A->id);
        AuraKey keyB(B->auraSum, B->id);
        (void)squadsByAura.remove(keyA);
        (void)squadsByAura.remove(keyB);

        // DSU directed union: B becomes child of A

        // Preserve fights for hunters in B:
        // old fights added for B-set was B->fightsAddRoot
        // new fights added will be A->fightsAddRoot + fightOffsetToParent
        // choose offset so equality holds:
        B->fightOffsetToParent = B->fightsAddRoot - A->fightsAddRoot;

        // Chronological order: all A hunters precede all B hunters
        // so B block gets an additional prefix = current nenSum(A)
        B->nenOffsetToParent = A->nenSum;

        B->parent = A;

        // merge aggregates into A
        A->experience += B->experience;
        A->huntersCount += B->huntersCount;
        A->auraSum += B->auraSum;
        A->nenSum += B->nenSum;

        // forced squad is removed from active-id structure
        (void)squadsById.remove(forcedSquadId);

        // insert updated A into aura tree
        AuraKey newKey(A->auraSum, A->id);
        (void)squadsByAura.insert(newKey, A);

        return StatusType::SUCCESS;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
}
