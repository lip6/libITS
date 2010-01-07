#include "Modular2ITS.hh"
#include "PNet.hh"
#include "Composite.hh"
#include "parser_RdPE/RdPE.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>


using std::string;
using std::endl;
using std::cerr;
using std::vector;
using std::set;

using its::PNet;

bool try_file_readable2(const string &path) {
  std::ifstream file(path.c_str());
  if(file && !file.eof())
    return 1;
  cerr << "File is not readable, or content is empty. Cannot access :" << path << endl;
  cerr << "Error is fatal, sorry. Rerun with --help for options summary." << endl;
  exit(1);
}

vLabel modName (int i) {
  return "mod" + to_string(i);
}

vLabel instName (int i) {
  return "inst" + to_string(i);
}

static int isLocal (const RdPE &R, vector<Transition>::const_iterator t) {
  int mod = -1 ;
  for(vector< pair<int,int> >::const_iterator it=t->inhibitor.begin();it!=t->inhibitor.end(); ++it) {
    if (mod == -1) {
      mod = R.getmodule(it->first);
    } else if (mod != R.getmodule(it->first)) {
      return -1;
    }
  }
  for(vector< pair<int,int> >::const_iterator it=t->pre.begin();it!=t->pre.end(); ++it) {
    if (mod == -1) {
      mod = R.getmodule(it->first);
    } else if (mod != R.getmodule(it->first)) {
      return -1;
    }
  }
  for(vector< pair<int,int> >::const_iterator it=t->post.begin();it!=t->post.end(); ++it) {
    if (mod == -1) {
      mod = R.getmodule(it->first);
    } else if (mod != R.getmodule(it->first)) {
      return -1;
    }
  }

  return mod;
}


vLabel RdPELoader::loadModularProd (its::ITSModel & model, const std::string & pathnetff) {  
  try_file_readable2(pathnetff);
  RdPE R = RdPE (pathnetff.c_str());
  vLabel modelName = pathnetff;

  // build a PNet for each module
  typedef std::map<int, PNet > mod_t;
  typedef mod_t::const_iterator mod_it; 
  typedef mod_t::iterator mod_vit; 
  mod_t modules;

  for (map<int, vector<int> >::const_iterator it = R.modules.begin() ; it != R.modules.end() ; ++it ) {
    std::pair< mod_vit, bool > iret = modules.insert(std::make_pair(it->first ,PNet(modName(it->first))));
    PNet & net = iret.first->second ; 
    for (vector<int>::const_iterator jt = it->second.begin() ;  jt != it->second.end() ; ++jt ) {
      if (! net.addPlace (R.places[*jt].name) ){
	std::cerr << "duplicate place name \"" << R.places[*jt].name << "\" continuing parse" << std::endl;
      }
      net.setMarking("init", R.places[*jt].name, R.places[*jt].marking);
    }
  }

  // to hold synchronizations
  labels_t syncs;
  // map syncs to modules they hit
  std::map<vLabel, std::set<int> > syncTargets;

  for (vector<class Transition>::const_iterator it = R.transitions.begin() ; it != R.transitions.end() ; ++it ) {
    int mod = isLocal(R,it);
    if ( mod != -1 ) {
      // is Local
      mod_vit netit = modules.find(mod);
      PNet * net = & netit->second ;
      vLabel tname = it->name;
      net->addTransition (tname, tname, its::PRIVATE);
      for(vector< pair<int,int> >::const_iterator jt=it->inhibitor.begin(); jt != it->inhibitor.end(); ++jt) {
	net->addArc (PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::INHIBITOR);
      }
      for(vector< pair<int,int> >::const_iterator jt=it->pre.begin(); jt != it->pre.end(); ++jt) {
	net->addArc (PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::INPUT);
      }
      for(vector< pair<int,int> >::const_iterator jt=it->post.begin(); jt != it->post.end(); ++jt) {
	net->addArc (PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::OUTPUT);
      }
      if ( ! it->preAuto.empty() || ! it->postAuto.empty() ) {
	std::cerr << " Your Pnet contains self modifying behavior not handled in this version of the tool. Ignoring self-modifying behavior, this is a real error. You could try pnddd tool rather than this ITS version.";
      }	
    } else {
      // not a local behavior
      vLabel tname = it->name;

      syncs.push_back(tname);
      for(vector< pair<int,int> >::const_iterator jt=it->inhibitor.begin(); jt != it->inhibitor.end(); ++jt) {
	int targetmodule = R.getmodule(jt->first) ;
	syncTargets[tname].insert(targetmodule);
	mod_vit netit = modules.find( targetmodule );
	PNet & net = netit->second ;
	// NOTE THIS OP DOES NOTHING IF TRANSITION EXISTS
	net.addTransition (tname, tname, its::PUBLIC);
	net.addArc (PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::INHIBITOR);
      }
      for(vector< pair<int,int> >::const_iterator jt=it->pre.begin(); jt != it->pre.end(); ++jt) {
	int targetmodule = R.getmodule(jt->first) ;
	syncTargets[tname].insert(targetmodule);
	mod_vit netit = modules.find( targetmodule );
	PNet & net = netit->second ;
	// NOTE THIS OP DOES NOTHING IF TRANSITION EXISTS
	net.addTransition (tname, tname, its::PUBLIC);
	net.addArc (PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::INPUT);
      }
      for(vector< pair<int,int> >::const_iterator jt=it->post.begin(); jt != it->post.end(); ++jt) {
	int targetmodule = R.getmodule(jt->first) ;
	syncTargets[tname].insert(targetmodule);
	mod_vit netit = modules.find( targetmodule );
	PNet * net = & netit->second ;
	// NOTE THIS OP DOES NOTHING IF TRANSITION EXISTS
	net->addTransition (tname, tname, its::PUBLIC);
	net->addArc (PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::OUTPUT);
      }
      if ( ! it->preAuto.empty() || ! it->postAuto.empty() ) {
	std::cerr << " Your Pnet contains self modifying behavior on transition "  << tname << " not handled in this version of the tool. Ignoring self-modifying behavior, this is a real error. You could try pnddd tool rather than this ITS version.";
      }	

    }
  }

  // now declare types 
  for ( mod_it it = modules.begin() ; it != modules.end() ; ++it ){
    model.declareType ( it->second );
  }
  // build a composite
  vLabel compositeName= "NetMain";
  its::Composite c = its::Composite(compositeName);
  
  for (mod_t::const_reverse_iterator it = modules.rbegin() ; it != ((const mod_t& )modules).rend() ; ++it ) {
    c.addInstance ( instName(it->first),  it->second.getName(), model); 
  }
  for (mod_it it = modules.begin() ; it != modules.end() ; ++it ) {
    c.updateStateDef ( "init", instName(it->first),"init");
  }

  // add syncs
  for (labels_it it = syncs.begin() ; it != syncs.end() ; ++it ) {
    Label tname = *it;
    c.addSynchronization ( tname, "");
    std::set<int> & tset = syncTargets [tname];
    for (std::set<int>::const_iterator jt = tset.begin() ; jt != tset.end() ; ++jt ) {
      c.addSyncPart (tname, instName(*jt) , tname);
    }
  }

  // add atomic properties
  for ( mod_it it = modules.begin() ; it != modules.end() ; ++it ){
    vLabel iname = instName(it->first);
    for (PNet::places_it pit = it->second.places_begin() ; pit != it->second.places_end() ; ++pit ) {
      vLabel tname = pit->getName() ;
      c.addSynchronization ( tname, tname );
      c.addSyncPart (tname, iname , tname);
    }
  }

  model.declareType(c);

  return compositeName;
}

