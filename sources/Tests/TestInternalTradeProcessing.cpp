#include "TestInternalTradeProcessing.hpp"
#include <Urho3D/Input/Input.h>
#include <Urho3D/Core/CoreEvents.h>

#include <Urho3D/IO/Log.h>
#include <Colonization/Core/District.hpp>
#include <Colonization/Core/Map.hpp>
#include <Colonization/Core/InternalTradeArea.hpp>
#include <Colonization/Backend/TradeProcessor.hpp>
#include <Colonization/Backend/PlayersManager.hpp>

URHO3D_DEFINE_APPLICATION_MAIN (Tests::TestInternalTradeProcessingApplication)
namespace Tests
{
TestInternalTradeProcessingApplication::TestInternalTradeProcessingApplication (Urho3D::Context *context) :
    Urho3D::Application (context)
{

}

TestInternalTradeProcessingApplication::~TestInternalTradeProcessingApplication ()
{

}

void TestInternalTradeProcessingApplication::Setup ()
{
    engineParameters_ ["FullScreen"] = false;
    engineParameters_ ["WindowResizable"] = true;
    engineParameters_ ["LogName"] = "TestInternalTradeProcessing.log";
    engineParameters_ ["WindowTitle"] = "TestInternalTradeProcessing";
}

void TestInternalTradeProcessingApplication::Start ()
{
    // Set mouse to free mode
    Urho3D::Input *input = GetSubsystem <Urho3D::Input> ();
    input->SetMouseVisible (true);
    input->SetMouseMode (Urho3D::MM_FREE);

    // Set logging level
    Urho3D::Log *log = context_->GetSubsystem <Urho3D::Log> ();
    log->SetLevel (Urho3D::LOG_DEBUG);

    context_->SetGlobalVar ("oneColonistFarmsProductionConsumption", 0.01f);
    context_->SetGlobalVar ("farmsProductionMinesConsumption", 0.005f);
    context_->SetGlobalVar ("farmsProductionIndustryConsumption", 0.3f);

    context_->SetGlobalVar ("oneColonistMinesProductionConsumption", 0.002f);
    context_->SetGlobalVar ("minesProductionFarmsConsumption", 0.2f);
    context_->SetGlobalVar ("minesProductionIndustryConsumption", 0.9f);

    context_->SetGlobalVar ("oneColonistIndustryProductionConsumption", 0.003f);
    context_->SetGlobalVar ("industryProductionFarmsConsumption", 0.15f);
    context_->SetGlobalVar ("industryProductionMinesConsumption", 0.25f);

    context_->SetGlobalVar ("farmsProductionInternalCost", 1.0f);
    context_->SetGlobalVar ("minesProductionInternalCost", 1.5f);
    context_->SetGlobalVar ("industryProductionInternalCost", 5.0f);

    context_->SetGlobalVar ("internalTaxes", 0.2f);
    context_->SetGlobalVar ("externalTaxes", 0.2f);

    Urho3D::SharedPtr <Colonization::Map> map (new Colonization::Map (context_));
    const int mapWidth = 4;
    const int mapHeight = 4;

    for (int x = 0; x < mapWidth; x++)
        for (int y = 0; y < mapHeight; y++)
        {
            Colonization::District *district = new Colonization::District (context_);
            district->name_ = "(" + Urho3D::String (x) + "; " + Urho3D::String (y) + ")";

            district->polygonPoints_.Push (Urho3D::Vector3 (x - 0.5f, 0.0f, y - 0.5f));
            district->polygonPoints_.Push (Urho3D::Vector3 (x + 0.5f, 0.0f, y - 0.5f));
            district->polygonPoints_.Push (Urho3D::Vector3 (x + 0.5f, 0.0f, y + 0.5f));
            district->polygonPoints_.Push (Urho3D::Vector3 (x - 0.5f, 0.0f, y + 0.5f));

            district->unitPosition_ = Urho3D::Vector3 (x, 0.0f, y);
            district->colonyPosition_ = Urho3D::Vector3 (x, 0.0f, y);

            district->isSea_ = true;
            district->hasColony_ = false;
            map->AddDistrict (district);
            district->UpdateHash (map);
        }
    map->UpdateNeighborsOfDistricts ();

    // Map: (~ -- sea, = -- terrain, numbers -- colonies with equal logistics level)
    //   0 1 2 3
    // 3 ~ ~ 5 ~
    // 2 7 ~ 5 ~
    // 1 ~ ~ 1 1
    // 0 ~ ~ 1 5
    // array (X * HEIGHT + Y) = (X, Y)

    Urho3D::PODVector <Urho3D::Vector3> coloniesCoordinatesWithLogistics;
    coloniesCoordinatesWithLogistics.Push (Urho3D::Vector3 (0, 2, 7));
    coloniesCoordinatesWithLogistics.Push (Urho3D::Vector3 (2, 3, 5));
    coloniesCoordinatesWithLogistics.Push (Urho3D::Vector3 (2, 2, 5));
    coloniesCoordinatesWithLogistics.Push (Urho3D::Vector3 (2, 1, 1));
    coloniesCoordinatesWithLogistics.Push (Urho3D::Vector3 (2, 0, 1));
    coloniesCoordinatesWithLogistics.Push (Urho3D::Vector3 (3, 1, 1));
    coloniesCoordinatesWithLogistics.Push (Urho3D::Vector3 (3, 0, 5));

    for (int index = 0; index < coloniesCoordinatesWithLogistics.Size (); index++)
    {
        Urho3D::Vector3 coordinate = coloniesCoordinatesWithLogistics.At (index);
        Colonization::District *district = map->GetDistrictByIndex (coordinate.x_ * mapHeight + coordinate.y_);
        district->hasColony_ = true;
        district->colonyOwnerName_ = "PlayerX";
        district->farmsEvolutionPoints_ = 4.0f;
        district->minesEvolutionPoints_ = 2.0f;
        district->industryEvolutionPoints_ = 1.5f;
        district->logisticsEvolutionPoints_ = coordinate.z_;
        district->defenseEvolutionPoints_ = 5.0f;
    }

    Urho3D::SharedPtr <Colonization::PlayersManager> playersManager (new Colonization::PlayersManager (context_));
    Colonization::Player *fakePlayer = new Colonization::Player (context_, "PlayerX", 0);
    playersManager->ManuallyAddFakePlayer (fakePlayer);

    Urho3D::SharedPtr <Colonization::TradeProcessor> tradeProcessor (new Colonization::TradeProcessor (context_));
    context_->SetGlobalVar ("Map", Urho3D::Variant (map.Get ()));
    context_->SetGlobalVar ("PlayersManager", Urho3D::Variant (playersManager.Get ()));
    context_->SetGlobalVar ("TradeProcessor", Urho3D::Variant (tradeProcessor.Get ()));

    Urho3D::VariantMap eventData;
    eventData [Urho3D::Update::P_TIMESTEP] = 0.1f;
    tradeProcessor->Update (Urho3D::E_UPDATE, eventData);

    Urho3D::Log::Write (Urho3D::LOG_INFO, "Trade areas count: " + Urho3D::String (tradeProcessor->GetTradeAreasCount ()));
    for (int index = 0; index < tradeProcessor->GetTradeAreasCount (); index++)
    {
        Colonization::InternalTradeArea *tradeArea = tradeProcessor->GetTradeAreaByIndex (index);
        Urho3D::Log::Write (Urho3D::LOG_INFO, "Trade area: " + Urho3D::String (index) + ", districts:");
        for (int districtIndex = 0; districtIndex < tradeArea->GetDistrictsHashesCount (); districtIndex++)
            Urho3D::Log::Write (Urho3D::LOG_INFO, map->GetDistrictByHash (tradeArea->GetDistrictHashByIndex (districtIndex))->name_);
        Urho3D::Log::Write (Urho3D::LOG_INFO, "");
    }


    Urho3D::Log::Write (Urho3D::LOG_INFO, "PlayerX's gold: " + Urho3D::String (fakePlayer->GetGold ()));
    if (tradeProcessor->GetTradeAreasCount () != 2)
    {
        playersManager->ManuallyRemoveFakePlayer (fakePlayer->GetName ());
        ErrorExit ("Expected 2 trade areas!");
    }
    else if (fakePlayer->GetGold () <= 5.0f)
    {
        playersManager->ManuallyRemoveFakePlayer (fakePlayer->GetName ());
        ErrorExit ("Expected that PlayerX has more than 5.0 gold!");
    }
    else
    {
        playersManager->ManuallyRemoveFakePlayer (fakePlayer->GetName ());
        engine_->Exit ();
    }
}

void TestInternalTradeProcessingApplication::Stop ()
{

}
}

