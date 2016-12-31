class Player : ScriptObject
{
    protected ActivitiesApplication @activitiesApplication_;
    protected String playerName_;
    protected bool isSceneLoaded_;
    
    protected void CheckIsSceneLoaded ()
    {
        if (scene.vars ["ReplicatedNodesCount"].GetInt () != 0)
        {
            Array <Node @> children = scene.GetChildren (true);
            int replicated = 0;
            for (int index = 0; index < children.length; index++)
                if (children [index].id < FIRST_LOCAL_ID)
                    replicated++;
            isSceneLoaded_ = (replicated == scene.vars ["ReplicatedNodesCount"].GetInt ());
        }
        else
            isSceneLoaded_ = false;
    }
    
    Player ()
    {
        isSceneLoaded_ = false;
    }
    
    ~Player ()
    {
        
    }
    
    void Init ()
    {
        
    }
    
    void Start ()
    {
        XMLFile@ style = cache.GetResource ("XMLFile", "UI/DefaultStyle.xml");
        ui.root.defaultStyle = style;
        
        Node @uiScriptNode = node.CreateChild ("uiScriptNode", LOCAL);
        ScriptInstance @uiScript = uiScriptNode.CreateComponent ("ScriptInstance", LOCAL);
        uiScript.CreateObject (cache.GetResource ("ScriptFile",
                                                         "AngelScript/Client/PlayerUi.as"),
                                     "PlayerUi");
                                     
        Node @networkScriptNode = node.CreateChild ("networkScriptNode", LOCAL);
        ScriptInstance @networkScript = networkScriptNode.CreateComponent ("ScriptInstance", LOCAL);
        networkScript.CreateObject (cache.GetResource ("ScriptFile",
                                                         "AngelScript/Client/PlayerNetwork.as"),
                                     "PlayerNetwork");
                                     
        Node @sceneManagerScriptNode = node.CreateChild ("sceneManagerScriptNode", LOCAL);
        ScriptInstance @sceneManagerScript = sceneManagerScriptNode.CreateComponent ("ScriptInstance", LOCAL);
        sceneManagerScript.CreateObject (cache.GetResource ("ScriptFile",
                                                         "AngelScript/Client/SceneManager.as"),
                                     "SceneManager");
        
        Node @screenPressesHandlerScriptNode = node.CreateChild ("screenPressesHandlerScriptNode", LOCAL);
        ScriptInstance @screenPressesHandlerScript = screenPressesHandlerScriptNode.CreateComponent ("ScriptInstance", LOCAL);
        screenPressesHandlerScript.CreateObject (cache.GetResource ("ScriptFile",
                                                         "AngelScript/Client/ScreenPressesHandler.as"),
                                     "ScreenPressesHandler");
        
        SubscribeToEvent ("ServerDisconnected", "HandleServerDisconnected");
        SubscribeToEvent ("ConnectFailed", "HandleConnectFailed");
    }
    
    void Update (float timeStep)
    {
        if (node.vars ["goToMenuCalled"].GetBool ())
            GoToMainMenuState ();
            
        if (!isSceneLoaded_)
            CheckIsSceneLoaded ();
    }
    
    void Stop ()
    {
        UnsubscribeFromAllEvents ();
    }
    
    ActivitiesApplication @get_activitiesApplication ()
    {
        return activitiesApplication_;
    }
    
    void set_activitiesApplication (ActivitiesApplication @activitiesApplication)
    {
        activitiesApplication_ = activitiesApplication;
    }
    
    String get_playerName ()
    {
        return playerName_;
    }
    
    void set_playerName (String playerName)
    {
        playerName_ = playerName;
        node.vars ["playerName"] = playerName_;
    }
    
    void HandleServerDisconnected ()
    {
        ErrorDialog ("Disconnected from server!", "Disconnected from server!\nPress OK to return to main menu.");
        GoToMainMenuState ();
    }
    
    void GoToMainMenuState ()
    {
        for (int index = 0; index < activitiesApplication_.GetActivitiesCount (); index++)
            activitiesApplication_.StopActivityNextFrame (activitiesApplication_.GetActivityByIndex (index));
         
        MainMenuActivity @mainMenuActivity = MainMenuActivity ();
        activitiesApplication_.SetupActivityNextFrame (mainMenuActivity);
    }
    
    void HandleConnectFailed ()
    {
        ErrorDialog ("Connect failed!", "Connect failed!\nPress OK to return to main menu.");
        GoToMainMenuState ();
    }
};

