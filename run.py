import networkit as nk

G = nk.readGraph("../instances/105.txt", nk.Format.METIS)

btwn = nk.centrality.Betweenness(G)

btwn.run()

print(btwn.ranking()[:10])