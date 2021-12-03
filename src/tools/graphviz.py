import networkx as nx
import matplotlib.pyplot as plt

n = int(input())
e = int(input())
d = int(input())


G = nx.Graph()
G.add_nodes_from([i for i in range(n)])
edges = []
for _ in range(e):
	x,y = [int(x) for x in input().split()]
	edges.append((x,y))

r = int(input())
	
G.add_edges_from(edges)
colors = []
colorset = set()
for _ in range(n):
	c = int(input())
	colors.append(c)
	colorset.add(c)

flag = True
for i in range(n):
	if flag:
		break
	for j in range(i,n):
		if ((i,j) in edges or (j,i) in edges) and color[i] == color[j]:
			flag = False
			break
print("the graph had {} vertices with max degree {} and an input {} coloring".format(n,d,n))
print("after {} rounds of reduction there were".format(r))

if flag:
	print("No color conflicts detected")
	print("so this is a valid", len(colorset), "coloring.")
else:
	print("Color conflicts detected.")

t = input() #just pull the last line in for fun	

print(t)

if n < 300:
	nx.draw(G, node_color=colors, with_labels=True)
	plt.show() 
else:
	print("Graph too large. Not showing visual")
