ns = node_scalar("scale",0.05,0.0,1.0)
output("scaled",scale(ns)*input("shape"))
