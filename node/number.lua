--[[
function input(name)
inputCallback(name)
return _G[prefixInput(name)]
end

function output(name,val)
outputCallback(name)
_G[prefixOutput(name)] = val
end
]]--


output("output", 42)


