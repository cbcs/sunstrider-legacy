/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef TRINITY_CREATUREAI_H
#define TRINITY_CREATUREAI_H

#include "Define.h"
#include "Dynamic/FactoryHolder.h"
#include "UnitAI.h"

class Unit;
class Creature;
class Player;
class GameObject;
class GameObjectAI;
class SpellInfo;
class PlayerAI;

#define TIME_INTERVAL_LOOK   5000
#define VISIBILITY_RANGE    10000

//Spell targets used by SelectSpell
enum SelectSpellTarget
{
    SELECT_TARGET_DONTCARE = 0,                             //All target types allowed

    SELECT_TARGET_SELF,                                     //Only Self casting

    SELECT_TARGET_SINGLE_ENEMY,                             //Only Single Enemy
    SELECT_TARGET_AOE_ENEMY,                                //Only AoE Enemy
    SELECT_TARGET_ANY_ENEMY,                                //AoE or Single Enemy

    SELECT_TARGET_SINGLE_FRIEND,                            //Only Single Friend
    SELECT_TARGET_AOE_FRIEND,                               //Only AoE Friend
    SELECT_TARGET_ANY_FRIEND,                               //AoE or Single Friend
};

//Spell Effects used by SelectSpell
enum SelectEffect
{
    SELECT_EFFECT_DONTCARE = 0,                             //All spell effects allowed
    SELECT_EFFECT_DAMAGE,                                   //Spell does damage
    SELECT_EFFECT_HEALING,                                  //Spell does healing
    SELECT_EFFECT_AURA,                                     //Spell applies an aura
};

enum SCEquip
{
    EQUIP_NO_CHANGE = -1,
    EQUIP_UNEQUIP   = 0
};

class CreatureAI : public UnitAI
{
    protected:
        Creature *me;

        bool UpdateVictim(bool evade = true);
    public:
        enum EvadeReason
        {
            EVADE_REASON_NO_HOSTILES,       // the creature's threat list is empty
            EVADE_REASON_BOUNDARY,          // the creature has moved outside its evade boundary
            EVADE_REASON_NO_PATH,           // the creature was unable to reach its target for over 5 seconds
            EVADE_REASON_SEQUENCE_BREAK,    // this is a boss and the pre-requisite encounters for engaging it are not defeated yet
            EVADE_REASON_OTHER,
        };

        CreatureAI(Creature *c) : UnitAI((Unit*)c), me(c) {}

        ~CreatureAI() override = default;

        void AttackStartIfCan(Unit* victim);
        void Talk(uint8 id, WorldObject const* whisperTarget = nullptr);
        //Places the entire map into combat with creature
        void DoZoneInCombat(Unit* pUnit = nullptr, bool force = false);
        bool IsInMeleeRange() const;

        //Called when MoveInLineOfSight, check if 'who' is a player or has a player owner, and help him if any of his attackers are in assist range. Return true if started helping.
        virtual bool AssistPlayerInCombatAgainst(Unit* who);

        // Called at each *who move, AND if creature is aggressive
        virtual void MoveInLineOfSight(Unit *);
        
        //Same as MoveInLineOfSight but with is called with every react state (so not only if the creature is aggressive)
        virtual void MoveInLineOfSight2(Unit *) {}

        // Called for reaction at stopping attack at no attackers or targets
        virtual void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER);

        // Called when the creature is killed
        virtual void JustDied(Unit *) {}

        // Called when the creature kills a unit
        virtual void KilledUnit(Unit *) {}

        // Called when the creature summon successfully other creature
        virtual void JustSummoned(Creature* ) {}
        virtual void IsSummonedBy(Unit* /*summoner*/) { }

        virtual void SummonedCreatureDespawn(Creature* /*unit*/) {}
        virtual void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/) {}

        // Called when hit by a spell
        virtual void SpellHit(Unit*, const SpellInfo*) {}

        // Called when spell hits a target
        virtual void SpellHitTarget(Unit* target, const SpellInfo*) {}

        // Called when the creature is target of hostile action: swing, hostile spell landed, fear/etc)
        void AttackedBy(Unit* /*attacker*/) override { }
        virtual bool IsEscorted() const { return false; }
        virtual void AttackedUnitDied(Unit* /* attacked */) { }

        // Called when vitim entered water and creature can not enter water
        virtual bool canReachByRangeAttack(Unit*) { return false; }

        // Called when creature is spawned or respawned (for reseting variables)
        virtual void JustRespawned() {}

        // Called at waypoint reached or point movement finished
        virtual void MovementInform(uint32 /*MovementType*/, uint32 /*Data*/) {}
        
        // Called when creature finishes a spell cast
        virtual void OnSpellFinish(Unit *caster, uint32 spellId, Unit *target, bool ok) {}
        
        // Called when creature reaches its home position
        virtual void JustReachedHome() {}

        // Called for reaction at enter to combat if not in combat yet (enemy can be NULL)
        virtual void EnterCombat(Unit* enemy) {}
        
        virtual void ReceiveEmote(Player* /*player*/, uint32 /*text_emote*/) {}
        
        virtual void DespawnDueToGameEventEnd(int32 /*eventId*/) {}

        // called when the corpse of this creature gets removed
        virtual void CorpseRemoved(uint32& respawnDelay) {}

        void OnCharmed(Unit* charmer, bool apply) override;
        void OnPossess(Unit* charmer, bool apply) override;
        
        // Called when creature's master (pet case) killed a unit
        virtual void MasterKilledUnit(Unit* unit) {}
        
        //called for friendly creatures death FOR UP TO 60m
        virtual void FriendlyKilled(Creature const* c, float range) {}

        virtual bool sOnDummyEffect(Unit* /*caster*/, uint32 /*spellId*/, uint32 /*effIndex*/) { return false; }

        virtual void OnRemove() {}
        
        virtual void OnSpellClick(Unit* /*clicker*/, bool& /*result*/) { }

        /* Script interaction */
        virtual uint64 message(uint32 id, uint64 data) { return 0; }

        // Called when a player is charmed by the creature
        // If a PlayerAI* is returned, that AI is placed on the player instead of the default charm AI
        // Object destruction is handled by Unit::RemoveCharmedBy
        virtual PlayerAI* GetAIForCharmedPlayer(Player* /*who*/) { return nullptr; }

    protected:
        bool _EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER);        
};

struct SelectableAI : public FactoryHolder<CreatureAI>, public Permissible<Creature>
{

    SelectableAI(const char *id) : FactoryHolder<CreatureAI>(id) {}
};

template<class REAL_AI>
struct CreatureAIFactory : public SelectableAI
{
    CreatureAIFactory(const char *name) : SelectableAI(name) {}

    CreatureAI* Create(void *) const override;

    int Permit(const Creature *c) const override { return REAL_AI::Permissible(c); }
};

enum Permitions
{
    PERMIT_BASE_NO                 = -1,
    PERMIT_BASE_IDLE               = 1,
    PERMIT_BASE_REACTIVE           = 100,
    PERMIT_BASE_PROACTIVE          = 200,
    PERMIT_BASE_FACTION_SPECIFIC   = 400,
    PERMIT_BASE_SPECIAL            = 800
};

typedef FactoryHolder<CreatureAI> CreatureAICreator;
typedef FactoryHolder<CreatureAI>::FactoryHolderRegistry CreatureAIRegistry;

#define sCreatureAIRegistry CreatureAIRegistry::instance()

//GO
struct SelectableGameObjectAI : public FactoryHolder<GameObjectAI>, public Permissible<GameObject>
{
    SelectableGameObjectAI(const char *id) : FactoryHolder<GameObjectAI>(id) {}
};

template<class REAL_GO_AI>
struct GameObjectAIFactory : public SelectableGameObjectAI
{
    GameObjectAIFactory(const char *name) : SelectableGameObjectAI(name) {}

    GameObjectAI* Create(void *) const override;

    int Permit(const GameObject *g) const override { return REAL_GO_AI::Permissible(g); }
};

template<class REAL_GO_AI>
inline GameObjectAI*
GameObjectAIFactory<REAL_GO_AI>::Create(void *data) const
{
    GameObject* go = reinterpret_cast<GameObject *>(data);
    return (new REAL_GO_AI(go));
}

typedef FactoryHolder<GameObjectAI> GameObjectAICreator;
typedef FactoryHolder<GameObjectAI>::FactoryHolderRegistry GameObjectAIRegistry;

#define sGameObjectAIRegistry GameObjectAIRegistry::instance()

#endif
