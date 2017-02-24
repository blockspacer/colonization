#include <Colonization/BuildConfiguration.hpp>
#include "NetworkUpdateCounter.hpp"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

#include <Colonization/Utils/Network/NetworkUpdateSmoother.hpp>
#include <Colonization/Utils/Serialization/Categories.hpp>
#include <Colonization/Utils/Serialization/AttributeMacro.hpp>

namespace Colonization
{
NetworkUpdateCounter::NetworkUpdateCounter (Urho3D::Context *context) : Urho3D::Component (context),
    object_ (0),
    accumulatedUpdatePoints_ (0.0f)
{

}

NetworkUpdateCounter::~NetworkUpdateCounter ()
{

}

void NetworkUpdateCounter::RegisterObject (Urho3D::Context *context)
{
    context->RegisterFactory <NetworkUpdateCounter> (COLONIZATION_SERVER_ONLY_CATEGORY);
    URHO3D_ACCESSOR_ATTRIBUTE ("Is Enabled", IsEnabled, SetEnabled, bool, true, Urho3D::AM_DEFAULT);
}

Urho3D::Serializable *NetworkUpdateCounter::GetObject ()
{
    return object_;
}

void NetworkUpdateCounter::SetObject (Urho3D::Serializable *object)
{
    assert (object);
    object_ = object;
}

float NetworkUpdateCounter::GetAccumulatedUpdatePoints ()
{
    return accumulatedUpdatePoints_;
}

void NetworkUpdateCounter::AddUpdatePoints (float points)
{
    accumulatedUpdatePoints_ += points;
    if (accumulatedUpdatePoints_ >= 100.0f)
    {
        if (object_)
        {
            NetworkUpdateSmoother *smoother = node_->GetScene ()->GetComponent <NetworkUpdateSmoother> ();
            smoother->RequestNetworkUpdate (object_);
        }
        accumulatedUpdatePoints_ = 0.0f;
    }
}

NetworkUpdateCounter *CreateNetworkUpdateCounterForComponent(Urho3D::Component *component)
{
    NetworkUpdateCounter *counter = component->GetNode ()->CreateComponent <NetworkUpdateCounter> (Urho3D::LOCAL);
    counter->SetObject (component);
    return counter;
}

NetworkUpdateCounter *CreateNetworkUpdateCounterForNode(Urho3D::Node *node)
{
    NetworkUpdateCounter *counter = node->CreateComponent <NetworkUpdateCounter> (Urho3D::LOCAL);
    counter->SetObject (node);
    return counter;
}
}