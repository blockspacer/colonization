#include <Colonization/BuildConfiguration.hpp>
#include "BindMainMenuActivity.hpp"
#include <Urho3D/ThirdParty/AngelScript/angelscript.h>
#include <Urho3D/AngelScript/APITemplates.h>
#include <Colonization/Utils/Activity.hpp>
#include <Colonization/Activities/MainMenuActivity.hpp>
#include <Colonization/AngelScriptBinders/Activities/BindActivity.hpp>

namespace Colonization
{
void BindMainMenuActivity (Urho3D::Script *script)
{
    asIScriptEngine *engine = script->GetScriptEngine ();
    Urho3D::RegisterObject <MainMenuActivity> (engine, "MainMenuActivity");
    Urho3D::RegisterObjectConstructor <MainMenuActivity> (engine, "MainMenuActivity");
    Urho3D::RegisterSubclass <Activity, MainMenuActivity> (engine, "Activity", "MainMenuActivity");
    BindActivityInterface (script, "MainMenuActivity");
}
}