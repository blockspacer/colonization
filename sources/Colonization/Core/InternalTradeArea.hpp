#pragma once
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Scene/Component.h>
#include <Colonization/Core/GameConfiguration.hpp>

namespace Colonization
{
class Map;
class District;

const Urho3D::StringHash DISTRICT_PRODUCTION_AMOUNT_KEY ("DistrictProductionAmount");
const Urho3D::StringHash DISTRICT_PRODUCTION_RELATIVE_PRICE_KEY ("DistrictProductionRelativePrice");
const Urho3D::StringHash DISTRICT_PRODUCTION_QUALITY_KEY ("DistrictProductionQuality");
const Urho3D::StringHash DISTRICT_PRODUCTION_SELLABILITY_KEY ("DistrictProductionSellability");
const Urho3D::StringHash DISTRICT_PRODUCTION_SELLED_KEY ("DistrictProductionSelled");

struct DistrictProductionInfo
{
    Urho3D::StringHash districtHash_;
    float amount_;
    float relativePrice_;
    float quality_;
    float sellability_;
    float selled_;

    void CalculateSellability ();
    Urho3D::VariantMap ToVariantMap ();
};

typedef bool (*DistrictProductionInfoComparator) (DistrictProductionInfo &first, DistrictProductionInfo &second);
namespace DistrictProductionInfoComparators
{
bool HigherSellability (DistrictProductionInfo &first, DistrictProductionInfo &second);
}

class TradeDistrictProcessingInfo : public Urho3D::Object
{
URHO3D_OBJECT (TradeDistrictProcessingInfo, Object)
protected:
    Urho3D::HashMap <Urho3D::StringHash, float> unusedProduction_;
    float unsoldTradeGoodsCost_;
    float soldTradeGoodsCost_;
    float logisticsBonus_;
    float defenseBonus_;
public:
    TradeDistrictProcessingInfo (Urho3D::Context *context);
    virtual ~TradeDistrictProcessingInfo ();

    float GetUnusedProductionOf (Urho3D::StringHash type);
    void SetUnusedProductionOf (Urho3D::StringHash type, float points);

    float GetUnsoldTradeGoodsCost ();
    void SetUnsoldTradeGoodsCost (float unsoldTradeGoodsCost);

    float GetSoldTradeGoodsCost ();
    void SetSoldTradeGoodsCost (float soldTradeGoodsCost);

    float GetLogisticsBonus ();
    void SetLogisticsBonus (float logisticsBonus);

    float GetDefenseBonus ();
    void SetDefenseBonus (float defenseBonus);
};

// TODO: Maybe reimplement later to add some features?
// Maybe don't calculate whole production?
// Firstly calculate production of all districts separately.
// Then sort them by sellability: for example (goodsQuality / relativePrice) ^ 2.
// Them firstly sell better production.

class InternalTradeArea : public Urho3D::Component
{
URHO3D_OBJECT (InternalTradeArea, Component)
protected:
    Urho3D::PODVector <Urho3D::StringHash> districtsHashes_;

    void ConstructVectorOfRealDistricts (Map *map, Urho3D::PODVector <District *> &output);
    float CalculateTotalEvolutionOf (Urho3D::StringHash evolutionBranch, Urho3D::PODVector <District *> &realDistricts);
    int CalculateTotalSoldiersCount ();

    DistrictProductionInfo CalculateDistrictProductionOfFarms (District *district, GameConfiguration *configuration);
    DistrictProductionInfo CalculateDistrictProductionOfMines (District *district, GameConfiguration *configuration);
    DistrictProductionInfo CalculateDistrictProductionOfIndustry (District *district, GameConfiguration *configuration);

    void CalculateTotalProductionOfFarms (Urho3D::PODVector <District *> &realDistricts, GameConfiguration *configuration, Urho3D::Vector <DistrictProductionInfo> &output);
    void CalculateTotalProductionOfMines (Urho3D::PODVector <District *> &realDistricts, GameConfiguration *configuration, Urho3D::Vector <DistrictProductionInfo> &output);
    void CalculateTotalProductionOfIndustry (Urho3D::PODVector <District *> &realDistricts, GameConfiguration *configuration, Urho3D::Vector <DistrictProductionInfo> &output);

    float CalculateTotalProductionConsumptionOfFarms (GameConfiguration *configuration, Urho3D::PODVector <District *> &realDistricts, int soldiersCount);
    float CalculateTotalProductionConsumptionOfMines (GameConfiguration *configuration, Urho3D::PODVector <District *> &realDistricts, int soldiersCount);
    float CalculateTotalProductionConsumptionOfIndustry (GameConfiguration *configuration, Urho3D::PODVector <District *> &realDistricts, int soldiersCount);

    float CalculateDistrictProductionConsumptionOfFarms (GameConfiguration *configuration, District *district);
    float CalculateDistrictProductionConsumptionOfMines (GameConfiguration *configuration, District *district);
    float CalculateDistrictProductionConsumptionOfIndustry (GameConfiguration *configuration, District *district);

    /// This function return unsatisfied consumption or 0.0f.
    float ConsumeProduction (float consumption, Urho3D::Vector <DistrictProductionInfo> &production);
    float CalculateTotalProduction (Urho3D::Vector <DistrictProductionInfo> &production);

public:
    InternalTradeArea (Urho3D::Context *context);
    virtual ~InternalTradeArea ();

    virtual void DrawDebugGeometry (Urho3D::DebugRenderer *debug, bool depthTest);
    static void RegisterObject (Urho3D::Context *context);
    Urho3D::SharedPtr <TradeDistrictProcessingInfo> ProcessTrade (Map *map);

    int GetDistrictsHashesCount ();
    Urho3D::StringHash GetDistrictHashByIndex (int index);
    Urho3D::PODVector <Urho3D::StringHash> GetDistrictsHashesArray ();
    void AddDistrictHash (Urho3D::StringHash districtHash);
    bool ContainsDistrictHash (Urho3D::StringHash districtHash);
    bool RemoveDistrictHash (Urho3D::StringHash districtHash);

    Urho3D::VariantVector GetDistrictsHashesArrayAttribute () const;
    void SetDistrictsHashesArrayAttribute (const Urho3D::VariantVector &attribute);
};
}
