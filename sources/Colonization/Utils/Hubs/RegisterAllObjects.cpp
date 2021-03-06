#include <Colonization/BuildConfiguration.hpp>
#include "RegisterAllObjects.hpp"

#include <Colonization/Backend/ColoniesProcessor.hpp>
#include <Colonization/Backend/MessagesHandler/MessagesHandler.hpp>
#include <Colonization/Backend/Player/Player.hpp>
#include <Colonization/Backend/PlayersManager.hpp>
#include <Colonization/Backend/PlayersPointsCalculator.hpp>
#include <Colonization/Backend/TradeProcessor.hpp>
#include <Colonization/Backend/UnitsManager.hpp>
#include <Colonization/Backend/VictoryProgressUpdater/VictoryProgressUpdater.hpp>
#include <Colonization/Backend/BattlesProcessor.hpp>

#include <Colonization/Backend/Diplomacy/DiplomacyProcessor.hpp>
#include <Colonization/Backend/Diplomacy/DiplomacyRequest.hpp>
#include <Colonization/Backend/Diplomacy/DiplomacyWarRequest.hpp>
#include <Colonization/Backend/Diplomacy/DiplomacyPeaceRequest.hpp>

#include <Colonization/Core/Diplomacy/DiplomacyWar.hpp>
#include <Colonization/Core/District/District.hpp>
#include <Colonization/Core/GameConfiguration.hpp>
#include <Colonization/Core/InternalTradeArea/InternalTradeArea.hpp>
#include <Colonization/Core/Map.hpp>
#include <Colonization/Core/Battle/Battle.hpp>
#include <Colonization/Core/PlayerInfo/PlayerInfo.hpp>

#include <Colonization/Core/Unit/Unit.hpp>
#include <Colonization/Core/Unit/FleetUnit.hpp>
#include <Colonization/Core/Unit/TradersUnit.hpp>
#include <Colonization/Core/Unit/ColonizatorsUnit.hpp>
#include <Colonization/Core/Unit/ArmyUnit.hpp>

#include <Colonization/Frontend/FogOfWarCalculator.hpp>
#include <Colonization/Frontend/MapMaskUpdater.hpp>

#include <Colonization/Utils/Network/NetworkUpdateCounter.hpp>
#include <Colonization/Utils/Network/NetworkUpdateSmoother.hpp>

namespace Colonization
{
void RegisterAllObjects (Urho3D::Context *context)
{
    ColoniesProcessor::RegisterObject (context);
    MessagesHandler::RegisterObject (context);
    PlayersManager::RegisterObject (context);
    PlayersPointsCalculator::RegisterObject (context);
    TradeProcessor::RegisterObject (context);
    UnitsManager::RegisterObject (context);
    VictoryProgressUpdater::RegisterObject (context);
    BattlesProcessor::RegisterObject (context);

    DiplomacyProcessor::RegisterObject (context);
    DiplomacyRequest::RegisterObject (context);
    DiplomacyWarRequest::RegisterObject (context);
    DiplomacyPeaceRequest::RegisterObject (context);

    DiplomacyWar::RegisterObject (context);
    District::RegisterObject (context);
    GameConfiguration::RegisterObject (context);
    InternalTradeArea::RegisterObject (context);
    Map::RegisterObject (context);
    Battle::RegisterObject (context);
    PlayerInfo::RegisterObject (context);

    Unit::RegisterObject (context);
    FleetUnit::RegisterObject (context);
    TradersUnit::RegisterObject (context);
    ColonizatorsUnit::RegisterObject (context);
    ArmyUnit::RegisterObject (context);

    FogOfWarCalculator::RegisterObject (context);
    MapMaskUpdater::RegisterObject (context);

    NetworkUpdateCounter::RegisterObject (context);
    NetworkUpdateSmoother::RegisterObject (context);
}
}
