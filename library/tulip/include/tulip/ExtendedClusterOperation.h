#ifndef TLPEXTENDEDCLUSTEROPERATIONS
#define TLPEXTENDEDCLUSTEROPERATIONS

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <set>
#include <string>
#include <tulip/Node.h>
#include <tulip/Graph.h>
#include <tulip/GraphProperty.h>

namespace tlp {
  /**
   * Function to open a metanode and replace all edges between that
   * meta node and other nodes in the graph.
   *
   * \param graph The graph where the metanode is contained and where
   * the subgraph will be replaced.
   * \param n The meta node to open.
   * \param groupUnderSubgraph The subgraph where the groups are placed 
   * \param metaGraphProperty The metagraph property used to associate 
   * the subgraph to metanodes
   */
  TLP_SCOPE void openMetaNode(Graph *graph, node metaNode,
			      Graph *groupUnderSubGraph,
			      GraphProperty *metaGraphProperty);

  /**
   * Function to open a metanode and replace all edges between that
   * meta node and other nodes in the graph.  Provided for backwards
   * compatability with groupUnderSubGraph set to graph->getSuperGraph(),
   * and metaGraphs set to viewMetaGraph.
   */
  TLP_SCOPE void openMetaNode(Graph *graph, node n);

  /**
   * Function to close a subgraph into a metanode.  Edges from nodes
   * in the subgraph to nodes outside the subgraph are replaced with
   * edges from the metanode to the nodes outside the subgraph.
   *
   * \param graph The graph where the meta node will be placed.
   * \param subGraph The set of nodes to be contracted into a metanode.
   * \param groupUnderSubgraph The subgraph where the groups are placed. 
   * \param metaGraphProperty The metagraph property used to associate
   * the subgraph to the new metanode.
   */
  TLP_SCOPE node createMetaNode(Graph *graph, std::set<node> &subGraph,
				Graph *groupUnderSubGraph, 
				GraphProperty *metaInfo);

  /**
   * Function to close a subgraph into a metanode.  Edges from nodes
   * in the subgraph to nodes outside the subgraph are replaced with
   * edges from the metanode to the nodes outside the subgraph.
   * Version for backwards compatibility with groupUnderSubGraph set
   * to graph->getSuperGraph(), multiEdges set to true,
   * metaInfo set to viewMetaGraph, and delAllEdge set to true.
   */
  TLP_SCOPE node createMetaNode(Graph *graph, 
				std::set<node> &subGraph);
  /**
   *  Return a subgraph induced by a set of nodes
   */
  TLP_SCOPE Graph *inducedSubGraph(Graph *, const std::set<node> &, std::string name = "induced subgraph");
}

#endif
