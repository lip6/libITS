#include "Graph.hh"
#include "ddd/Hom_Basic.hh"
#include <fstream>
#include <functional>
#include "ToTransRel.hh"

GraphBuilder::GraphBuilder(Label file) : out(file),nextID(0) {
	out << "digraph G {" << std::endl;
}

void GraphBuilder::addEdge (const state_t & src, const state_t & dest, Label label) {
	map_t::const_accessor caccess;
	if (!  index.find(caccess,src) ) {
		return;
	}
	map_t::const_accessor caccessd;
	if (!  index.find(caccessd,dest) ) {
		return;
	}
	int sid = caccess->second;
	int did = caccessd->second;

	out << "n" << sid << " -> " << "n" << did << " [label=\"" << label << "\"] ;" << std::endl;
}

GraphBuilder::~GraphBuilder() {
	out << "}" << std::endl;
	out.flush();
}

void GraphBuilder::addNode (state_t & s) {
    map_t::const_accessor caccess;
    if (!  index.find(caccess,s) ) {
		// miss
		map_t::accessor access ;
		index.insert(access,s);
		int id = nextID++;
		access->second = id;
		out << "n" << id << " [label=\"" ;
		bool first = true;
		for (auto & i : s) {
			if (first) {
				first = false;
			} else  {
				out << ",";
			}
			out <<  i;
		}
		out << "\"] ;" << std::endl;
    }
}

void handlePair (state_t & s, GraphBuilder * gb, Label label) {
	state_t src, dest;
	src.reserve(s.size()/2);
	dest.reserve(s.size()/2);
	for (size_t i=0; i < s.size() ; i++) {
		if (i%2 == 0) {
			src.push_back(s[i]);
		} else {
			dest.push_back(s[i]);
		}
	}
	gb->addEdge(src, dest, label);
};

void plotGraph (its::State toplot, its::Type::namedTrs_t & next, GraphBuilder * gb) {


	// step 1 : collect nodes
	using std::placeholders::_1;
	callback_t cb = std::bind(& GraphBuilder::addNode, gb, _1);
	iterate(toplot, &cb);
	// nodes are loaded, build the transition relation
	for (auto & tr : next) {
		its::State pairs = toRelation (toplot, tr.second);
		callback_t ea = std::bind(& handlePair, _1, gb, tr.first);
		iterate(pairs, &ea);
	}

}
