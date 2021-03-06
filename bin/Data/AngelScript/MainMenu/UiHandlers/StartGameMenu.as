#include "AngelScript/Utils/Constants.as"
#include "AngelScript/Utils/ClientUtils.as"

class StartGameMenu : ScriptObject
{
    Array <String> maps_;

    protected XMLFile @style_;

    StartGameMenu ()
    {

    }

    ~StartGameMenu ()
    {

    }

    void Start ()
    {
        style_ = cache.GetResource ("XMLFile", "UI/ColonizationUIStyle.xml");
        Window @startGameMenu = ui.root.GetChild ("mainMenu").GetChild ("startGameMenu");
        Button @startGameButton = startGameMenu.GetChild ("startGameButton");
        Button @backButton = startGameMenu.GetChild ("goBackButton");
        Button @selectMapButton = startGameMenu.GetChild ("selectMapButton");

        SubscribeToEvent (startGameButton, "Released", "HandleStartGameClick");
        SubscribeToEvent (backButton, "Released", "HandleGoBackClick");
        SubscribeToEvent (selectMapButton, "Released", "HandleSelectMapClick");
        ScanForMaps ();
    }

    void Update (float timeStep)
    {
        Window @startGameMenu = ui.root.GetChild ("mainMenu").GetChild ("startGameMenu");
        Slider @redSlider = startGameMenu.GetChild ("redSlider");
        Slider @greenSlider = startGameMenu.GetChild ("greenSlider");
        Slider @blueSlider = startGameMenu.GetChild ("blueSlider");

        float r = redSlider.value / redSlider.range;
        float g = greenSlider.value / greenSlider.range;
        float b = blueSlider.value / blueSlider.range;

        BorderImage @colorSample = startGameMenu.GetChild ("colorSample");
        colorSample.color = Color (r, g, b, 1.0f);

        Button @startGameButton = startGameMenu.GetChild ("startGameButton");
        LineEdit @nicknameEdit = startGameMenu.GetChild ("nicknameEdit");
        startGameButton.visible = (nicknameEdit.text != "");
        UpdateMapInfo ();

        int serverPort = node.parent.vars ["serverPort"].GetInt ();
        Text @serverPortLabel = startGameMenu.GetChild ("serverPortLabel");
        serverPortLabel.text = "Server will be started at port " + serverPort + ".";
    }

    void Stop ()
    {

    }

    void HandleStartGameClick ()
    {
        Window @startGameMenu = ui.root.GetChild ("mainMenu").GetChild ("startGameMenu");
        LineEdit @nicknameEdit = startGameMenu.GetChild ("nicknameEdit");
        Slider @redSlider = startGameMenu.GetChild ("redSlider");
        Slider @greenSlider = startGameMenu.GetChild ("greenSlider");
        Slider @blueSlider = startGameMenu.GetChild ("blueSlider");

        String nickname = nicknameEdit.text;
        int selectedMapIndex = node.parent.vars ["selectedMapIndex"].GetInt ();
        String selectedMapFolder = MAPS_FOLDER + maps_ [selectedMapIndex];
        String selectedMapInfo = selectedMapFolder + MAP_INFO_FILE;
        float r = redSlider.value / redSlider.range;
        float g = greenSlider.value / greenSlider.range;
        float b = blueSlider.value / blueSlider.range;

        VariantMap eventData;
        eventData [StartGameRequest::NICKNAME] = Variant (nickname);
        eventData [StartGameRequest::COLOR] = Variant (Color (r, g, b, 1.0f));
        eventData [StartGameRequest::SELECTED_MAP_FOLDER] = Variant (selectedMapFolder);
        eventData [StartGameRequest::SELECTED_MAP_INFO] = Variant (selectedMapInfo);
        SendEvent (EVENT_START_GAME_REQUEST, eventData);
    }

    void HandleGoBackClick ()
    {
        VariantMap eventData;
        eventData [ShowWindowRequest::WINDOW_NAME] = Variant ("primaryMenu");
        SendEvent (EVENT_SHOW_WINDOW_REQUEST, eventData);
    }

    void HandleSelectMapClick ()
    {
        ui.root.GetChild ("mainMenu").GetChild ("mapsList").visible = true;
    }

    protected void ScanForMaps ()
    {
        maps_ = GetMapsFoldersNames ();
        Array <String> mapsNamesList;
        for (uint index = 0; index < maps_.length; index++)
        {
            XMLFile @infoXML = cache.GetResource ("XMLFile", MAPS_FOLDER + maps_ [index] + MAP_INFO_FILE);
            XMLElement userInfo = infoXML.GetRoot ().GetChild ("userInformation");
            String name = userInfo.GetAttribute ("name");
            mapsNamesList.Push (name);
        }
        node.parent.vars ["mapsList"] = mapsNamesList;
    }

    protected void UpdateMapInfo ()
    {
        Window @startGameMenu = ui.root.GetChild ("mainMenu").GetChild ("startGameMenu");
        Text @mapLabel = startGameMenu.GetChild ("mapLabel");
        Text @mapInfo = startGameMenu.GetChild ("mapInfo");
        BorderImage @preview = startGameMenu.GetChild ("mapPreview");

        int selectedMapIndex = node.parent.vars ["selectedMapIndex"].GetInt ();
        String selectedMapFolder = MAPS_FOLDER + maps_ [selectedMapIndex];
        XMLFile @infoXML = cache.GetResource ("XMLFile", selectedMapFolder + MAP_INFO_FILE);
        XMLElement filesInfo = infoXML.GetRoot ().GetChild ("mapFiles");
        XMLElement userInfo = infoXML.GetRoot ().GetChild ("userInformation");

        mapLabel.text = "Map: " + userInfo.GetAttribute ("name") + ".";
        String infoText = "Short info: " + userInfo.GetAttribute ("shortInfo") + "\n\n";
        infoText += "Maximum players: " + userInfo.GetInt ("maxPlayers") + ".\n\n";
        infoText += "Supported victory types: " + userInfo.GetAttribute ("supportedVictoryTypes") + ".";
        mapInfo.text = infoText;
        preview.texture = cache.GetResource ("Texture2D", selectedMapFolder + filesInfo.GetAttribute ("preview"));
    }
}
