#pragma once
#include <Urho3D/Scene/Component.h>
#include <Colonization/Backend/Player.hpp>
#include <Colonization/Activities/HostActivity.hpp>

namespace Colonization
{
enum NetworkMessageType
{
    // CTS -- client to server message.
    CTS_NETWORK_MESSAGE_SEND_CHAT_MESSAGE = 100, // Data: message (String).
    CTS_NETWORK_MESSAGE_SEND_PRIVATE_MESSAGE = 101, // Data: message (String), ...recieviers... (String's).
    CTS_NETWORK_MESSAGE_SEND_PLAYER_ACTION = 102, // Data: actionType (int), actionDataBuffer (VectorBuffer ... [data]).

    // STC -- server to client message.
    STC_NETWORK_MESSAGE_SEND_PLAYER_STATS = 200, // Data: gold (float), points (float).
    STC_NETWORK_MESSAGE_CHAT_MESSAGE = 201, // Data: isPrivate (bool), senderName (String), message (String).
    STC_NETWORK_MESSAGE_TEXT_INFO_FROM_SERVER = 202, // Data: message (String).
    STC_NETWORK_MESSAGE_SEND_GAME_STATE = 203, // Data: int (GameState [enum]).
    STC_NETWORK_MESSAGE_GAME_ENDED = 204 // Data: winnerName (String), victoryType (String), victoryInfo (String).

    // TODO: To be continued.
};

class MessagesHandler : public Urho3D::Component
{
URHO3D_OBJECT (MessagesHandler, Component)
public:
    MessagesHandler (Urho3D::Context *context);
    virtual ~MessagesHandler ();
    static void RegisterObject (Urho3D::Context *context);

    void HandleClientIdentity (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void HandleNetworkMessage (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);

    void SendPlayersStats (Player *player);
    void SendChatMessage (Urho3D::String senderName, Urho3D::String message, Urho3D::Vector <Player *> &recieviers, bool isPrivate);
    void SendTextInfoFromServer (Urho3D::String info, Urho3D::Vector <Player *> &recieviers);
    void SendGameState (GameStateType gameState, Urho3D::Vector <Player *> &recieviers);
    void SendGameEnded (Urho3D::String winnerName, Urho3D::String victoryType, Urho3D::String victoryInfo, Urho3D::Vector <Player *> &recieviers);
};
}
