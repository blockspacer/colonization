if CreateNewClass == nil then
    CreateNewClass = require (scriptDirectory .. "BindingTypes/CreateNewClass")
end

if Tokens == nil then
    Tokens = require (scriptDirectory .. "Tokenization/Tokens")
end

if TokenToString == nil then
    TokenToString, TokenTypeToString = require (scriptDirectory .. "Tokenization/TokenToString")
end

Constant = CreateNewClass ()
Constant.Construct = function (self, fileName, bindingAguments)
    self.fileName = fileName
    self.type = ""
    self.name = ""
    self.arguments = bindingAguments
end

-- Return true if no errors.
Constant.Parse = function (self, tokensList)
    tokensList.skipEndOfLineTokens = true
    return (self:ReadType (tokensList) and self:ReadName (tokensList));
end

Constant.ToString = function (self, indent)
    local string = indent .. self.type .. " " .. self.name .. " from file " .. self.fileName .. "\n"
    return string
end

Constant.ReadType = function (self, tokensList)
    local token = tokensList:CurrentOrNextToken ()
    if token == nil then
        print ("Fatal error, token is nil!")
        return false
    elseif token.type ~= Tokens.TypeOrName then
        print ("Line " .. token.line .. ": Expected constant type, but got " .. TokenToString (token) .. "!")
        return false
    else
        self.type = token.value
        return true
    end
end

Constant.ReadName = function (self, tokensList)
    token = tokensList:NextToken ()
    if token == nil then
        print ("Fatal error, token is nil!")
        return false
    elseif token == nil or token.type ~= Tokens.TypeOrName then
        print ("Line " .. token.line .. ": Expected constant name, but got " .. TokenToString (token) .. "!")
        return false
    else
        self.name = token.value
        return true
    end
end

Constant.GetDataDestination = function ()
    return "constants"
end

Constant.GetTypeName = function ()
    return "Constant"
end

return Constant
