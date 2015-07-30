/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include <tulip/tuliphash.h>
#include <tulip/TulipPluginHeaders.h>

using namespace std;
using namespace tlp;

/** \addtogroup clustering */
/*@{*/
/** \file
* \brief  An implementation of the Louvain clustering algorithm
*
* This plugin is an implementation of the Louvain clustering algorithm
* first published as:
*
* Blondel, V.D. and Guillaume, J.L. and Lambiotte, R. and Lefebvre, E., \n
* "Fast unfolding of communities in large networks", \n
* "Journal of Statistical Mechanics: Theory and Experiment, P10008",\n
* 2008. \n
*
* <b> HISTORY</b>
*
* - 25/02/2011 Version 1.0: Initial release (François Queyroi)
* - 13/05/2011 Version 2.0 (Bruno Pinaud): Change plugin type from General Algorithm to DoubleAlgorithm, code cleaning and fix some memory leaks.
* - 09/06/2015 Version 2.1 (Patrick Mary) full rewrite according the updated version of the original source code available at  https://sites.google.com/site/findcommunities/
*
* \note A threshold for modularity improvement is used here, its value is 0.000001
*
* \author Patrick Mary, Labri
*
*
**/
class LouvainClustering : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION("Louvain","Patrick Mary","09/06/15",
                    "Nodes partitioning measure used for community detection."
                    "This is an implementation of the Louvain clustering algorithm first published as:<br/>"
                    "<b>Fast unfolding of communities in large networks</b>, Blondel, V.D. and Guillaume, J.L. and Lambiotte, R. and Lefebvre, E., Journal of Statistical Mechanics: Theory and Experiment, P10008 (2008).",
                    "2.1", "Clustering")
  LouvainClustering(const tlp::PluginContext*);
  bool run();
private:
  // the number of nodes of the original graph
  unsigned int nb_nodes;

  // a quotient graph of the original graph
  VectorGraph* quotient;
  // number of nodes in the quotient graph and size of all vectors
  unsigned int size;

  // the mapping between the nodes of the original graph
  // and the quotient nodes
  std::vector<unsigned int> clusters;

  // quotient graph edge weights
  EdgeProperty<double>* weights;
  // total weight (sum of edge weights for the quotient graph)
  double total_weight;

  std::vector<double> neigh_weight;
  std::vector<unsigned int> neigh_pos;
  unsigned int neigh_last;

  // community to which each node belongs
  std::vector<unsigned int> n2c;
  // used to renumber the communities
  std::vector<int> renumber;
  // used to compute the modularity participation of each community
  std::vector<double> in,tot;

  // a new pass is computed if the last one has generated an increase
  // greater than min_modularity
  // if 0. even a minor increase is enough to go for one more pass
  double min_modularity;

  // return the weighted degree and selfloops of a node
  // of the current quotient graph
  void get_weighted_degree_and_selfloops(unsigned int n, double& wdg, double& nsl) {
    wdg = nsl = 0;
    const std::vector<edge>& edges = quotient->star(node(n));

    for (unsigned int i = 0; i < edges.size(); ++i) {
      edge e = edges[i];
      double weight = (*weights)[e];
      wdg += weight;
      // self loop must be counted only once
      std::pair<node, node> ends = quotient->ends(e);

      if (ends.first == ends.second) {
        nsl = weight;
        ++i;
      }
    }
  }

  // compute the gain of modularity if node where inserted in comm
  // given that node has dnodecomm links to comm.  The formula is:
  // [(In(comm)+2d(node,comm))/2m - ((tot(comm)+deg(node))/2m)^2]-
  // [In(comm)/2m - (tot(comm)/2m)^2 - (deg(node)/2m)^2]
  // where In(comm)    = number of half-links strictly inside comm
  //       Tot(comm)   = number of half-links inside or outside comm (sum(degrees))
  //       d(node,com) = number of links from node to comm
  //       deg(node)   = node degree
  //       m           = number of links
  double modularity_gain(unsigned int /*node*/, unsigned int comm,
                         double dnode_comm, double w_degree) {
    return (dnode_comm - tot[comm] * w_degree/total_weight);
  }


  // compute the modularity of the current partition
  double modularity()  {
    double q  = 0.;
    double m2 = total_weight;

    for (unsigned int i=0 ; i<size ; i++) {
      if (tot[i]>0)
        q += in[i]/m2 - (tot[i]/m2)*(tot[i]/m2);
    }

    return q;
  }

  // compute the set of neighboring communities of node
  // for each community, gives the number of links from node to comm
  void neigh_comm(unsigned int n) {
    for (unsigned int i=0 ; i<neigh_last ; i++)
      neigh_weight[neigh_pos[i]]=-1;

    neigh_last=0;

    neigh_pos[0]=n2c[n];
    neigh_weight[neigh_pos[0]]=0;
    neigh_last=1;

    const std::vector<edge>& edges = quotient->star(node(n));
    unsigned int nb_edges = edges.size();

    for (unsigned int j = 0; j < nb_edges; ++j) {
      edge e = edges[j];
      std::pair<node, node> ends = quotient->ends(e);
      unsigned int neigh = (ends.first == node(n)) ? ends.second : ends.first;
      unsigned int neigh_comm   = n2c[neigh];
      double neigh_w = (*weights)[e];

      if (neigh!=n) {
        if (neigh_weight[neigh_comm]==-1) {
          neigh_weight[neigh_comm]=0.;
          neigh_pos[neigh_last++]=neigh_comm;
        }

        neigh_weight[neigh_comm]+=neigh_w;
      }
    }
  }

  // generates the quotient graph of communities as computed by one_level
  void partitionToQuotient(VectorGraph* new_quotient,
                           EdgeProperty<double>* new_weights) {
    // Renumber communities
    vector<int> renumber(size, -1);

    for (unsigned int n=0 ; n<size ; n++) {
      renumber[n2c[n]]++;
    }

    int final=0;

    for (unsigned int i=0 ; i<size ; i++)
      if (renumber[i]!=-1)
        renumber[i]=final++;

    // update clustering
    for(unsigned int i = 0; i < nb_nodes; ++i) {
      clusters[i] = renumber[n2c[clusters[i]]];
    }

    // Compute weighted graph
    for (int i = 0; i < final; ++i)
      new_quotient->addNode();

    total_weight = 0;
    const std::vector<edge>& edges = quotient->edges();
    unsigned int nb_edges = edges.size();

    for(unsigned int i = 0; i < nb_edges; ++i) {
      edge e = edges[i];
      std::pair<node, node> ends = quotient->ends(e);
      node src = ends.first;
      node tgt = ends.second;
      unsigned int src_comm = renumber[n2c[src]];
      unsigned int tgt_comm = renumber[n2c[tgt]];
      double weight = (*weights)[e];
      edge e_comm =
        new_quotient->existEdge(node(src_comm), node(tgt_comm), false);
      total_weight += weight;
      double* weight_comm = NULL;

      if (!e_comm.isValid()) {
        ends = make_pair(node(src_comm), node(tgt_comm));
        e_comm = new_quotient->addEdge(node(src_comm), node(tgt_comm));
        weight_comm = &((*new_weights)[e_comm]);
        *weight_comm = weight;
      }
      else {
        ends = new_quotient->ends(e_comm);
        weight_comm = &((*new_weights)[e_comm]);

        if (ends.second == node(tgt_comm)) {
          *weight_comm += weight;
        }
      }

      // self loop are counted only once
      if (src != tgt) {
        total_weight += weight;

        if (ends.first == node(tgt_comm)) {
          *weight_comm += weight;
        }
      }
    }

    delete quotient;
    delete weights;
    quotient = new_quotient;
    weights = new_weights;
  }

  // compute communities of the graph for one level
  // return true if some nodes have been moved
  bool one_level() {
    bool improvement=false ;
    int nb_moves;
    double new_mod   = modularity();
    double cur_mod   = new_mod;

    quotient->shuffleNodes();

    // repeat while
    // there is an improvement of modularity
    // or there is an improvement of modularity greater than a given epsilon
    do {
      cur_mod = new_mod;
      nb_moves = 0;
      // for each node:
      // remove the node from its community
      // and insert it in the best community
      for (unsigned int n = 0 ; n <size ; n++) {
        unsigned int n_comm     = n2c[n];
        double n_wdg;
        double n_nsl;
        get_weighted_degree_and_selfloops(n, n_wdg, n_nsl);

        // computation of all neighboring communities of current node
        neigh_comm(n);

        // remove node from its current community
        tot[n_comm] -= n_wdg;
        in[n_comm]  -= 2*neigh_weight[n_comm] + n_nsl;

        // compute the nearest community for node
        // default choice for future insertion is the former community
        unsigned int best_comm        = n_comm;
        double best_nblinks  = 0.;
        double best_increase = 0.;

        for (unsigned int i=0 ; i<neigh_last ; i++) {
          double increase =
            modularity_gain(n, neigh_pos[i], neigh_weight[neigh_pos[i]], n_wdg);

          if (increase>best_increase  ||
              // keep the best cluster with the minimum id
              (increase == best_increase && neigh_pos[i] > best_comm)) {
            best_nblinks  = neigh_weight[neigh_pos[i]];
            best_increase = increase;
            best_comm = neigh_pos[i];
          }
        }

        // insert node in the nearest community
        tot[best_comm] += n_wdg;
        in[best_comm]  += 2*best_nblinks + n_nsl;
        n2c[n] = best_comm;


        if (best_comm!=n_comm)
          nb_moves++;
      }

      new_mod = modularity();

      if (nb_moves>0)
        improvement=true;

    }
    while ((nb_moves>0) && ((new_mod-cur_mod)>min_modularity));

    return improvement;
  }

  void init_level() {
    size = quotient->numberOfNodes();
    neigh_weight.resize(size,-1);
    neigh_pos.resize(size);
    neigh_last=0;

    n2c.resize(size);
    in.resize(size);
    tot.resize(size);

    for (unsigned int i=0 ; i < size ; i++) {
      n2c[i] = i;
      double wdg, nsl;
      get_weighted_degree_and_selfloops(i, wdg, nsl);
      in[i]  = nsl;
      tot[i] = wdg;
    }
  }

};
/*@}*/

//========================================================================================
PLUGIN(LouvainClustering)
//========================================================================================
//========================================================================================
namespace {
const char * paramHelp[] = {
  // metric
  HTML_HELP_OPEN()              \
  HTML_HELP_DEF( "type", "NumericProperty" )       \
  HTML_HELP_DEF( "value", "An existing edge metric" )                 \
  HTML_HELP_BODY()              \
  "An existing edge weight metric property. If it is not defined all edges have a weight of 1.0."\
  // precision
  HTML_HELP_CLOSE(),
  HTML_HELP_OPEN()              \
  HTML_HELP_DEF( "type", "double" )       \
  HTML_HELP_BODY()              \
  "A given pass stops when the modularity is increased by less than precision. Default value is <b>0.000001</b>"\
  HTML_HELP_CLOSE()
};
}
//========================================================================================
// same precision as the original code
#define DEFAULT_PRECISION 0.000001

LouvainClustering::LouvainClustering(const tlp::PluginContext* context): DoubleAlgorithm(context) {
  addInParameter<NumericProperty*>("metric", paramHelp[0], "",false);
  addInParameter<double>("precision", paramHelp[1], "0.000001",false);
}
//========================================================================================
bool LouvainClustering::run() {
  NumericProperty* metric = NULL;
  min_modularity = DEFAULT_PRECISION;

  if(dataSet!=NULL) {
    dataSet->get("metric", metric);
    dataSet->get("precision", min_modularity);
  }

  // initialize a random sequence according the given seed
  tlp::initRandomSequence();

  nb_nodes = graph->numberOfNodes();
  // init clusters
  clusters.resize(nb_nodes);
  quotient = new VectorGraph();

  for(unsigned int i = 0; i < nb_nodes; ++i) {
    clusters[i] = i;
    quotient->addNode();
  }

  weights = new EdgeProperty<double>();
  quotient->alloc(*weights);
  edge e;
  // init total_weight, weights and quotient edges
  forEach(e, graph->getEdges()) {
    double weight = metric ? metric->getEdgeDoubleValue(e) : 1;
    std::pair<node, node> ends = graph->ends(e);
    node q_src = node(clusters[ends.first.id]);
    node q_tgt = node(clusters[ends.second.id]);
    // self loops are counted only once
    total_weight += q_src != q_tgt ? 2 * weight : weight;
    // create corresponding edge if needed
    edge qe = quotient->existEdge(q_src, q_tgt, false);

    if (!qe.isValid()) {
      qe = quotient->addEdge(q_src, q_tgt);
      (*weights)[qe] = weight;
    }
    else
      // set current edge weight
      (*weights)[qe] += weight;
  }

  // init other vectors
  init_level();
  bool improvement = true;
  double mod = modularity(), new_mod;
  int level = 0;
  bool verbose = false;

  do {
    if (verbose) {
      std::cout << "level " << level << ':' << std::endl;
      std::cout << "  network size: "
                << size << " nodes, "
                << quotient->numberOfEdges() << " links, "
                << total_weight << " weight." << endl << std::flush;
    }

    improvement = one_level();
    new_mod = modularity();

    if (improvement) {
      ++level;

      VectorGraph* new_quotient = new VectorGraph();
      EdgeProperty<double>* new_weights = new EdgeProperty<double>();
      new_quotient->alloc(*new_weights);

      partitionToQuotient(new_quotient, new_weights);

      if (verbose)
        std::cout << " modularity increased from " << mod
                  << " to " << new_mod << endl << std::flush;

      mod=new_mod;
      init_level();
    }
    else {
      if (verbose)
        std::cout << " modularity increased from " << mod
                  << " to " << new_mod << endl << std::flush;

      // update measure
      // Renumber communities
      vector<int> renumber(size, -1);

      for (unsigned int n=0 ; n<size ; n++) {
        renumber[n2c[n]]++;
      }

      int final=0;

      for (unsigned int i=0 ; i<size ; i++)
        if (renumber[i]!=-1)
          renumber[i]=final++;

      // then set measure values
      unsigned int i = 0;
      node n;
      forEach(n, graph->getNodes()) {
        result->setNodeValue(n, renumber[n2c[clusters[i]]]);
        ++i;
      }
      delete quotient;
      delete weights;
    }
  }
  while(improvement);

  return true;
}
