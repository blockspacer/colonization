#include <Colonization/BuildConfiguration.hpp>
#include "BattlesProcessor.hpp"
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/Context.h>

#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

#include <Colonization/Backend/PlayersManager.hpp>
#include <Colonization/Backend/Diplomacy/DiplomacyProcessor.hpp>
#include <Colonization/Backend/Diplomacy/DiplomacyEvents.hpp>
#include <Colonization/Backend/UnitsManager.hpp>
#include <Colonization/Backend/Player/Player.hpp>

#include <Colonization/Core/District/District.hpp>
#include <Colonization/Core/Unit/Unit.hpp>
#include <Colonization/Core/Unit/UnitEvents.hpp>
#include <Colonization/Core/GameConfiguration.hpp>
#include <Colonization/Core/Battle/BattleHelpers.hpp>

#include <Colonization/Utils/Network/NetworkUpdateCounterUtils.hpp>
#include <Colonization/Utils/Serialization/Categories.hpp>
#include <Colonization/Utils/Serialization/AttributeMacro.hpp>

namespace Colonization
{
BattlesProcessor::BattlesProcessor (Urho3D::Context *context) : Urho3D::Component (context),
    battles_ ()
{

}

BattlesProcessor::~BattlesProcessor ()
{

}

void BattlesProcessor::RegisterObject (Urho3D::Context *context)
{
    context->RegisterFactory <BattlesProcessor> (COLONIZATION_SERVER_ONLY_CATEGORY);
    URHO3D_ACCESSOR_ATTRIBUTE ("Is Enabled", IsEnabled, SetEnabled, bool, true, Urho3D::AM_DEFAULT);
}

void BattlesProcessor::Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    if (enabled_)
    {
        UpdateBattlesList ();
        float timeStep = eventData [Urho3D::SceneUpdate::P_TIMESTEP].GetFloat ();
        Urho3D::Vector <Urho3D::SharedPtr <Battle> >::Iterator iterator = battles_.Begin ();

        while (iterator != battles_.End ())
        {
            if (!ProcessBattle (iterator->Get (), timeStep))
            {
                DeleteBattle (iterator->Get ());
                iterator = battles_.Erase (iterator);
            }
            else
            {
                iterator++;
            }
        }
    }
}

void BattlesProcessor::HandleUnitCreated (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    if (enabled_)
    {
        HandleUnitPositionChangedOrCreated (eventData);
    }
}

void BattlesProcessor::HandleUnitPositionChanged (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    if (enabled_)
    {
        HandleUnitPositionChangedOrCreated (eventData);
    }
}

void BattlesProcessor::HandleTradersUnitLossesGold (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    if (enabled_)
    {
        UnitsManager *unitsManager = node_->GetScene ()->GetChild ("units")->GetComponent <UnitsManager> ();
        GameConfiguration *configuration = node_->GetScene ()->GetComponent <GameConfiguration> ();
        PlayersManager *playersManager = node_->GetScene ()->GetChild ("players")->GetComponent <PlayersManager> ();

        Battle *battle = GetBattleByHash (eventData [TradersUnitLossesGold::BATTLE_HASH].GetStringHash ());
        assert (battle);
        Urho3D::StringHash tradersHash = eventData [TradersUnitLossesGold::UNIT_HASH].GetStringHash ();
        assert (battle->IsAttackerUnit (tradersHash) || battle->IsDefenderUnit (tradersHash));

        bool isLootersAttackers;
        if (battle->IsAttackerUnit (tradersHash))
        {
            isLootersAttackers = false;
        }
        else
        {
            isLootersAttackers = true;
        }

        float goldPerUnit = eventData [TradersUnitLossesGold::GOLD_AMOUNT].GetFloat () /
                BattleHelpers::GetUnitsCountInBattle (battle, isLootersAttackers);
        Urho3D::PODVector <Urho3D::StringHash> looters = BattleHelpers::GetUnitsInBattleList (battle, isLootersAttackers);

        for (int index = 0; index < looters.Size (); index++)
        {
            Unit *enemy = unitsManager->GetUnitByHash (looters.At (index));
            assert (enemy);
            Player *enemyPlayer = playersManager->GetPlayerByNameHash (Urho3D::StringHash (enemy->GetOwnerPlayerName ()));
            assert (enemyPlayer);
            enemyPlayer->SetGold (enemyPlayer->GetGold () + goldPerUnit * configuration->GetLootingCoefficient ());
        }
    }
}

void BattlesProcessor::HandlePlayerWillBeDisconnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    UnitsManager *unitsManager = node_->GetScene ()->GetChild ("units")->GetComponent <UnitsManager> ();
    Player *player = static_cast <Player *> (eventData [PlayerWillBeDisconnected::PLAYER].GetPtr ());
    Urho3D::PODVector <Urho3D::StringHash> playerUnitsHashes = unitsManager->GetUnitsOfPlayer (Urho3D::StringHash (player->GetName ()));

    // Because disconnected player units will be deleted anyway, we can simply delete their hashes from battles.
    for (int index = 0; index < battles_.Size (); index++)
    {
        Battle *battle = battles_.At (index);
        bool isBattleChanged = false;
        for (int unitHashIndex = 0; unitHashIndex < playerUnitsHashes.Size (); unitHashIndex++)
        {
            if (BattleHelpers::RemoveUnitWithUnkwnownAlignmentFromBattle (battle, playerUnitsHashes.At (unitHashIndex)))
            {
                isBattleChanged = true;
            }
        }

        if (isBattleChanged)
        {
            NetworkUpdateCounterUtils::AddNetworkUpdatePointsToComponentCounter (battle, 100.0f);
        }
    }
}

void BattlesProcessor::HandleWarStarted (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    UnitsManager *unitsManager = node_->GetScene ()->GetChild ("units")->GetComponent <UnitsManager> ();
    DiplomacyWar *war = static_cast <DiplomacyWar *> (eventData [WarStarted::WAR].GetPtr ());

    // To create new battles, we simply imitate each attackers unit reposition.
    for (int index = 0; index < war->GetAttackersCount (); index++)
    {
        Urho3D::PODVector <Urho3D::StringHash> playerUnitsHashes =
                unitsManager->GetUnitsOfPlayer (war->GetAttackerNameHashByIndex (index));
        for (int unitIndex = 0; unitIndex < playerUnitsHashes.Size (); unitIndex++)
        {
            Urho3D::VariantMap positionChangedEventData;
            positionChangedEventData [UnitPositionChanged::UNIT_HASH] = playerUnitsHashes.At (unitIndex);
            SendEvent (EVENT_UNIT_POSITION_CHANGED, positionChangedEventData);
        }
    }
}

void BattlesProcessor::HandleWarEnded (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    DiplomacyWar *war = static_cast <DiplomacyWar *> (eventData [WarEnded::WAR].GetPtr ());
    Urho3D::Vector <Urho3D::SharedPtr <Battle> >::Iterator iterator = battles_.Begin ();

    while (iterator != battles_.End ())
    {
        if (iterator->Get ()->GetWarHash () == war->GetHash ())
        {
            DeleteBattle (iterator->Get ());
            iterator = battles_.Erase (iterator);
        }
        else
        {
            iterator++;
        }
    }
}

int BattlesProcessor::GetBattlesCount () const
{
    return battles_.Size ();
}

void BattlesProcessor::UpdateBattlesList ()
{
    // Reload battles array from child nodes.
    assert (node_);
    battles_.Clear ();
    Urho3D::PODVector <Urho3D::Node *> battlesNodes;
    node_->GetChildrenWithTag (battlesNodes, TAG_BATTLE);
    for (int index = 0; index < battlesNodes.Size (); index++)
    {
        Urho3D::Node *battleNode = battlesNodes.At (index);
        if (battleNode->GetID () < Urho3D::FIRST_LOCAL_ID)
        {
            Battle *battle = battleNode->GetComponent <Battle> ();
            if (battle)
            {
                battles_.Push (Urho3D::SharedPtr <Battle> (battle));
            }
        }
    }
}

Battle *BattlesProcessor::GetBattleByIndex (int index) const
{
    assert (index < battles_.Size ());
    return battles_.At (index);
}

Battle *BattlesProcessor::GetBattleByHash (Urho3D::StringHash battleHash) const
{
    for (int index = 0; index < battles_.Size (); index++)
    {
        Battle *battle = battles_.At (index);
        if (battle->GetHash () == battleHash)
        {
            return battle;
        }
    }
    return 0;
}

Urho3D::PODVector <Urho3D::StringHash> BattlesProcessor::GetCurrentBattlesOfWar (Urho3D::StringHash warHash) const
{
    Urho3D::PODVector <Urho3D::StringHash> currentBattlesOfWar;
    for (int index = 0; index < battles_.Size (); index++)
    {
        Battle *battle = battles_.At (index);
        if (battle->GetWarHash () == warHash)
        {
            currentBattlesOfWar.Push (battle->GetHash ());
        }
    }
    return currentBattlesOfWar;
}

Urho3D::PODVector <Urho3D::StringHash> BattlesProcessor::GetCurrentBattlesInDistrict (Urho3D::StringHash districtHash) const
{
    Urho3D::PODVector <Urho3D::StringHash> currentBattlesInDistrict;
    for (int index = 0; index < battles_.Size (); index++)
    {
        Battle *battle = battles_.At (index);
        if (battle->GetDistrictHash () == districtHash)
        {
            currentBattlesInDistrict.Push (battle->GetHash ());
        }
    }
    return currentBattlesInDistrict;
}

Urho3D::PODVector <Battle *> GetBattlesInDistrict (Urho3D::Scene *scene, Urho3D::StringHash districtHash)
{
    Urho3D::PODVector <Battle *> battlesInDistrict;
    Urho3D::PODVector <Urho3D::Node *> battlesNodes;
    scene->GetChild ("battles")->GetChildrenWithTag (battlesNodes, TAG_BATTLE);

    for (int index = 0; index < battlesNodes.Size (); index++)
    {
        Urho3D::Node *battleNode = battlesNodes.At (index);
        if (battleNode->GetID () < Urho3D::FIRST_LOCAL_ID)
        {
            Battle *battle = battleNode->GetComponent <Battle> ();
            assert (battle);
            if (battle && battle->GetDistrictHash () == districtHash)
            {
                battlesInDistrict.Push (battle);
            }
        }
    }
    return battlesInDistrict;
}

void BattlesProcessor::OnSceneSet (Urho3D::Scene *scene)
{
    UnsubscribeFromAllEvents ();
    Urho3D::Component::OnSceneSet (scene);

    SubscribeToEvent (scene, Urho3D::E_SCENEUPDATE, URHO3D_HANDLER (BattlesProcessor, Update));
    SubscribeToEvent (EVENT_UNIT_CREATED, URHO3D_HANDLER (BattlesProcessor, HandleUnitCreated));
    SubscribeToEvent (EVENT_UNIT_POSITION_CHANGED, URHO3D_HANDLER (BattlesProcessor, HandleUnitPositionChanged));
    SubscribeToEvent (EVENT_TRADERS_UNIT_LOSSES_GOLD, URHO3D_HANDLER (BattlesProcessor, HandleTradersUnitLossesGold));

    SubscribeToEvent (EVENT_PLAYER_WILL_BE_DISCONNECTED, URHO3D_HANDLER (BattlesProcessor, HandlePlayerWillBeDisconnected));
    SubscribeToEvent (EVENT_WAR_STARTED, URHO3D_HANDLER (BattlesProcessor, HandleWarStarted));
    SubscribeToEvent (EVENT_WAR_ENDED, URHO3D_HANDLER (BattlesProcessor, HandleWarEnded));
}

void BattlesProcessor::HandleUnitPositionChangedOrCreated (Urho3D::VariantMap &eventData)
{
    Map *map = node_->GetScene ()->GetChild ("map")->GetComponent <Map> ();
    UnitsManager *unitsManager = node_->GetScene ()->GetChild ("units")->GetComponent <UnitsManager> ();
    DiplomacyProcessor *diplomacyProcessor = node_->GetScene ()->GetChild ("diplomacy")->GetComponent <DiplomacyProcessor> ();
    PlayersManager *playersManager = node_->GetScene ()->GetChild ("players")->GetComponent <PlayersManager> ();

    Unit *unit = unitsManager->GetUnitByHash (eventData [UnitPositionChanged::UNIT_HASH].GetStringHash ());
    assert (unit);
    District *district = map->GetDistrictByHash (unit->GetPositionHash ());
    assert (district);
    Player *player = playersManager->GetPlayerByNameHash (Urho3D::StringHash (unit->GetOwnerPlayerName ()));

    AddUnitToBattleIfNeeded (unit, district, player, diplomacyProcessor, unitsManager) ||
            CreateNewBattleIfNeeded (unit, district, player, diplomacyProcessor, unitsManager);
}

bool BattlesProcessor::AddUnitToBattleIfNeeded (Unit *unit, District *unitPosition, Player *unitPlayer,
                                                DiplomacyProcessor *diplomacyProcessor, UnitsManager *unitsManager)
{
    Urho3D::PODVector <Urho3D::StringHash> battlesInDistrict = GetCurrentBattlesInDistrict (unitPosition->GetHash ());
    Urho3D::PODVector <Urho3D::StringHash> warsOfPlayer = diplomacyProcessor->GetWarsOfPlayer (
                Urho3D::StringHash (unitPlayer->GetName ()));

    if (!battlesInDistrict.Empty () && !warsOfPlayer.Empty ())
    {
        for (int index = 0; index < battlesInDistrict.Size (); index++)
        {
            Battle *battle = GetBattleByHash (battlesInDistrict.At (index));
            if (warsOfPlayer.Contains (battle->GetWarHash ()))
            {
                DiplomacyWar *war = diplomacyProcessor->GetWarByHash (battle->GetWarHash ());
                assert (war);
                bool isAttackerInWar = war->IsAttacker (Urho3D::StringHash (unitPlayer->GetName ()));

                Unit *firstAttackersUnit = unitsManager->GetUnitByHash (battle->GetAttackerUnitHashByIndex (0));
                bool isFirstAttackerUnitAttackerInWar = war->IsAttacker (
                            Urho3D::StringHash (firstAttackersUnit->GetOwnerPlayerName ()));

                if (isAttackerInWar == isFirstAttackerUnitAttackerInWar)
                {
                    battle->AddAttackerUnitHash (unit->GetHash ());
                }
                else
                {
                    battle->AddDefenderUnitHash (unit->GetHash ());
                }

                unit->SetIsInBattle (true);
                unit->SetBattleHash (battle->GetHash ());
                NetworkUpdateCounterUtils::AddNetworkUpdatePointsToComponentCounter (unit, 100.0f);
                NetworkUpdateCounterUtils::AddNetworkUpdatePointsToComponentCounter (battle, 100.0f);
                return true;
            }
        }
    }
    return false;
}

bool BattlesProcessor::CreateNewBattleIfNeeded (Unit *unit, District *district, Player *unitPlayer,
                                                DiplomacyProcessor *diplomacyProcessor, UnitsManager *unitsManager)
{
    Urho3D::PODVector <Urho3D::StringHash> warsOfPlayer = diplomacyProcessor->GetWarsOfPlayer (
                Urho3D::StringHash (unitPlayer->GetName ()));

    if (!warsOfPlayer.Empty ())
    {
        Urho3D::PODVector <Unit *> unitsInDistrict = GetUnitsInDistrict (node_->GetScene (), district->GetHash ());
        Urho3D::HashMap <Urho3D::StringHash, Urho3D::PODVector <Unit *> > playersUnits;

        for (int index = 0; index < unitsInDistrict.Size (); index++)
        {
            Unit *unit = unitsInDistrict.At (index);
            if (!unit->GetIsInBattle ())
            {
                playersUnits [Urho3D::StringHash (unit->GetOwnerPlayerName ())].Push (unit);
            }
        }

        for (int warIndex = 0; warIndex < warsOfPlayer.Size (); warIndex++)
        {
            DiplomacyWar *war = diplomacyProcessor->GetWarByHash (warsOfPlayer.At (warIndex));
            assert (war);
            bool isAttackerInWar = war->IsAttacker (Urho3D::StringHash (unitPlayer->GetName ()));
            if (isAttackerInWar || war->IsDefender (Urho3D::StringHash (unitPlayer->GetName ())))
            {
                Urho3D::PODVector <Urho3D::StringHash> willBeAttackers;
                Urho3D::PODVector <Urho3D::StringHash> willBeDefenders;
                Urho3D::Vector <Urho3D::StringHash> playersList = playersUnits.Keys ();
                SortAttackersAndDefendersInBattle (war, isAttackerInWar, playersList, willBeAttackers, willBeDefenders);

                if (!willBeAttackers.Empty () && !willBeDefenders.Empty ())
                {
                    Battle *battle = CreateBattle (war->GetHash (), district->GetHash ());
                    InitNewBattle (battle, playersUnits, willBeAttackers, willBeDefenders);
                    return true;
                }
            }
        }
        return false;
    }
}

void BattlesProcessor::SortAttackersAndDefendersInBattle (DiplomacyWar *war, bool isNewUnitPlayerAttackerInWar,
                                                          Urho3D::Vector <Urho3D::StringHash> &playersList,
                                                          Urho3D::PODVector <Urho3D::StringHash> &willBeAttackers,
                                                          Urho3D::PODVector <Urho3D::StringHash> &willBeDefenders)
{
    for (int playerIndex = 0; playerIndex < playersList.Size (); playerIndex++)
    {
        Urho3D::StringHash playerNameHash = playersList.At (playerIndex);
        bool isCurrentScannedAttackerInWar = war->IsAttacker (playerNameHash);
        if (isCurrentScannedAttackerInWar || war->IsDefender (playerNameHash))
        {
            if (isNewUnitPlayerAttackerInWar == isCurrentScannedAttackerInWar)
            {
                willBeAttackers.Push (playerNameHash);
            }
            else
            {
                willBeDefenders.Push (playerNameHash);
            }
        }
    }
}

void BattlesProcessor::InitNewBattle (Battle *battle,
                                      Urho3D::HashMap <Urho3D::StringHash, Urho3D::PODVector <Unit *> > &unitsInDistrict,
                                      Urho3D::PODVector <Urho3D::StringHash> &willBeAttackers,
                                      Urho3D::PODVector <Urho3D::StringHash> &willBeDefenders)
{
    for (int attackerIndex = 0; attackerIndex < willBeAttackers.Size (); attackerIndex++)
    {
        Urho3D::StringHash playerNameHash = willBeAttackers.At (attackerIndex);
        Urho3D::PODVector <Unit *> units = unitsInDistrict [playerNameHash];
        for (int unitIndex = 0; unitIndex < units.Size (); unitIndex++)
        {
            Unit *unit = units.At (unitIndex);
            battle->AddAttackerUnitHash (unit->GetHash ());
            unit->SetIsInBattle (true);
            unit->SetBattleHash (battle->GetHash ());
        }
    }

    for (int defenderIndex = 0; defenderIndex < willBeDefenders.Size (); defenderIndex++)
    {
        Urho3D::StringHash playerNameHash = willBeDefenders.At (defenderIndex);
        Urho3D::PODVector <Unit *> units = unitsInDistrict [playerNameHash];
        for (int unitIndex = 0; unitIndex < units.Size (); unitIndex++)
        {
            Unit *unit = units.At (unitIndex);
            battle->AddDefenderUnitHash (unit->GetHash ());
            unit->SetIsInBattle (true);
            unit->SetBattleHash (battle->GetHash ());
        }
    }
}

bool BattlesProcessor::ProcessBattle (Battle *battle, float timeStep)
{
    UnitsManager *unitsManager = node_->GetScene ()->GetChild ("units")->GetComponent <UnitsManager> ();
    GameConfiguration *configuration = node_->GetScene ()->GetComponent <GameConfiguration> ();
    Map *map = node_->GetScene ()->GetChild ("map")->GetComponent <Map> ();
    District *district = map->GetDistrictByHash (battle->GetDistrictHash ());
    assert (district);

    const unsigned attackersCountBefore = battle->GetAttackersUnitsCount ();
    const unsigned defendersCountBefore = battle->GetDefendersUnitsCount ();
    Urho3D::PODVector <Unit *> attackers = ReconstructUnits (unitsManager, BattleHelpers::GetUnitsInBattleList (battle, true));
    Urho3D::PODVector <Unit *> defenders = ReconstructUnits (unitsManager, BattleHelpers::GetUnitsInBattleList (battle, false));

    float attackersAttackForce = timeStep * CalculateUnitsAttackForce (attackers, configuration, district->GetIsSea ());
    float defendersAttackForce = timeStep * CalculateUnitsAttackForce (defenders, configuration, district->GetIsSea ());
    ApplyDamage (battle, configuration, defendersAttackForce, attackers, true, timeStep * 100.0f);

    float districtDefense = district->GetIsSea () ? 1.0f : district->GetDefenseEvolutionPoints ();
    attackersAttackForce /= Urho3D::Sqrt (Urho3D::Sqrt (districtDefense));
    ApplyDamage (battle, configuration, attackersAttackForce, defenders, false, timeStep * 100.0f);

    if (attackersCountBefore != battle->GetAttackersUnitsCount () ||
            defendersCountBefore != battle->GetDefendersUnitsCount ())
    {
        NetworkUpdateCounterUtils::AddNetworkUpdatePointsToComponentCounter (battle, 100.0f);
    }
    return (battle->GetAttackersUnitsCount () > 0 && battle->GetDefendersUnitsCount () > 0);
}

Urho3D::PODVector <Unit *> BattlesProcessor::ReconstructUnits (UnitsManager *unitsManager, Urho3D::PODVector <Urho3D::StringHash> unitsHashes)
{
    Urho3D::PODVector <Unit *> units;
    for (int index = 0; index < unitsHashes.Size (); index++)
    {
        Unit *unit = unitsManager->GetUnitByHash (unitsHashes.At (index));
        if (unit)
        {
            units.Push (unit);
        }
    }
    return units;
}

float BattlesProcessor::CalculateUnitsAttackForce (Urho3D::PODVector <Unit *> &units, GameConfiguration *configuration, bool isNavalBattle)
{
    float attackForce = 0.0f;
    for (int index = 0; index < units.Size (); index++)
    {
        attackForce += units.At (index)->GetBattleAttackForce (configuration, isNavalBattle);
    }
    return attackForce;
}

void BattlesProcessor::ApplyDamage (Battle *battle, GameConfiguration *configuration, float fullDamage,
                                    Urho3D::PODVector <Unit *> &units, bool isAttackers, float damagedUnitUpdatePoints)
{
    int currentUnitIndex = Urho3D::Random (0, units.Size ());
    float medianDamagePerUnit = (fullDamage / (BattleHelpers::GetUnitsCountInBattle (battle, isAttackers) * 1.0f));

    while (fullDamage > 0.0f && BattleHelpers::GetUnitsCountInBattle (battle, isAttackers) > 0)
    {
        float damage = medianDamagePerUnit * Urho3D::Random (0.2f, 5.0f);
        if (damage > fullDamage)
        {
            damage = fullDamage;
        }
        fullDamage -= damage;

        Unit *unit = units.At (currentUnitIndex);
        if (!unit->ApplyDamage (configuration, damage))
        {
            BattleHelpers::RemoveUnitFromBattle (battle, isAttackers, unit->GetHash ());
            units.RemoveSwap (unit);
        }
        else
        {
            NetworkUpdateCounterUtils::AddNetworkUpdatePointsToComponentCounter (unit, damagedUnitUpdatePoints);
        }

        currentUnitIndex = Urho3D::Random (0, units.Size ());
    }
}

Battle *BattlesProcessor::CreateBattle (Urho3D::StringHash warHash, Urho3D::StringHash districtHash)
{
    Urho3D::Node *battleNode = node_->CreateChild ("battle", Urho3D::REPLICATED);
    Urho3D::SharedPtr <Battle> battle (battleNode->CreateComponent <Battle> (Urho3D::REPLICATED));
    battleNode->SetName ("battle" + Urho3D::String (battleNode->GetID ()));
    battleNode->AddTag (TAG_BATTLE);
    battles_.Push (battle);

    battle->SetWarHash (warHash);
    battle->SetDistrictHash (districtHash);
    battle->UpdateHash (this);
    return battle;
}

void BattlesProcessor::DeleteBattle (Battle *battle)
{
    UnitsManager *unitsManager = node_->GetScene ()->GetChild ("units")->GetComponent <UnitsManager> ();
    Urho3D::PODVector <Unit *> units;
    units.Push (ReconstructUnits (unitsManager, BattleHelpers::GetUnitsInBattleList (battle, true)));
    units.Push (ReconstructUnits (unitsManager, BattleHelpers::GetUnitsInBattleList (battle, false)));

    for (int index = 0; index < units.Size (); index++)
    {
        Unit *unit = units.At (index);
        if (unit)
        {
            unit->SetIsInBattle (false);
            NetworkUpdateCounterUtils::AddNetworkUpdatePointsToComponentCounter (unit, 100.0f);
        }
    }
    battle->GetNode ()->Remove ();
}
}
