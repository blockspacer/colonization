#include <Colonization/BuildConfiguration.hpp>
#include "District.hpp"
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/Context.h>
#include <Colonization/Utils/Categories.hpp>
#include <Colonization/Core/Map.hpp>

namespace Colonization
{
const char *climateTypesNames [] =
{
    "Tropical",
    "Hot",
    "Temperate",
    "Temperate Continental",
    "Desert",
    "Cold",
};

const char *nativesCharacterTypesNames [] =
{
    "Friendly",
    "Medium",
    "Cold",
    "Isolationist",
    "Aggressive"
};

District::District (Urho3D::Context *context) : Urho3D::Component (context),
    hash_ ("nothing"),
    isSea_ (true),
    isImpassable_ (false),
    name_ ("Unknown"),
    polygonPoints_ (),
    unitPosition_ (),
    colonyPosition_ (),
    neighbors_ (),

    farmingSquare_ (1.0f),
    forestsSquare_ (1.0f),
    landAverageFertility_ (1.0f),
    climate_ (CLIMATE_TEMPERATE),

    forestsReproductivity_ (1.0f),
    hasCoalDeposits_ (false),
    hasIronDeposits_ (false),
    hasSilverDeposits_ (false),
    hasGoldDeposits_ (false),

    nativesCount_ (0.0f),
    nativesFightingTechnologyLevel_ (1.0f),
    nativesAggressiveness_ (0.0f),
    nativesCharacter_ (NATIVES_CHARACTER_MEDIUM),

    hasColony_ (false),
    colonyOwnerName_ (),
    menCount_ (0.0f),
    womenCount_ (0.0f),
    localArmySize_ (0.0f),
    farmsEvolutionPoints_ (0.0f),
    minesEvolutionPoints_ (0.0f),
    industryEvolutionPoints_ (0.0f),
    logisticsEvolutionPoints_ (0.0f),
    defenseEvolutionPoints_ (0.0f),
    averageLevelOfLifePoints_ (0.0f)
{

}

District::~District ()
{

}

void District::RegisterObject (Urho3D::Context *context)
{
    context->RegisterFactory <District> (COLONIZATION_CORE_CATEGORY);
    using namespace Urho3D;

    URHO3D_MIXED_ACCESSOR_ATTRIBUTE ("Hash", GetHash, SetHash, StringHash, StringHash::ZERO, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Is Sea", IsSea, SetIsSea, bool, true, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Is Impassable", IsImpassable, SetIsImpassable, bool, false, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE ("Name", GetName, SetName, String, String::EMPTY, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE ("Polygon Points", GetPolygonPointsAttribute, SetPolygonPointsAttribute, VariantVector, Variant::emptyVariantVector, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE ("Unit Position", GetUnitPosition, SetUnitPosition, Vector3, Vector3::ZERO, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE ("Colony Position", GetColonyPosition, SetColonyPosition, Vector3, Vector3::ZERO, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE ("Neighbors Hashes", GetNeighborsHashesAttribute, SetNeighborsHashesAttribute, VariantVector, Variant::emptyVariantVector, AM_DEFAULT);

    URHO3D_ACCESSOR_ATTRIBUTE ("Farming Square", GetFarmingSquare, SetFarmingSquare, float, 1.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Forests Square", GetForestsSquare, SetForestsSquare, float, 1.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Land Average Fertility", GetLandAverageFertility, SetLandAverageFertility, float, 1.0f, AM_DEFAULT);
    URHO3D_ENUM_ACCESSOR_ATTRIBUTE ("Climate", GetClimate, SetClimate, ClimateType, climateTypesNames, CLIMATE_TEMPERATE, AM_DEFAULT);

    URHO3D_ACCESSOR_ATTRIBUTE ("Forests Reproductivity", GetForestsReproductivity, SetForestsReproductivity, float, 1.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Has Coal Deposits", HasCoalDeposits, SetCoalDeposits, bool, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Has Iron Deposits", HasIronDeposits, SetIronDeposits, bool, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Has Silver Deposits", HasSilverDeposits, SetSilverDeposits, bool, false, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Has Gold Deposits", HasGoldDeposits, SetGoldDeposits, bool, false, AM_DEFAULT);

    URHO3D_ACCESSOR_ATTRIBUTE ("Natives Count", GetNativesCount, SetNativesCount, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Natives Fighting Technology Level", GetNativesFightingTechnologyLevel, SetNativesFightingTechnologyLevel, float, 1.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Natives Aggressiveness", GetNativesAggressiveness, SetNativesAggressiveness, float, 1.0f, AM_DEFAULT);
    URHO3D_ENUM_ACCESSOR_ATTRIBUTE ("Natives Character", GetNativesCharacter, SetNativesCharacter, NativesCharacterType, nativesCharacterTypesNames, NATIVES_CHARACTER_MEDIUM, AM_DEFAULT);

    URHO3D_ACCESSOR_ATTRIBUTE ("Has Colony", HasColony, SetColony, bool, false, AM_DEFAULT);
    URHO3D_MIXED_ACCESSOR_ATTRIBUTE ("Colony Owner Name", GetColonyOwnerName, SetColonyOwnerName, String, String::EMPTY, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Men Count", GetMenCount, SetMenCount, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Women Count", GetWomenCount, SetWomenCount, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Local Army Size", GetLocalArmySize, SetLocalArmySize, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Farms Evolution Points", GetFarmsEvolutionPoints, SetFarmsEvolutionPoints, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Mines Evolution Points", GetMinesEvolutionPoints, SetMinesEvolutionPoints, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Industry Evolution Points", GetIndustryEvolutionPoints, SetIndustryEvolutionPoints, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Logistics Evolution Points", GetLogisticsEvolutionPoints, SetLogisticsEvolutionPoints, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Defense Evolution Points", GetDefenseEvolutionPoints, SetDefenseEvolutionPoints, float, 0.0f, AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE ("Average Level Of Life Points", GetAverageLevelOfLifePoints, SetAverageLevelOfLifePoints, float, 0.0f, AM_DEFAULT);
}

void District::CalculateNeighbors (Urho3D::Vector<Urho3D::SharedPtr<District> > &allDistricts)
{
    assert (!allDistricts.Empty ());
    neighbors_.Clear ();
    for (int index = 0; index < allDistricts.Size (); index++)
    {
        Urho3D::SharedPtr <District> another = allDistricts.At (index);
        Urho3D::PODVector <Urho3D::Vector3> anotherPolygonPoints = another->GetPolygonPoints ();
        assert (another);

        if (another != this && !anotherPolygonPoints.Empty () && !polygonPoints_.Empty ())
        {
            int contactsCount = 0;
            for (int anotherPolygonPointIndex = 0;
                 anotherPolygonPointIndex < anotherPolygonPoints.Size (); anotherPolygonPointIndex++)

                for (int thisPolygonPointIndex = 0;
                     thisPolygonPointIndex < polygonPoints_.Size (); thisPolygonPointIndex++)

                    if (anotherPolygonPoints.At (anotherPolygonPointIndex) ==
                            polygonPoints_.At (thisPolygonPointIndex))
                        contactsCount += 1;

            if (contactsCount >= 2)
                neighbors_.Push (another->GetHash ());
        }
    }
}

void District::UpdateHash (Map *owner)
{
    do
        hash_ = Urho3D::StringHash (name_ + Urho3D::String (Urho3D::Random (0, 1000)));
    while (owner->GetDistrictByHash (hash_) != this);
}

Urho3D::StringHash District::GetHash () const
{
    return hash_;
}

void District::SetHash (const Urho3D::StringHash &hash)
{
    hash_ = hash;
}

bool District::IsSea () const
{
    return isSea_;
}

void District::SetIsSea (bool isSea)
{
    isSea_ = isSea;
}

bool District::IsImpassable () const
{
    return isImpassable_;
}

void District::SetIsImpassable (bool isImpassable)
{
    isImpassable_ = isImpassable;
}

Urho3D::String District::GetName () const
{
    return name_;
}

void District::SetName (const Urho3D::String &name)
{
    assert (!name.Empty ());
    name_ = name;
}

Urho3D::PODVector <Urho3D::Vector3> District::GetPolygonPoints () const
{
    return polygonPoints_;
}

void District::SetPolygonPoints (Urho3D::PODVector <Urho3D::Vector3> polygonPoints)
{
    polygonPoints_ = polygonPoints;
}

Urho3D::VariantVector District::GetPolygonPointsAttribute () const
{
    Urho3D::VariantVector variantVector;
    for (int index = 0; index < polygonPoints_.Size (); index++)
        variantVector.Push (Urho3D::Variant (polygonPoints_.At (index)));
    return variantVector;
}

void District::SetPolygonPointsAttribute (const Urho3D::VariantVector &polygonPoints)
{
    polygonPoints_.Clear ();
    for (int index = 0; index < polygonPoints.Size (); index++)
        polygonPoints_.Push (polygonPoints.At (index).GetVector3 ());
}

Urho3D::Vector3 District::GetUnitPosition () const
{
    return unitPosition_;
}

void District::SetUnitPosition (const Urho3D::Vector3 &unitPosition)
{
    unitPosition_ = unitPosition;
}

Urho3D::Vector3 District::GetColonyPosition () const
{
    return colonyPosition_;
}

void District::SetColonyPosition (const Urho3D::Vector3 &colonyPosition)
{
    colonyPosition_ = colonyPosition;
}

Urho3D::PODVector <Urho3D::StringHash> District::GetNeighborsHashes () const
{
    return neighbors_;
}

void District::SetNeighborsHashes (Urho3D::PODVector <Urho3D::StringHash> neighbors)
{
    neighbors_ = neighbors;
}

Urho3D::VariantVector District::GetNeighborsHashesAttribute() const
{
    Urho3D::VariantVector variantVector;
    for (int index = 0; index < neighbors_.Size (); index++)
        variantVector.Push (Urho3D::Variant (neighbors_.At (index)));
    return variantVector;
}

void District::SetNeighborsHashesAttribute (const Urho3D::VariantVector &neighbors)
{
    neighbors_.Clear ();
    for (int index = 0; index < neighbors.Size (); index++)
        neighbors_.Push (neighbors.At (index).GetStringHash ());
}

float District::GetFarmingSquare () const
{
    return farmingSquare_;
}

void District::SetFarmingSquare (float farmingSquare)
{
    assert (farmingSquare > 0.0f);
    farmingSquare_ = farmingSquare;
}

float District::GetForestsSquare () const
{
    return forestsSquare_;
}

void District::SetForestsSquare (float forestsSquare)
{
    assert (forestsSquare > 0.0f);
    forestsSquare_ = forestsSquare;
}

float District::GetLandAverageFertility () const
{
    return landAverageFertility_;
}

void District::SetLandAverageFertility (float landAverageFertility)
{
    assert (landAverageFertility > 0.0f);
    landAverageFertility_ = landAverageFertility;
}

ClimateType District::GetClimate () const
{
    return climate_;
}

void District::SetClimate (ClimateType climate)
{
    climate_ = climate;
}

float District::GetForestsReproductivity () const
{
    return forestsReproductivity_;
}

void District::SetForestsReproductivity (float forestsReproductivity)
{
    assert (forestsReproductivity > 0.0f);
    forestsReproductivity_ = forestsReproductivity;
}

bool District::HasCoalDeposits () const
{
    return hasCoalDeposits_;
}

void District::SetCoalDeposits (bool hasCoalDeposits)
{
    hasCoalDeposits_ = hasCoalDeposits;
}

bool District::HasIronDeposits () const
{
    return hasIronDeposits_;
}

void District::SetIronDeposits (bool hasIronDeposits)
{
    hasIronDeposits_ = hasIronDeposits;
}

bool District::HasSilverDeposits () const
{
    return hasSilverDeposits_;
}

void District::SetSilverDeposits (bool hasSilverDeposits)
{
    hasSilverDeposits_ = hasSilverDeposits;
}

bool District::HasGoldDeposits () const
{
    return hasGoldDeposits_;
}

void District::SetGoldDeposits (bool hasGoldDeposits)
{
    hasGoldDeposits_ = hasGoldDeposits;
}

float District::GetNativesCount () const
{
    return nativesCount_;
}

void District::SetNativesCount (float nativesCount)
{
    assert (nativesCount >= 0.0f);
    nativesCount_ = nativesCount;
}

float District::GetNativesFightingTechnologyLevel () const
{
    return nativesFightingTechnologyLevel_;
}

void District::SetNativesFightingTechnologyLevel (float nativesFightingTechnologyLevel)
{
    assert (nativesFightingTechnologyLevel > 0.0f);
    nativesFightingTechnologyLevel_ = nativesFightingTechnologyLevel;
}

float District::GetNativesAggressiveness () const
{
    return nativesAggressiveness_;
}

void District::SetNativesAggressiveness (float nativesAggressiveness)
{
    assert (nativesAggressiveness >= 0.0f);
    nativesAggressiveness_ = nativesAggressiveness;
}

NativesCharacterType District::GetNativesCharacter () const
{
    return nativesCharacter_;
}

void District::SetNativesCharacter (NativesCharacterType nativesCharacter)
{
    nativesCharacter_ = nativesCharacter;
}

bool District::HasColony () const
{
    return hasColony_;
}

void District::SetColony (bool hasColony)
{
    hasColony_ = hasColony;
}

Urho3D::String District::GetColonyOwnerName () const
{
    return colonyOwnerName_;
}

void District::SetColonyOwnerName (const Urho3D::String &colonyOwnerName)
{
    colonyOwnerName_ = colonyOwnerName;
}

float District::GetMenCount () const
{
    return menCount_;
}

void District::SetMenCount (float menCount)
{
    assert (menCount >= 0.0f);
    menCount_ = menCount;
}

float District::GetWomenCount () const
{
    return womenCount_;
}

void District::SetWomenCount (float womenCount)
{
    assert (womenCount >= 0.0f);
    womenCount_ = womenCount;
}

float District::GetLocalArmySize () const
{
    return localArmySize_;
}

void District::SetLocalArmySize (float localArmySize)
{
    assert (localArmySize >= 0.0f);
    localArmySize_ = localArmySize;
}

float District::GetFarmsEvolutionPoints () const
{
    return farmsEvolutionPoints_;
}

void District::SetFarmsEvolutionPoints (float farmsEvolutionPoints)
{
    assert (farmsEvolutionPoints >= 0.0f);
    farmsEvolutionPoints_ = farmsEvolutionPoints;
}

float District::GetMinesEvolutionPoints () const
{
    return minesEvolutionPoints_;
}

void District::SetMinesEvolutionPoints (float minesEvolutionPoints)
{
    assert (minesEvolutionPoints >= 0.0f);
    minesEvolutionPoints_ = minesEvolutionPoints;
}

float District::GetIndustryEvolutionPoints () const
{
    return industryEvolutionPoints_;
}

void District::SetIndustryEvolutionPoints (float industryEvolutionPoints)
{
    assert (industryEvolutionPoints >= 0.0f);
    industryEvolutionPoints_ = industryEvolutionPoints;
}

float District::GetLogisticsEvolutionPoints () const
{
    return logisticsEvolutionPoints_;
}

void District::SetLogisticsEvolutionPoints (float logisticsEvolutionPoints)
{
    assert (logisticsEvolutionPoints >= 0.0f);
    logisticsEvolutionPoints_ = logisticsEvolutionPoints;
}

float District::GetDefenseEvolutionPoints () const
{
    return defenseEvolutionPoints_;
}

void District::SetDefenseEvolutionPoints (float defenseEvolutionPoints)
{
    assert (defenseEvolutionPoints >= 0.0f);
    defenseEvolutionPoints_ = defenseEvolutionPoints;
}

float District::GetAverageLevelOfLifePoints () const
{
    return averageLevelOfLifePoints_;
}

void District::SetAverageLevelOfLifePoints (float averageLevelOfLifePoints)
{
    assert (averageLevelOfLifePoints >= 0.0f);
    averageLevelOfLifePoints_ = averageLevelOfLifePoints;
}
}
