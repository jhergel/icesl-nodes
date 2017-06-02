
_Gcurrent = {} -- clear _Gcurrent
setmetatable(_Gcurrent, {__index = _G0}) --copy index from _G0
setfenv(1, _Gcurrent)    -- set it
--function for the tweaks. If you want to add a tweaks add a function here. It must the the exact same stuff.
__tweaksTable = {}
function node_scalar(name,val)
    return __tweaksTable[name]
end
function node_string(name,val)
    return __tweaksTable[name]
end
function node_int(name,val)
    return __tweaksTable[name]
end
function node_path(name,val)
    return __tweaksTable[name]
end
