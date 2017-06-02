#pragma once
#include <Colonization/Utils/Activities/Activity.hpp>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/AngelScript/ScriptInstance.h>

namespace Colonization
{
//@ASBindGen Class ObjectType=Ref
class MainMenuActivity : public Activity
{
URHO3D_OBJECT (MainMenuActivity, Activity)
protected:
    Urho3D::Scene *backgroundScene_;
    Urho3D::ScriptInstance *angelScriptGUI_;
public:
    //@ASBindGen Constructor UseUrho3DScriptContext_arg0
    MainMenuActivity (Urho3D::Context *context);
    virtual ~MainMenuActivity ();

    virtual void Start ();
    virtual void Update (float timeStep);
    virtual void Stop ();
};
}
