shape = input("Shape")


seed = input("seed")
ds = node_scalar("distrib",0.5,0.0,1.0)
tr = node_scalar("translate",0.0)
s = distribute(shape, ds)

centroids = {} 
for i = 1,#s,1 do
	--centroids[i] = translate(s[i][1]) * frame(s[i][2]) * merge{ cone(s[i][3],0,1), mirror(Z) * cone(s[i][3],0,1) }
	centroids[i] = translate(s[i][1]) * frame(s[i][2]) * merge{ translate(0,0,tr)*seed, mirror(Z) * translate(0,0,tr)*seed};
	--print(''..s[i][3])
end

output("Sout",merge(centroids))
