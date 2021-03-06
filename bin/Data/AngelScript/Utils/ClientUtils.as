#include "AngelScript/Utils/Constants.as"

shared Node @GetScriptMain (Node @requester)
{
    Array <Node @> nodesWithTag = requester.scene.GetChildrenWithTag ("ScriptMain", true);
    if (not nodesWithTag.empty)
    {
        return nodesWithTag [0];
    }
    else
    {
        return null;
    }
}

shared bool CheckIsSceneLoaded (Scene @scene_)
{
    if (scene_.vars [VAR_REPLICATED_NODES_COUNT].GetInt () != 0)
    {
        Array <Node @> children = scene_.GetChildren (true);
        int replicated = 0;
        for (uint index = 0; index < children.length; index++)
        {
            if (children [index].id < FIRST_LOCAL_ID)
            {
                replicated++;
            }
        }
        return (replicated == scene_.vars [VAR_REPLICATED_NODES_COUNT].GetInt ());
    }
    else
        return false;
}

shared PlayerInfo @GetPlayerInfoByIndex (Scene @scene_, uint index)
{
    if (scene_.GetChild ("players") is null)
    {
        return null;
    }

    Array <Node @> playersNodes = scene_.GetChild ("players").GetChildrenWithComponent ("PlayerInfo");
    if (playersNodes.empty)
    {
        return null;
    }

    if (index < playersNodes.length)
    {
        return playersNodes [index].GetComponent ("PlayerInfo");
    }
    else
    {
        return null;
    }
}

shared Array <String> GetPlayersNamesList (Scene @scene_)
{
    Array <String> playersNames;
    if (scene_.GetChild ("players") is null)
    {
        return playersNames;
    }

    Array <Node @> playersNodes = scene_.GetChild ("players").GetChildrenWithComponent ("PlayerInfo");
    if (playersNodes.empty)
    {
        return playersNames;
    }

    for (uint index = 0; index < playersNodes.length; index++)
    {
        PlayerInfo @playerInfo = playersNodes [index].GetComponent ("PlayerInfo");
        playersNames.Push (playerInfo.name);
    }
    return playersNames;
}

shared String ConstructWarShortname (Scene @scene_, DiplomacyWar @war)
{
    PlayerInfo @firstAttacker = GetPlayerInfoByNameHash (scene_, war.GetAttackerNameHashByIndex (0));
    PlayerInfo @firstDefender = GetPlayerInfoByNameHash (scene_, war.GetDefenderNameHashByIndex (0));
    String shortname = firstAttacker.name;
    if (war.attackersCount > 1)
    {
        shortname += " (" + war.attackersCount + ")";
    }

    shortname += " vs ";
    shortname += firstDefender.name;
    if (war.defendersCount > 1)
    {
        shortname += " (" + war.defendersCount + ")";
    }
    return shortname;
}

shared Array <String> GetWarsShortnamesList (Scene @scene_, Array <StringHash> &out warsHashesArray)
{
    Array <String> warsShortnames;
    if (scene_.GetChild ("diplomacy") is null)
    {
        return warsShortnames;
    }

    Array <Node @> warsNodes = scene_.GetChild ("diplomacy").GetChildrenWithComponent ("DiplomacyWar");
    if (warsNodes.empty)
    {
        return warsShortnames;
    }

    for (uint index = 0; index < warsNodes.length; index++)
    {
        DiplomacyWar @war = warsNodes [index].GetComponent ("DiplomacyWar");
        warsHashesArray.Push (war.hash);
        warsShortnames.Push (ConstructWarShortname (scene_, war));
    }
    return warsShortnames;
}

DiplomacyWar @GetWarByHash (Scene @scene_, StringHash warHash)
{
    if (scene_.GetChild ("diplomacy") is null)
    {
        return null;
    }

    Array <Node @> warsNodes = scene_.GetChild ("diplomacy").GetChildrenWithComponent ("DiplomacyWar");
    if (warsNodes.empty)
    {
        return null;
    }

    DiplomacyWar @war = warsNodes [0].GetComponent ("DiplomacyWar");
    uint index = 1;
    while (war.hash != warHash and index < warsNodes.length)
    {
        war = warsNodes [index].GetComponent ("DiplomacyWar");
        index++;
    }

    if (war.hash == warHash)
    {
        return war;
    }
    else
    {
        return null;
    }
}

shared Component @GetFirstComponentOfNodeDerivedFrom (Node @node_, StringHash type)
{
    Array <Component @> components = node_.GetComponents ();
    for (uint index = 0; index < components.length; index++)
    {
        Component @component = components [index];
        if ((cast <Unit> (component)) !is null)
        {
            return component;
        }
    }
    return null;
}

shared Unit @GetUnitByHash (Scene @scene_, StringHash unitHash)
{
    if (scene_.GetChild ("units") is null)
    {
        return null;
    }

    Array <Node @> unitsNodes = scene_.GetChild ("units").GetChildrenWithTag (TAG_UNIT);
    if (unitsNodes.empty)
    {
        return null;
    }

    Unit @unit = cast <Unit> (GetFirstComponentOfNodeDerivedFrom (unitsNodes [0], StringHash ("Unit")));
    uint index = 1;
    while (unit.hash != unitHash and index < unitsNodes.length)
    {
        unit = cast <Unit> (GetFirstComponentOfNodeDerivedFrom (unitsNodes [index], StringHash ("Unit")));
        index++;
    }

    if (unit.hash == unitHash)
    {
        return unit;
    }
    else
    {
        return null;
    }
}

shared Battle @GetBattleByHash (Scene @scene_, StringHash battleHash)
{
    if (scene_.GetChild ("battles") is null)
    {
        return null;
    }

    Array <Node @> battlesNodes = scene_.GetChild ("battles").GetChildrenWithTag (TAG_BATTLE);
    if (battlesNodes.empty)
    {
        return null;
    }

    Battle @battle = battlesNodes [0].GetComponent ("Battle");
    uint index = 1;
    while (battle.hash != battleHash and index < battlesNodes.length)
    {
        battle = battlesNodes [index].GetComponent ("Battle");
        index++;
    }

    if (battle.hash == battleHash)
    {
        return battle;
    }
    else
    {
        return null;
    }
}

Array <DiplomacyWar @> GetWarsWithPlayer (Scene @scene_, StringHash playerNameHash)
{
    Array <DiplomacyWar @> wars;
    if (scene_.GetChild ("diplomacy") is null)
    {
        return wars;
    }

    Array <Node @> warsNodes = scene_.GetChild ("diplomacy").GetChildrenWithComponent ("DiplomacyWar");
    if (warsNodes.empty)
    {
        return wars;
    }

    for (uint index = 0; index < warsNodes.length; index++)
    {
        DiplomacyWar @war = warsNodes [index].GetComponent ("DiplomacyWar");
        if (war.IsAttacker (playerNameHash) or war.IsDefender (playerNameHash))
        {
            wars.Push (war);
        }
    }
    return wars;
}

Array <DiplomacyWar @> FindWarsWhereThesePlayersFight (Scene @scene_, StringHash firstNameHash, StringHash secondNameHash)
{
    Array <DiplomacyWar @> wars;
    Array <DiplomacyWar @> warsOfFirstPlayer = GetWarsWithPlayer (scene_, firstNameHash);
    if (warsOfFirstPlayer.empty)
    {
        return wars;
    }

    for (uint index = 0; index < warsOfFirstPlayer.length; index++)
    {
        DiplomacyWar @war = warsOfFirstPlayer [index];
        if ((war.IsAttacker (firstNameHash) and war.IsDefender (secondNameHash)) or
            (war.IsDefender (firstNameHash) and war.IsAttacker (secondNameHash)))
        {
            wars.Push (war);
        }
    }
    return wars;
}

bool IsCanSettleInDistrict (District @district, String playerName)
{
    return not district.isSea and not district.isImpassable and
           (district.colonyOwnerName == playerName or not district.hasColony);
}

shared void RegisterLineEdit (Node @scriptMain, LineEdit @lineEdit)
{
    Array <Variant> lineEditVector = scriptMain.vars ["lineEditVector"].GetVariantVector ();
    lineEditVector.Push (Variant (lineEdit));
    scriptMain.vars ["lineEditVector"] = Variant (lineEditVector);
}

shared void UnregisterLineEdit (Node @scriptMain, LineEdit @lineEdit)
{
    Array <Variant> lineEditVector = scriptMain.vars ["lineEditVector"].GetVariantVector ();
    uint index = lineEditVector.Find (Variant (lineEdit));
    if (index >= 0)
    {
        lineEditVector.Erase (index);
    }
    scriptMain.vars ["lineEditVector"] = Variant (lineEditVector);
}

shared bool IsAnyLineEditFocused (Node @scriptMain)
{
    Array <Variant> lineEditVector = scriptMain.vars ["lineEditVector"].GetVariantVector ();
    for (uint index = 0; index < lineEditVector.length; index++)
    {
        LineEdit @lineEdit = lineEditVector [index].GetPtr ();
        if (lineEdit.focus)
        {
            return true;
        }
    }
    return false;
}

Array <String> GetMapsFoldersNames (String filesystemPrefix = FILESYSTEM_PREFIX,
     String mapsFolder = MAPS_FOLDER,
     String mapInfoFileName = MAP_INFO_FILE,
     String filter = EMPTY_FILTER)
{
    Array <String> foldersNames;
    foldersNames = fileSystem.ScanDir (filesystemPrefix + mapsFolder, mapInfoFileName, SCAN_DIRS, false);
    // Check maps list and delete items if they aren't maps.
    uint index = 0;
    while (index < foldersNames .length)
    {
        if (fileSystem.FileExists (filesystemPrefix + mapsFolder + foldersNames [index] + mapInfoFileName))
        {
            index++;
        }
        else
        {
            foldersNames.Erase (index);
        }
    }
    return foldersNames;
}
