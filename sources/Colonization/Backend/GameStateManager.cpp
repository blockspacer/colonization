#include <Colonization/BuildConfiguration.hpp>
#include "GameStateManager.hpp"
#include <Urho3D/Core/Context.h>
#include <Colonization/Backend/MessagesHandler.hpp>
#include <Colonization/Backend/PlayersManager.hpp>

namespace Colonization
{
void GameStateManager::SetupWaitingForPlayersState ()
{
    // TODO: To be continued...
    MessagesHandler *messagesHandler = new MessagesHandler (context_);
    PlayersManager *playersManager = new PlayersManager (context_);

    context_->SetGlobalVar ("MessagesHandler", Urho3D::Variant (messagesHandler));
    context_->SetGlobalVar ("PlayersManager", Urho3D::Variant (playersManager));
}

void GameStateManager::DisposeWaitingForPlayersState ()
{
    // TODO: Temporary. Reimplement later.
    SetupState (GAME_STATE_FINISHED);
    DisposeCurrentState ();
}

void GameStateManager::SetupPlayingState ()
{
    // TODO: Implement later.
}

void GameStateManager::DisposePlayingState ()
{
    // TODO: Implement later.
}

void GameStateManager::SetupFinishedState ()
{
    // TODO: Implement later.
}

void GameStateManager::DisposeFinishedState ()
{
    // TODO: To be continued...
    delete ( (MessagesHandler *) context_->GetGlobalVar ("MessagesHandler").GetPtr ());
    delete ( (PlayersManager *) context_->GetGlobalVar ("PlayersManager").GetPtr ());
}

void GameStateManager::SetupState (GameStateType state)
{
    if (state == GAME_STATE_WAITING_FOR_PLAYERS)
        SetupWaitingForPlayersState ();
    else if (state == GAME_STATE_PLAYING)
        SetupPlayingState ();
    else if (state == GAME_STATE_FINISHED)
        SetupFinishedState ();
    currentState_ = state;
}

void GameStateManager::DisposeCurrentState ()
{
    if (currentState_ == GAME_STATE_WAITING_FOR_PLAYERS)
        DisposeWaitingForPlayersState ();
    else if (currentState_ == GAME_STATE_PLAYING)
        DisposePlayingState ();
    else if (currentState_ == GAME_STATE_FINISHED)
        DisposeFinishedState ();
}

bool GameStateManager::WillIGoFromWaitingForPlayersToPlayingState ()
{
    PlayersManager *playersManager = (PlayersManager *) context_->GetGlobalVar ("PlayersManager").GetPtr ();
    // TODO: Reimplement later!
    return (playersManager->GetAllPlayers ().Size () > 0);
}

bool GameStateManager::WillIGoFromPlayingToFinishedState ()
{
    // TODO: Implement later.
    return false;
}

GameStateManager::GameStateManager (Urho3D::Context *context) : Urho3D::Object (context),
    currentState_ (GAME_STATE_UNITIALIZED)
{
    SetupState (GAME_STATE_WAITING_FOR_PLAYERS);
}

GameStateManager::~GameStateManager ()
{
    DisposeCurrentState ();
}

GameStateType GameStateManager::GetCurrentState ()
{
    return currentState_;
}
}
