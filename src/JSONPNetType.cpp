#include <typeinfo>
#include <iostream>
#include "JsonPNetType.hh"
#include "CircularSetType.hh"
#include "Composite.hh"

#define trace while(0) std::cerr


namespace its {

  class JSONRepresentationBuilder {
    
    // to store which variables (of the PNet) belong to each hierarchy level
    typedef std::set<vLabel> places_t;
    typedef places_t::const_iterator places_it;
    // to store hierarchy to variable relation
    typedef std::map<const json::ItfElement *, places_t> varset_t;
    typedef varset_t::iterator varset_it;
    varset_t varset; 
    // to store hierarchy to instance/type name in ITS model
    typedef std::map<const json::ItfElement *, vLabel> hiername_t;
    typedef hiername_t::const_iterator hiername_it;
    hiername_t hiername;

    int nextID;
    vLabel nextName() {
      return "PNet" + to_string(nextID++);
    }


    /** Test if a transition is local to a given set of places, as indicated through their index.
     *  Returns 0 if no hit on component, 1 if touches, 2 if pure local */
    int isLocal ( const PTransition & t, const places_t & places ) {
      bool local = false, global = false ;
      
      for (PTransition::arcs_it arc = t.begin() ; arc != t.end() ; ++arc) {
	for (Arc::places_it pit = arc->begin() ; pit != arc->end() ; ++pit) {
// 	  std::cerr << "arc to place : " << pit->getPlace() << std::endl;
// 	  for (places_it ppit = places.begin(); ppit != places.end() ; ++ppit)
// 	    std::cerr << *ppit << " ," ;
// 	  std::cerr << std::endl;
	  if ( places.find(pit->getPlace()) != places.end() ) {
	    local = true;
	  } else {
	    global = true;
	  }
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

  public :
    vLabel buildComposite (const json::Hierarchie * hier, const PNet & net , std::vector<PTransition> & trans, its::ITSModel & model) {
      
      if ( (*(hier->begin()))->getType() == json::HIE ) {
	vLabel compname = nextName();
	hiername[hier] = compname;
	its::Composite comp(compname);
	
	places_t & places = varset[hier];
	
	for (json::Hierarchie::elts_it it = hier->begin() ; it != hier->end() ; ++it ) {      
	  json::Hierarchie * phier = (json::Hierarchie *) (*it);
	  vLabel inst = buildComposite (phier, net, trans, model);
	  places_t & subp = varset[phier]; 
	  // add all variables of sublevels to this hierarchy level
	  places.insert ( subp.begin(), subp.end() );
	  comp.addInstance ( inst,  inst , model);
	  comp.updateStateDef ( "init", inst, "init");
	  
// 	  for (places_it subit = subp.begin() ; subit != subp.end() ; ++subit ) {
// 	    vLabel pname = *subit ;
// 	    comp.addSynchronization (pname, pname);
// 	    comp.addSyncPart (pname, inst, pname);
// 	  }
	  
	}
	
	
	
	for ( std::vector<PTransition>::iterator it = trans.begin() ; it != trans.end() ; /** increment done in non erase case */ ) {
	  PTransition & curt = *it;
	  Label tname = curt.getName();
	  Label tlabel = curt.getLabel();
	  Visibility tvis = curt.getVisibility();
	  int locality =  isLocal (curt, places);

// 	  std::cerr << "locality =" << locality << std::endl;
	  
	  if ( locality == 2 ) {
	    // pure local
	    // copy the label if PUBLIC transition
	    if (tvis == PUBLIC) {
	      comp.addSynchronization( tname, tlabel);
	      // keep the transition alive, it needs to be further exported
	      ++it;
	    } else {
	      comp.addSynchronization( tname, "");
	      // remove from todo list
	      it = trans.erase(it);
	    }
	    
	  } else if ( locality == 1 ) {
	    // touches the component, not yet fully resolved
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
	  for (PTransition::arcs_it arc = curt.begin() ; arc != curt.end() ; ++arc) {
	    for (Arc::places_it pit = arc->begin() ; pit != arc->end() ; ++pit) {

	      // find the right instance
	      for (json::Hierarchie::elts_it hit = hier->begin() ; hit != hier->end() ; ++hit ) {
		places_t & subp = varset[*hit];
		if ( subp.find(pit->getPlace()) != subp.end() ) {
		  targets.insert(hiername[*hit]);
		  // to next transition arc
		  break;
		}
	      }
	    }
	  }
	  if (targets.size() == 1) {
	    // was pure local to a subcomponent : use the label
	    comp.addSyncPart (tname, *targets.begin(), tlabel);
	  } else {
	    // add sync parts
	    for (targets_it tit = targets.begin() ; tit != targets.end() ; ++tit ) {
	      comp.addSyncPart (tname, *tit, tname);
	    }
	  }
	  

	}
	
	model.declareType(comp);
	
	return compname;
      } else {
	// A basic petri net module : list of places.
	vLabel netname = nextName();
	hiername [hier] = netname;
	its::PNet newnet (netname);
	
	places_t & places = varset[hier]; 
	
	
	for (json::Hierarchie::elts_it it = hier->begin() ; it != hier->end() ; ++it ) {      
	  vLabel pname = ((json::PName *) (*it))->place;
	  newnet.addPlace( pname );
	  places.insert( pname );
	  for (PNet::markings_it mark = net.markings_begin() ; mark != net.markings_end() ; ++mark) {
	    newnet.setMarking(mark->first, pname, mark->second.getMarking(pname));
	  }
	}

	
	for ( std::vector<PTransition>::iterator it = trans.begin() ; it != trans.end() ; /** increment done in non erase case */ ) {
	  int locality =  isLocal (*it, places);
	  PTransition & curt = *it;
	  Label tname = curt.getName();
	  Label tlabel = curt.getLabel();
	  Visibility tvis = curt.getVisibility();

	  if ( locality == 2 ) {
	    // pure local
	    newnet.addTransition (tname, tlabel, tvis);

	    if (tvis == its::PRIVATE) {
	      // remove from todo list
	      it = trans.erase(it);
	    } else {
	      // keep it in todo list for export from higher level components
	      ++it;
	    }
	  } else if ( locality == 1 ) {
	    // touches the component, not yet fully resolved
	    newnet.addTransition (tname, tname, its::PUBLIC);
	    // increment it
	    ++it;
	  } else {
	    // no hit : skip this transition
	    ++it;
	    continue;
	  }

	  // for each target of the transition
	  for (PTransition::arcs_it arc = curt.begin() ; arc != curt.end() ; ++arc) {
	    for (Arc::places_it pit = arc->begin() ; pit != arc->end() ; ++pit) {
	      if ( places.find(pit->getPlace()) != places.end() ) {
		newnet.addArc (its::PNet::ArcVal(pit->getPlace(),pit->getValuation()),  tname, arc->getType());
	      }
	    }
	  }
	  

	}
	//    std::cerr << "Built model : " ;
	//    net.print(std::cerr);
	//    std::cerr << std::endl;
	model.declareType(newnet);
	return netname;
      }
    }

    JSONRepresentationBuilder ():nextID(0){};
    
    

  };

  pType JsonPNetType::getConcrete () const {
    if (concrete_ == NULL ) {
      JSONRepresentationBuilder jrb;
       // Todo list of transitions
      std::vector<PTransition> trans ;
      for (PNet::trans_it tit= net_.transitions_begin() ; tit != net_.transitions_end() ; ++tit)
	trans.push_back(*tit);

      //      std::cerr << "trans size : " << trans.size() << std::endl;

      vLabel name = jrb.buildComposite(hier_,net_,trans,model_);
      concrete_ =  model_.findType(name);

      std::cerr << model_ << std::endl;
    }
    return concrete_;
  }


} // namespace

