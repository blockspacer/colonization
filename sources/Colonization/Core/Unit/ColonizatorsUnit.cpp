#include <Colonization/BuildConfiguration.hpp>
#include "ColonizatorsUnit.hpp"
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Scene/Scene.h>

#include <Colonization/Core/Map.hpp>
#include <Colonization/Core/District/District.hpp>
#include <Colonization/Core/Unit/UnitTags.hpp>
#include <Colonization/Core/GameConfiguration.hpp>

#include <Colonization/Backend/UnitsManager.hpp>
#include <Colonization/Utils/Serialization/Categories.hpp>
#include <Colonization/Utils/Serialization/AttributeMacro.hpp>

namespace Colonization
{
ColonizatorsUnit::ColonizatorsUnit (Urho3D::Context *context) : Unit (context),
    colonizatorsCount_ (0.0f),
    isGoingToSettle_ (false)
{
    unitType_ = UNIT_COLONIZATORS;
}

ColonizatorsUnit::~ColonizatorsUnit ()
{

}

void ColonizatorsUnit::RegisterObject (Urho3D::Context *context)
{
    context->RegisterFactory <ColonizatorsUnit> (COLONIZATION_CORE_CATEGORY);
    URHO3D_COPY_BASE_ATTRIBUTES (Unit);
    URHO3D_ACCESSOR_ATTRIBUTE ("Colonizators Count", GetColonizatorsCount, SetColonizatorsCount, float, 0.0f, Urho3D::AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Is Going To Settle", GetIsGoingToSettle, SetIsGoingToSettle, bool, false, Urho3D::AM_DEFAULT);
}

void ColonizatorsUnit::DrawDebugGeometry (Urho3D::DebugRenderer *debug, bool depthTest)
{
    Map *map = node_->GetScene ()->GetChild ("map")->GetComponent <Map> ();
    assert (map);

    District *district = map->GetDistrictByHash (GetPositionHash ());
    if (district)
    {
        Urho3D::Sphere sphere (district->GetUnitPosition (), 0.15f);
        debug->AddSphere (sphere, Urho3D::Color::GREEN, depthTest);
    }
}

float ColonizatorsUnit::GetColonizatorsCount () const
{
    return colonizatorsCount_;
}

void ColonizatorsUnit::SetColonizatorsCount (float colonizatorsCount)
{
    colonizatorsCount_ = colonizatorsCount;
}

bool ColonizatorsUnit::GetIsGoingToSettle () const
{
    return isGoingToSettle_;
}

void ColonizatorsUnit::SetIsGoingToSettle (bool isGoingToSettle)
{
    isGoingToSettle_ = isGoingToSettle;
}

bool ColonizatorsUnit::IsCanGoTo (const District *district, const Map *map, Urho3D::StringHash imaginePosition) const
{
    if (Unit::IsCanGoTo (district, map, imaginePosition))
    {
        if (district->GetIsSea () || (district->GetHasColony () && district->GetColonyOwnerName () == GetOwnerPlayerName ()))
        {
            return true;
        }
        else if (!district->GetHasColony ())
        {
            Urho3D::PODVector <Urho3D::StringHash> targetNeighbors = district->GetNeighborsHashes ();
            for (int index = 0; index < targetNeighbors.Size (); index++)
            {
                District *targetNeighbor = map->GetDistrictByHash (targetNeighbors.At (index));
                if (targetNeighbor->GetIsSea () || (targetNeighbor->GetHasColony () &&
                                                    targetNeighbor->GetColonyOwnerName () == GetOwnerPlayerName ()))
                {
                    return true;
                }
            }
            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

float ColonizatorsUnit::GetBattleAttackForce (GameConfiguration *configuration, bool isNaval) const
{
    if (isNaval)
    {
        return configuration->GetColonizatorBasicNavalAttackForce () * colonizatorsCount_;
    }
    else
    {
        return configuration->GetColonizatorBasicLandAttackForce () * colonizatorsCount_;
    }
}

bool ColonizatorsUnit::ApplyDamage (GameConfiguration *configuration, float damage)
{
    float losses = damage / configuration->GetColonizatorBasicHealth ();
    colonizatorsCount_ -= losses;
    return IsAlive ();
}

bool ColonizatorsUnit::IsAlive () const
{
    return colonizatorsCount_ > 0.0f;
}

Urho3D::String ColonizatorsUnit::GetUnitTypeTag () const
{
    return TAG_COLONIZATORS;
}
}
