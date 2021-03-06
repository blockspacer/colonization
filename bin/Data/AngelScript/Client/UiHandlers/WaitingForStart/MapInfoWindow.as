#include "AngelScript/Utils/Constants.as"
#include "AngelScript/Utils/ClientUtils.as"

class MapInfoWindow : ScriptObject
{
    MapInfoWindow ()
    {

    }

    ~MapInfoWindow ()
    {

    }

    void Start ()
    {
        Window @mapInfoWindow = ui.root.GetChild ("waitingForStart").GetChild ("functionalWindows").GetChild ("mapInfoWindow");
        Button @hideButton = mapInfoWindow.GetChild ("hideButton");
        SubscribeToEvent (hideButton, "Released", "HandleHideClick");
    }

    void Update (float timeStep)
    {
        Window @mapInfoWindow = ui.root.GetChild ("waitingForStart").GetChild ("functionalWindows").GetChild ("mapInfoWindow");
        Text @info = mapInfoWindow.GetChild ("info");

        String selectedMapFolder = scene.vars [VAR_MAP_FOLDER].GetString ();
        XMLFile @infoXML = cache.GetResource ("XMLFile", scene.vars [VAR_MAP_INFO_PATH].GetString ());
        if (infoXML !is null)
        {
            XMLElement userInfo = infoXML.GetRoot ().GetChild ("userInformation");
            String infoText = "Short info: " + userInfo.GetAttribute ("shortInfo") + "\n\n";
            infoText += "Maximum players: " + userInfo.GetInt ("maxPlayers") + ".\n\n";
            infoText += "Supported victory types: " + userInfo.GetAttribute ("supportedVictoryTypes") + ".";
            info.text = infoText;
        }
    }

    void Stop ()
    {
        UnsubscribeFromAllEvents ();
    }

    void HandleHideClick ()
    {
        VariantMap eventData;
        eventData [ShowFunctionalWindowRequest::WINDOW_NAME] = Variant ("actionsWindow");
        SendEvent (EVENT_SHOW_FUNCTIONAL_WINDOW_REQUEST, eventData);
    }
}
