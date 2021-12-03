from random import randint


V = 1000
E = 2250


graph = [[0 for _ in range(V)] for _ in range(V)]

count = 0
while count < E:
	a,b = randint(0,V-1), randint(0,V-1)
	if graph[a][b] == 0 and a != b:
		graph[a][b] = 1
		graph[b][a] = 1
		count +=1

for i in range(V):
	# if a vertex is disconnected, randomly generate it a friend
	if sum(graph[i]) == 0:
		x = randint(0, V-1)
		while x != i:
			x = randint(0, V-1)
		graph[i][x] = 1 
		graph[x][i] = 1
		E+=1
			

D = max(sum(graph[i]) for i in range(V))

print(V)
print(E)
print(D)
print(V) #this is size of initial coloring

#print edgelist
for i in range(V):
	for j in range(i+1, V):
		if graph[i][j] == 1:
			print(i,j)

#print coloring (default is just use vid as color)
for i in range(V):
	print(i)
