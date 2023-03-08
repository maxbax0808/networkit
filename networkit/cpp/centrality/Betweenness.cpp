/*
 * Betweenness.cpp
 *
 *  Created on: 29.07.2014
 *      Author: cls, ebergamini
 */

#include <memory>
#include <omp.h>

#include <networkit/auxiliary/Log.hpp>
#include <networkit/auxiliary/SignalHandling.hpp>
#include <networkit/centrality/Betweenness.hpp>
#include <networkit/distance/BFS.hpp>
#include <networkit/distance/Dijkstra.hpp>

namespace NetworKit {

Betweenness::Betweenness(const Graph &G, bool normalized, bool computeEdgeCentrality)
    : Centrality(G, normalized, computeEdgeCentrality) {}

void Betweenness::run() {
    std::cout << "Running single threaded betweeness\n";
    Aux::SignalHandler handler;
    const count z = G.upperNodeIdBound();
    scoreData.clear();
    scoreData.resize(z);
    if (computeEdgeCentrality) {
        count z2 = G.upperEdgeIdBound();
        edgeScoreData.clear();
        edgeScoreData.resize(z2);
    }

    std::vector<std::vector<double>> dependencies(1, std::vector<double>(z));
    std::unique_ptr<SSSP> ssps;
#pragma omp parallel
    {
        //omp_index i = omp_get_thread_num();
        if (G.isWeighted())
            ssps = std::unique_ptr<SSSP>(new Dijkstra(G, 0, true, true));
        else
            ssps = std::unique_ptr<SSSP>(new BFS(G, 0, true, true));
    }
    std::cout << "Getting to line 44\n";
    auto computeDependencies = [&](node s) -> void {
        std::vector<double> &dependency = dependencies[0];
        std::fill(dependency.begin(), dependency.end(), 0);
        std::cout << "Getting to line 48\n";

        // run SSSP algorithm and keep track of everything
        auto &sssp = *ssps;
        sssp.setSource(s);
        if (!handler.isRunning())
            return;
        sssp.run();
        if (!handler.isRunning())
            return;
        std::cout << "Getting to line 58\n";
        // compute dependencies for nodes in order of decreasing distance from s
        std::vector<node> stack = sssp.getNodesSortedByDistance();
        while (!stack.empty()) {
            node t = stack.back();
            stack.pop_back();
            for (node p : sssp.getPredecessors(t)) {
                // workaround for integer overflow in large graphs
                bigfloat tmp = sssp.numberOfPaths(p) / sssp.numberOfPaths(t);
                double weight;
                tmp.ToDouble(weight);
                double c = weight * (1 + dependency[t]);
                dependency[p] += c;

                if (computeEdgeCentrality) {
                    const edgeid edgeId = G.edgeId(p, t);
#pragma omp atomic
                    edgeScoreData[edgeId] += c;
                }
            }

            if (t != s)
#pragma omp atomic
                scoreData[t] += dependency[t];
        }
    };
    handler.assureRunning();
    G.balancedParallelForNodes(computeDependencies);
    handler.assureRunning();
    std::cout << "Getting to line 85\n";

    if (normalized) {
        // divide by the number of possible pairs
        const double n = static_cast<double>(G.numberOfNodes());
        const double pairs = (n - 2.) * (n - 1.);
        const double edges = n * (n - 1.);
        G.parallelForNodes([&](node u) { scoreData[u] /= pairs; });
        std::cout << "Getting to line 93\n";

        if (computeEdgeCentrality) {
//#pragma omp parallel for
            for (size_t i = 0; i < edgeScoreData.size(); ++i) {
                edgeScoreData[i] = edgeScoreData[i] / edges;
            }
        }
    }

    hasRun = true;
}

double Betweenness::maximum() {
    if (normalized) {
        return 1;
    }

    const double n = static_cast<double>(G.numberOfNodes());
    double score = (n - 1) * (n - 2);
    if (!G.isDirected())
        score /= 2.;
    return score;
}

} /* namespace NetworKit */
