
local _G0 = {}        -- this environment will be use to swap variables between scripts.
local _Gcurrent = {}  -- this is the environment local to the script : _Gc includes _G0
__inputmap = {}
__tweaksTable = {}
function input(name)
    return _G[__inputmap[name]]
end

function output(name,val)
    setfenv(1, _G0)    -- swap the two env
    _G[name..__curentNodeId] = val
    setfenv(1,_Gcurrent) --go back to current env
end
setmetatable(_G0, {__index = _G})
