import matplotlib.pyplot as plt
import networkx as nx
import sys
assignment = sys.argv[1]
graph_conn = sys.argv[2]

G=nx.Graph()

assigned = {}
with open(assignment) as file:
	for line in file:
		(key, val) = line.split()
		assigned[key] = val
		G.add_node(key)

with open(graph_conn) as file:
	for line in file:
		(key, val) = line.split()
		G.add_edge(key,val)

pos=nx.spring_layout(G)

for node in assigned:
	nx.draw_networkx_nodes(G,pos,nodelist=[node],node_color=assigned[node],node_size=100)
nx.draw_networkx_edges(G,pos)
nx.draw_networkx_labels(G,pos,font_size='15')
plt.axis('off')
plt.savefig(graph_conn+'.png') 