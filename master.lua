
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
setfenv(1, _G0)  --go back to global initialization
__curentNodeId = 33171712

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
__tweaksTable["path"] = "/home/jean/Work/icesl-next/models/meshes/pyr.stl"
shape = node_path("path","/home/jean/Work/NodeInterface/test4/node/knight.stl")

output("STL",load(shape))
setfenv(1, _G0)  --go back to global initialization
__curentNodeId = 36546176
__inputmap["shape"] = "STL33171712"

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
__tweaksTable["scale"] = 0.510000
ns = node_scalar("scale",0.05,0.0,1.0)
output("scaled",scale(ns)*input("shape"))
setfenv(1, _G0)  --go back to global initialization
__curentNodeId = 33167632
__inputmap["Shape"] = "scaled36546176"

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
__tweaksTable["brush"] = 0

emit(input("Shape"),node_int("brush",0))
setfenv(1, _G0)  --go back to global initialization
__curentNodeId = 34674752

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
__tweaksTable["Ray"] = 25.000000
n = node_scalar("Ray",5)
output("sphere",translate(0,0,0)*sphere(n))
setfenv(1, _G0)  --go back to global initialization
__curentNodeId = 33179760
__inputmap["Shape"] = "sphere34674752"

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
__tweaksTable["brush"] = 4

emit(input("Shape"),node_int("brush",0))
