#pragma once
#include <Urho3D/Scene/Component.h>
#include <Colonization/Core/District.hpp>

namespace Colonization
{
class FogOfWarCalculator : public Urho3D::Component
{
URHO3D_OBJECT (FogOfWarCalculator, Component)
protected:
    Urho3D::String playerName_;
    // If true -- district visible.
    Urho3D::HashMap <Urho3D::StringHash, bool> fogOfWarMap_;
    void OpenDistrictAndNeighbors (District *district);
    virtual void OnSceneSet (Urho3D::Scene* scene);

public:
    FogOfWarCalculator (Urho3D::Context *context);
    virtual ~FogOfWarCalculator ();

    static void RegisterObject (Urho3D::Context *context);
    void Update (Urho3D::StringHash eventType, Urho3D::VariantMap &eventData);

    Urho3D::String GetPlayerName () const;
    void SetPlayerName (const Urho3D::String &playerName);

    bool IsDistrictVisible (Urho3D::StringHash districtHash);
    Urho3D::HashMap <Urho3D::StringHash, bool> GetFogOfWarMap ();
};
}