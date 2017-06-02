
__curid = 1
function output(name,val)
   _G[name..__curid] = val
end


output("a",42)

print("".._G["a"..__curid])
