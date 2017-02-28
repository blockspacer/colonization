#pragma once
#include <Colonization/BuildConfiguration.hpp>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Network/Connection.h>

#include <Colonization/Backend/Player.hpp>
#include <Colonization/Backend/MessagesHandler.hpp>
#include <Colonization/Core/PlayerInfo.hpp>

namespace Colonization
{
class PlayersManager : public Urho3D::Component
{
URHO3D_OBJECT (PlayersManager, Component)
protected:
    Urho3D::HashMap <Urho3D::StringHash, Player *> players_;
    Urho3D::HashMap <Urho3D::StringHash, Urho3D::StringHash> connectionHashToNameHashMap_;
    Urho3D::Vector <Urho3D::Pair <float, Urho3D::Connection *> > connectionsWithoutId_;

    bool DeleteIdentificatedConnection (Urho3D::Connection *connection);
    void UpdatePlayers (MessagesHandler *messagesHandler, float timeStep);
    void UpdateConnectionsWithoudId (float timeStep);
    void UpdatePlayersInfos ();
    virtual void OnSceneSet (Urho3D::Scene* scene);

public:
    PlayersManager (Urho3D::Context *context);
    virtual ~PlayersManager ();
    static void RegisterObject (Urho3D::Context *context);

    void Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void HandleClientConnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);
    void HandleClientDisconnected (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);

    int GetPlayersCount () const;
    Player *GetPlayerByIndex (int index) const;
    Player *GetPlayerByConnection (Urho3D::Connection *connection) const;
    Player *GetPlayerByNameHash (Urho3D::StringHash nameHash) const;

    PlayerInfo *GetPlayerInfoByPointer (Player *player) const;
    PlayerInfo *GetPlayerInfoByNameHash (Urho3D::StringHash nameHash) const;

    void DisconnectAllUnidentificatedConnections ();
    Urho3D::Vector <Player *> GetPlayersByNames (Urho3D::Vector <Urho3D::StringHash> namesHashes) const;
    Urho3D::Vector <Player *> GetAllPlayers () const;

    void PlayerIdentified (Urho3D::Connection *connection, Urho3D::String name, Urho3D::Color color);
    void DisconnectPlayer (Urho3D::StringHash nameHash);
    void DisconnectPlayer (Urho3D::Connection *connection);
};
}
