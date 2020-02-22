#ifndef __GRAPH__HH__
#define __GRAPH__HH__

#include "ddd/DDD.h"
#include "ddd/util/ext_hash_map.hh"
#include "its/ITSModel.hh"
#include "ddd/Hom_Basic.hh"
#include <fstream>

class GraphBuilder  {
	std::ofstream out;
	typedef ext_hash_map<state_t,int> map_t;
	map_t index;
	int nextID;
	labels_t vars;
	vLabel wgoDD;
	bool doO;
	bool doDD;
	friend void plotGraph (its::State toplot, its::Type::namedTrs_t & next, GraphBuilder * gb);
public :
	GraphBuilder(Label file, Label fileDD, bool doO, bool doDD, const labels_t & vars);
	void addEdge (const state_t & src, const state_t & dest, Label label);
	void addNode (state_t & s) ;
	~GraphBuilder() ;
};

void plotGraph (its::State toplot, its::Type::namedTrs_t & next, GraphBuilder * gb);

#endif
