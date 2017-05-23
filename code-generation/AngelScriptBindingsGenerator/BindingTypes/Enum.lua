if Class == nil then
    Class = require (scriptDirectory .. "ElementsTypes/Class")
end

if Tokens == nil then
    Tokens = require (scriptDirectory .. "Tokenization/Tokens")
end

Enum = Class ()
Enum.Construct = function (self, fileName, bindingAguments)
    self.fileName = fileName
    self.name = ""
    self.values = {}
    self.arguments = bindingAguments
end

-- Return true if no errors.
Enum.Parse = function (self, tokensList)
    if not self:SkipUntilEnumKeyword (tokensList) then
        return false

    elseif not self:ReadName (tokensList) then
        return false

    elseif not self:ReadValues (tokensList) then
        return false
    else
        return true
    end
end

Enum.PrintInfo = function (self)
    print ("    " .. self.name .. " from file " .. self.fileName)
    for index, value in pairs (self.values) do
        print ("        " .. value)
    end
    print ("")
end

Enum.SkipUntilEnumKeyword = function (self, tokensList)
    local token = tokensList:CurrentToken ()
    while token.type ~= Tokens.TypeOrName and token ~= nil do
        token = tokensList:NextToken ()
    end

    if token == nil then
        print ("End of file reached when trying to find \"enum\" keyword while reading enum!")
        return false

    elseif token.value ~= "enum" then
        print ("Line " .. token.line .. ": Expected \"enum\", but got \"" .. token.value .. "\"!")
        return false
    else
        return true
    end
end

Enum.ReadName = function (self, tokensList)
    local token = tokensList:NextToken ()
    while token.type ~= Tokens.TypeOrName and token ~= nil do
        token = tokensList:NextToken ()
    end

    if token == nil then
        print ("End of file reached when trying to read name while reading enum!")
        return false

    else
        self.name = token.value
        return true
    end
end

Enum.ReadValues = function (self, tokensList)
    if not self:SkipUntilValuesBlockBegin (tokensList) then
        return false
    end

    local isNextNameTokenEnumValue = true
    local token = tokensList:NextToken ()

    while token ~= nil and not (token.type == Tokens.Operator and token.value == "}") do
        if token.type == Tokens.TypeOrName and isNextNameTokenEnumValue then
            table.insert (self.values, token.value)
            isNextNameTokenEnumValue = false
        elseif token.type == Tokens.Operator and token.value == "," then
            isNextNameTokenEnumValue = true
        end
        token = tokensList:NextToken ()
    end

    if token == nil then
        print ("End of file reached when trying to read values names while reading enum!")
    else
        return true
    end
end

Enum.SkipUntilValuesBlockBegin = function (self, tokensList)
    local token = tokensList:NextToken ()
    while token.type ~= Tokens.Operator and token ~= nil do
        token = tokensList:NextToken ()
    end

    if token == nil then
        print ("End of file reached when trying to find value block begin while reading enum!")
        return false

    elseif token.value ~= "{" then
        print ("Line " .. token.line .. ": Expected \"{\", but got \"" .. token.value .. "\"!")
        return false
    else
        return true
    end
end

Enum.GetDataDestination = function ()
    return "enums"
end

Enum.GetTypeName = function ()
    return "Enum"
end

return Enum;
