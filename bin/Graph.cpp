#include "Graph.hh"
#include "ddd/Hom_Basic.hh"
#include <fstream>
#include <functional>
#include "ToTransRel.hh"
#include "ddd/util/dotExporter.h"

GraphBuilder::GraphBuilder(Label file, Label fileDD, bool doO, bool doDD, const labels_t & vars) : nextID(0),vars(vars),wgoDD(fileDD),doO(doO),doDD(doDD) {
	if (doO) {
		out = std::ofstream(file);
		out << "digraph G {" << std::endl;
	}
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
	if (doO) {
		out << "}" << std::endl;
		out.flush();
	}
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
		for (size_t i=0,e=s.size(); i < e ; i++) {
			auto v = s[i];
			if (v==0)
				continue ; // skip 0 values
			if (first) {
				first = false;
			} else  {
				out << ",";
			}
			out << vars[i] << "=" << v;
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
	using std::placeholders::_1;
	// step 1 : collect nodes
	if (gb->doO) {
		callback_t cb = std::bind(& GraphBuilder::addNode, gb, _1);
		iterate(toplot, &cb);
	}
	if (gb->doDD) {
		exportDot(toplot,gb->wgoDD+"states");
	}
	// nodes are loaded, build the transition relation
	for (auto & tr : next) {
		its::State pairs = toRelation (toplot, tr.second);
		if (gb->doO) {
			callback_t ea = std::bind(& handlePair, _1, gb, tr.first);
			iterate(pairs, &ea);
		}
		if (gb->doDD) {
			exportDot(toplot,gb->wgoDD+tr.first);
		}
	}

}
