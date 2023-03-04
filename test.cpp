#include <iostream>
#include <networkit/graph/Graph.hpp>
#include <networkit/centrality/Betweenness.hpp>
#include <networkit/io/GraphIO.hpp>

int main() {
    // Load the graph from a file
    NetworKit::Graph G = NetworKit::GraphIO::readGraph("../instances/105.txt", NetworKit::FileType::METIS);

    // Calculate the betweenness centrality
    NetworKit::Betweenness centrality(G);
    centrality.run();
    auto scores = centrality.scores();

    // Print the betweenness centrality scores
    for (NetworKit::node v = 0; v < G.numberOfNodes(); ++v) {
        std::cout << "Betweenness centrality of node " << v << ": " << scores[v] << std::endl;
    }

    return 0;
}