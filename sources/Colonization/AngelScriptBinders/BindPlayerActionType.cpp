#include <Colonization/BuildConfiguration.hpp>
#include "BindPlayerActionType.hpp"
#include <Urho3D/ThirdParty/AngelScript/angelscript.h>
#include <Urho3D/AngelScript/APITemplates.h>
#include <Colonization/Backend/Player.hpp>
#include <Colonization/AngelScriptBinders/BindingMacroses.hpp>

namespace Colonization
{
void BindPlayerActionType(Urho3D::Script *script)
{
    asIScriptEngine *engine = script->GetScriptEngine ();
    engine->RegisterEnum ("PlayerActionType");
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_NONE", PLAYER_ACTION_NONE);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_SET_UNIT_MOVE_TARGET", PLAYER_ACTION_SET_UNIT_MOVE_TARGET);

    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_INVEST_TO_COLONY", PLAYER_ACTION_INVEST_TO_COLONY);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_CHANGE_INTERNAL_TRADE_TAXES", PLAYER_ACTION_CHANGE_INTERNAL_TRADE_TAXES);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_CHANGE_EUROPE_TRADE_TAXES", PLAYER_ACTION_CHANGE_EUROPE_TRADE_TAXES);

    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_REQUEST_COLONIZATORS_FROM_EUROPE", PLAYER_ACTION_REQUEST_COLONIZATORS_FROM_EUROPE);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_REQUEST_ARMY_FROM_EUROPE", PLAYER_ACTION_REQUEST_ARMY_FROM_EUROPE);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_REQUEST_FLEET_FROM_EUROPE", PLAYER_ACTION_REQUEST_FLEET_FROM_EUROPE);

    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_MOBILIZE_ARMY", PLAYER_ACTION_MOBILIZE_ARMY);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_IMMOBILIZE_ARMY", PLAYER_ACTION_IMMOBILIZE_ARMY);

    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_START_WAR", PLAYER_ACTION_START_WAR);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_SEND_PEACE_TREATY", PLAYER_ACTION_SEND_PEACE_TREATY);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_ACCEPT_PEACE_TREATY", PLAYER_ACTION_ACCEPT_PEACE_TREATY);
    engine->RegisterEnumValue ("PlayerActionType", "PLAYER_ACTION_SEND_GIFT_TO_THE_KING", PLAYER_ACTION_SEND_GIFT_TO_THE_KING);
}
}
