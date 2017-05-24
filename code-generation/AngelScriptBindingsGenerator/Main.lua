scriptDirectory = ""
local PrintConfiguration = require (scriptDirectory .. "PrintConfiguration")
local ReadFile = require (scriptDirectory .. "ReadFile")

Class = require (scriptDirectory .. "BindingTypes/CreateNewClass")
data, GetBindingTypesOfFile = require (scriptDirectory .. "Data")

bindingTypes = {}
bindingTypes ["Enum"] = require (scriptDirectory .. "BindingTypes/Enum")
bindingTypes ["Constant"] = require (scriptDirectory .. "BindingTypes/Constant")
bindingTypes ["Function"] = require (scriptDirectory .. "BindingTypes/Function")
bindingTypes ["Constructor"] = function (fileName, bindingAguments) return bindingTypes ["Function"] (fileName, bindingAguments, true) end
bindingTypes ["Urho3DSubsystem"] = require (scriptDirectory .. "BindingTypes/Urho3DSubsystem")

local configurationFile = arg [1]:gsub (".lua", "")
configuration = require (configurationFile)
PrintConfiguration ()

print ("Reading files...")
local filesCount = #configuration.files
for index, fileName in ipairs (configuration.files) do
    print ("[" .. (index * 100.0 / filesCount) .. "%] " .. fileName)
    if not ReadFile (fileName) then
        print ("Error while reading and parsing file!")
        return
    end
end
print ("\n")

print ("Enums: ")
for index, value in pairs (data.enums) do
    print (value:ToString ("    "))
end
print ("")

print ("Constants: ")
for index, value in pairs (data.constants) do
    print (value:ToString ("    "))
end
print ("")

print ("Free functions: ")
for index, value in pairs (data.freeFunctions) do
    print (value:ToString ("    "))
end
print ("")

print ("Classes: ")
for index, value in pairs (data.classes) do
    print (value:ToString ("    "))
end
print ("")

print ("Subsystems: ")
for index, value in pairs (data.subsystems) do
    print (value:ToString ("    "))
end
print ("")
