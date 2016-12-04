#include <Colonization/BuildConfiguration.hpp>
#include "MessagesHandler.hpp"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Connection.h>
#include <Colonization/Backend/PlayersManager.hpp>

namespace Colonization
{
MessagesHandler::MessagesHandler (Urho3D::Context *context) : Urho3D::Object (context)
{
    SubscribeToEvent (Urho3D::E_CLIENTIDENTITY, URHO3D_HANDLER (MessagesHandler, HandleClientIdentity));
    SubscribeToEvent (Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER (MessagesHandler, HandleNetworkMessage));
}

MessagesHandler::~MessagesHandler ()
{

}

void MessagesHandler::HandleClientIdentity (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    Urho3D::Connection *connection = (Urho3D::Connection *)
            eventData [Urho3D::ClientDisconnected::P_CONNECTION].GetPtr ();
    PlayersManager *playersManager = (PlayersManager *) context_->GetGlobalVar ("PlayersManager").GetPtr ();
    assert (playersManager);
    playersManager->PlayerIdentified (connection, connection->GetIdentity () ["Name"].GetString ());
}

void MessagesHandler::HandleNetworkMessage (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData)
{
    PlayersManager *playersManager = (PlayersManager *) context_->GetGlobalVar ("PlayersManager").GetPtr ();
    assert (playersManager);

    Urho3D::Connection *connection = (Urho3D::Connection *)
            eventData [Urho3D::NetworkMessage::P_CONNECTION].GetPtr ();
    Player *player = playersManager->GetPlayer (connection);
    assert (player);

    NetworkMessageType messageType = static_cast <NetworkMessageType> (
                eventData [Urho3D::NetworkMessage::P_MESSAGEID].GetInt ());
    Urho3D::VectorBuffer messageData = eventData [Urho3D::NetworkMessage::P_DATA].GetVectorBuffer ();

    if (messageType == CTS_NETWORK_MESSAGE_SEND_CHAT_MESSAGE)
    {
        Urho3D::Vector <Player *> players = playersManager->GetAllPlayers ();
        SendChatMessage (player->GetName (), messageData.ReadString (), players, false);
    }

    else if (messageType == CTS_NETWORK_MESSAGE_SEND_PRIVATE_MESSAGE)
    {
        Urho3D::String message = messageData.ReadString ();
        Urho3D::Vector <Urho3D::StringHash> recievers;
        while (!messageData.IsEof ())
            recievers.Push (Urho3D::StringHash (messageData.ReadString ()));
        Urho3D::Vector <Player *> players = playersManager->GetPlayersByNames (recievers);
        SendChatMessage (player->GetName (), message, players, true);
    }

    else if (messageType == CTS_NETWORK_MESSAGE_SEND_PLAYER_ACTION)
    {
        Urho3D::VariantMap actionData = messageData.ReadVariantMap ();
        PlayerActionType actionType = static_cast <PlayerActionType> (messageData.ReadInt ());
        Urho3D::Pair <PlayerActionType, Urho3D::VariantMap> action =
                Urho3D::Pair <PlayerActionType, Urho3D::VariantMap> (actionType, actionData);
        player->AddAction (action);
    }
}

void MessagesHandler::SendPlayersStats (Player *player)
{
    assert (player);
    Urho3D::VectorBuffer messageData;
    messageData.WriteFloat (player->GetGold ());
    messageData.WriteFloat (player->GetPoints ());
    player->GetConnection ()->SendMessage (STC_NETWORK_MESSAGE_SEND_PLAYER_STATS, true, false, messageData);
}

void MessagesHandler::SendChatMessage (Urho3D::String senderName, Urho3D::String message, Urho3D::Vector <Player *> &recieviers, bool isPrivate)
{
    Urho3D::VectorBuffer messageData;
    messageData.WriteBool (isPrivate);
    messageData.WriteString (senderName);
    messageData.WriteString (message);

    for (int index = 0; index < recieviers.Size (); index++)
        recieviers.At (index)->GetConnection ()->SendMessage (STC_NETWORK_MESSAGE_CHAT_MESSAGE, true, false, messageData);
}

void MessagesHandler::SendTextInfoFromServer (Urho3D::String info, Urho3D::Vector <Player *> &recieviers)
{
    Urho3D::VectorBuffer messageData;
    messageData.WriteString (info);
    for (int index = 0; index < recieviers.Size (); index++)
        recieviers.At (index)->GetConnection ()->SendMessage (STC_NETWORK_MESSAGE_TEXT_INFO_FROM_SERVER, true, false, messageData);
}

void MessagesHandler::SendGameState (GameStateType gameState, Urho3D::Vector<Player *> &recieviers)
{
    Urho3D::VectorBuffer messageData;
    messageData.WriteInt (static_cast <int> (gameState));
    for (int index = 0; index < recieviers.Size (); index++)
        recieviers.At (index)->GetConnection ()->SendMessage (STC_NETWORK_MESSAGE_SEND_GAME_STATE, true, false, messageData);
}
}