__number_of_shapes = 1
__emited_shape = {}
function emit(name,val)
   __emited_shape[__number_of_shapes] = name
   __number_of_shapes = __number_of_shapes+1	
end

function emit(name)
   __emited_shape[__number_of_shapes] = name
   __number_of_shapes = __number_of_shapes+1	
end
