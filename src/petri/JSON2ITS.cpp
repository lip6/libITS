#include "petri/JSON2ITS.hh"
#include "petri/PNet.hh"
#include "composite/Composite.hh"
#include "parser_RdPE/RdPE.h"
#include "parser_CAMI/Cami.hpp"
#include "parser_json/parse_json.hh"
#include "petri/Modular2ITS.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>


using namespace json;

// to store which variables (of the RdPE) belong to each hierarchy level
typedef std::set<int> places_t;
typedef places_t::const_iterator places_it;
// to store hierarchy to variable relation
typedef std::map<ItfElement *, places_t> varset_t;
typedef varset_t::iterator varset_it;
varset_t varset; 
// to store hierarchy to instance/type name in ITS model
typedef std::map<ItfElement *, vLabel> hiername_t;
typedef hiername_t::const_iterator hiername_it;
hiername_t hiername;

bool try_file_readable(const string &path) {
  std::ifstream file(path.c_str());
  if(file && !file.eof())
    return 1;
  cerr << "File is not readable, or content is empty. Cannot access :" << path << endl;
  cerr << "Error is fatal, sorry. Rerun with --help for options summary." << endl;
  exit(1);
}

int nextID = 0;
vLabel nextName() {
  return "PNet" + to_string(nextID++);
}


/** Test if a transition is local to a given set of places, as indicated through their index.
 *  Returns 0 if no hit on component, 1 if touches, 2 if pure local */
int isLocal ( const Transition & t, const places_t & places ) {
  bool local = false, global = false ;

  for(vector< pair<int,int> >::const_iterator jt= t.inhibitor.begin(); jt != t.inhibitor.end(); ++jt) {
    if ( places.find(jt->first) != places.end() ) {
      local = true;
    } else {
      global = true;
    }
  }
  for(vector< pair<int,int> >::const_iterator jt= t.pre.begin(); jt != t.pre.end(); ++jt) {
    if ( places.find(jt->first) != places.end() ) {
      local = true;
    } else {
      global = true;
    }
  }
  for(vector< pair<int,int> >::const_iterator jt= t.post.begin(); jt != t.post.end(); ++jt) {
    if ( places.find(jt->first) != places.end() ) {
      local = true;
    } else {
      global = true;
    }
  }
  if (global && !local) {
    // no hit
    return 0;
  } else if (local && !global) {
    // pure local
    return 2;
  } else {
    // mixed
    return 1;
  }
}


vLabel buildComposite (Hierarchie * hier, const RdPE & R , std::vector<Transition> & trans, its::ITSModel & model) {

  if ( (*(hier->begin()))->getType() == HIE ) {
    vLabel compname = nextName();
    hiername[hier] = compname;
    its::Composite comp(compname);

    places_t & places = varset[hier];
    
    for (Hierarchie::elts_it it = hier->begin() ; it != hier->end() ; ++it ) {      
      Hierarchie * phier = (Hierarchie *) (*it);
      vLabel inst = buildComposite (phier, R, trans,model);
      places_t & subp = varset[phier]; 
      // add all variables of sublevels to this hierarchy level
      places.insert ( subp.begin(), subp.end() );
      comp.addInstance ( inst,  inst , model);
      comp.updateStateDef ( "init", inst, "init");

      // Atomic propositions exported
      for (places_it subit = subp.begin() ; subit != subp.end() ; ++subit ) {
	vLabel pname = R.places[*subit].name ;
	comp.exposeVarIn (pname,inst);
// 	comp.addSynchronization (pname, pname);
// 	comp.addSyncPart (pname, inst, pname);
      }
      
    }



    for ( std::vector<Transition>::iterator it = trans.begin() ; it != trans.end() ; /** increment done in non erase case */ ) {
      Transition & curt = *it;
      Label tname = curt.name;
      int locality =  isLocal (*it, places);

      if ( locality == 2 ) {
	// pure local
	// no label
	comp.addSynchronization( tname, "");

      } else if ( locality == 1 ) {
	// touches the component
	comp.addSynchronization (tname, tname);
	// increment it
	++it;
      } else {
	// no hit : skip this transition
	++it;
	continue;
      }
      // to find which instances are impacted
      typedef std::set<vLabel> targets_t;
      typedef targets_t::const_iterator targets_it;
      targets_t targets;

      // for each target of the transition
      for(vector< pair<int,int> >::const_iterator jt=curt.inhibitor.begin(); jt != curt.inhibitor.end(); ++jt) {	
	// find the right instance
	for (Hierarchie::elts_it hit = hier->begin() ; hit != hier->end() ; ++hit ) {
	  places_t & subp = varset[*hit];
	  if ( subp.find(jt->first) != subp.end() ) {
	    targets.insert(hiername[*hit]);
	    // to next transition
	    break;
	  }
	}
      }
      for(vector< pair<int,int> >::const_iterator jt=curt.pre.begin(); jt != curt.pre.end(); ++jt) {
	// find the right instance
	for (Hierarchie::elts_it hit = hier->begin() ; hit != hier->end() ; ++hit ) {
	  places_t & subp = varset[*hit];
	  if ( subp.find(jt->first) != subp.end() ) {
	    targets.insert(hiername[*hit]);
	    // to next transition
	    break;
	  }
	}
      }
      for(vector< pair<int,int> >::const_iterator jt=curt.post.begin(); jt != curt.post.end(); ++jt) {
	// find the right instance
	for (Hierarchie::elts_it hit = hier->begin() ; hit != hier->end() ; ++hit ) {
	  places_t & subp = varset[*hit];
	  if ( subp.find(jt->first) != subp.end() ) {
	    targets.insert(hiername[*hit]);
	    // to next transition
	    break;
	  }
	}
      }

      // add sync parts
      for (targets_it tit = targets.begin() ; tit != targets.end() ; ++tit ) {
	comp.addSyncPart (tname, *tit, tname);
      }

      if (locality == 2)
	// remove from todo list
	it = trans.erase(it);

    }

    model.declareType(comp);

    return compname;
  } else {
    // A basic petri net module : list of places.
    vLabel netname = nextName();
    hiername [hier] = netname;
    its::PNet net (netname);

    places_t & places = varset[hier]; 

    
    for (Hierarchie::elts_it it = hier->begin() ; it != hier->end() ; ++it ) {      
      vLabel pname = ((PName *) (*it))->place;
      net.addPlace( pname );
      places.insert( R.getPlaceIndex(pname) );
      net.setMarking("init", pname, R.places[ R.getPlaceIndex(pname) ].marking);
    }
    
    for ( std::vector<Transition>::iterator it = trans.begin() ; it != trans.end() ; /** increment done in non erase case */ ) {
      int locality =  isLocal (*it, places);
      Transition & thetrans = *it;
      Label tname = thetrans.name;

      if ( locality == 2 ) {
	// pure local
	net.addTransition (tname, tname, its::PRIVATE);
      } else if ( locality == 1 ) {
	// touches the component
	net.addTransition (tname, tname, its::PUBLIC);
	// increment it
	++it;
      } else {
	// no hit : skip this transition
	++it;
	continue;
      }
      for(vector< pair<int,int> >::const_iterator jt=thetrans.inhibitor.begin(); jt != thetrans.inhibitor.end(); ++jt) {	
	if ( places.find(jt->first) != places.end() ) {
	  net.addArc (its::PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::INHIBITOR);
	}
      }
      for(vector< pair<int,int> >::const_iterator jt=thetrans.pre.begin(); jt != thetrans.pre.end(); ++jt) {
	if ( places.find(jt->first) != places.end() ) {
	  net.addArc (its::PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::INPUT);
	}
      }
      for(vector< pair<int,int> >::const_iterator jt=thetrans.post.begin(); jt != thetrans.post.end(); ++jt) {
	if ( places.find(jt->first) != places.end() ) {
	  net.addArc (its::PNet::ArcVal( R.places[jt->first].name, jt->second), tname, its::OUTPUT);
	}
      }
      if (locality == 2)
	// remove from todo list
	it = trans.erase(it);
      

    }
    //    std::cerr << "Built model : " ;
    //    net.print(std::cerr);
    //    std::cerr << std::endl;
    model.declareType(net);
    return netname;
  }

}

static void parseCami (RdPE & R, const std::string & pathcamiff) {

  try_file_readable(pathcamiff);
  cami::CAMI c(pathcamiff.c_str());
  if(c.valide_CAMI()) {
    R= cami::CAMI_RdPE(c);
  } else {
    cerr << "Parse error occurred while parsing Cami model file. Are you sure this net is Black&White ?"<<endl;
  }

}

vLabel JSONLoader::loadCami (its::ITSModel & model, const std::string & pathcamiff) {
  RdPE R;

  parseCami(R, pathcamiff);

  return RdPELoader::loadModularRdPE(model,R);
}

vLabel JSONLoader::loadJsonCami (its::ITSModel & model, const std::string & pathcamiff, const std::string & confff) {
  
  // Load CAMI model into an RdPE
  RdPE R;
  parseCami(R, pathcamiff);

  return loadJsonRdPE (model,  R, confff);
}

vLabel JSONLoader::loadJsonRdPE (its::ITSModel & model, class RdPE & R, const std::string & confff) {

  // Load order file
  try_file_readable(confff);
  Hierarchie hier;
  json_parse(confff, hier);

  // Todo list of transitions
  std::vector<Transition> trans = R.transitions;

  varset = varset_t();
  hiername = hiername_t();
  vLabel mainname = buildComposite (&hier, R , trans, model);

  varset = varset_t();
  hiername = hiername_t();

  return mainname;
}

