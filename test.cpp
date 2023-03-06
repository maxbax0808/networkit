#include <iostream>
#include <networkit/graph/Graph.hpp>
#include <networkit/centrality/Betweenness.hpp>
#include <networkit/io/METISGraphReader.hpp>

int main() {
    // Load the graph from a file
    auto reader = NetworKit::METISGraphReader();
    NetworKit::Graph G = reader.read("../instances/104.txt");

    // Calculate the betweenness centrality
    NetworKit::Betweenness centrality(G);
    centrality.run();
    auto scores = centrality.scores();

    // Print the betweenness centrality scores
    for (NetworKit::node v = 0; v < 10; ++v) {
        std::cout << "Betweenness centrality of node " << v << ": " << scores[v] << std::endl;
    }

    return 0;
}