x = node_scalar("x",0.0)
y = node_scalar("y",0.0)
z = node_scalar("z",0.0)
output("translated",translate(x,y,z)*input("shape"))
