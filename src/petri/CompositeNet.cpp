#include "CompositeNet.hh"
#include "IPN.hh"

#include <set>

#include <iostream>

const Event* CompositeNet::getEvent(const string & event_name) const {
  vector<Synchronization>::const_iterator it = findName(event_name,syncs_) ;
  if (it != syncs_.end()) {
      return &(*it);
  }
  return NULL;
}

bool Synchronization::isEnabled (const State * s) {
  return true;
}

const State * Synchronization::fire (const State * s) {
  return s;
}

bool CompositeNet::openStateDef (const string & name) const {
  if (build_state_ != NULL) {
    std::cerr << "Reopening state definition before closing it.";
    return false;
  } else {
    build_state_ = new NamedCompositeNetState(name,subnets_.size());
    int ind = 0;
    for (vector<NetInstance>::const_iterator it = subnets_.begin() ;
	 it != subnets_.end();
	 ++it,++ind) {
      build_state_->setPart (ind, it->getType()->getDefaultState());
    }
    return true;
  }
}


bool CompositeNet::updateStateDef (const string & subnet, const string & substate) const {
  if (build_state_ == NULL) {
    std::cerr << "No state definition opened when trying to update state def." << std::endl;
    return false;
  } else {
    const NetInstance * p = NULL;
    int ind=0;
    for (vector<NetInstance>::const_iterator it = subnets_.begin() ;
	 it != subnets_.end();
	 ++it,++ind) {
      if (it->getName() == subnet) {
	p = &(*it);
	break;
      }
    }
    if (p == NULL) {
      std::cerr << "Unknown subnet named " << subnet << " when updating state " << build_state_->getName() << std::endl;
      return false;
    }
    const NamedState * subst = p->getType()->getState(substate);
    if (subst == NULL) {
      std::cerr << "Unknown subnet state named " << substate << " when updating state " << build_state_->getName() << std::endl;
      return false;
    }
    build_state_->setPart (ind,subst);
    return true;
  }
}

bool CompositeNet::closeStateDef () const {
  if (build_state_ == NULL) {
    std::cerr << "No state definition opened when trying to close definition." << std::endl;
    return false;
  }
  addState(build_state_->getName(),(NamedState *)build_state_->copy());
  delete build_state_;
  build_state_ = NULL;
  return true;
}



bool CompositeNet::addSynchronization (const std::string & sname, Visibility vis, bool isAND) {
  for (vector<Synchronization>::const_iterator it = syncs_.begin();
       it != syncs_.end();
       ++it ) {
    if (it->getName() == sname)
      return false;
  }
  syncs_.push_back(Synchronization(sname,vis,isAND));
  return true;
}

bool CompositeNet::addSyncPart (const std::string & sname, const std::string & subnetname, const std::string & tname) {
  Synchronization * sync = NULL;
  for (vector<Synchronization>::iterator it = syncs_.begin();
       it != syncs_.end();
       ++it ) {
    if (it->getName() == sname) {
      sync = &(*it);
      break;
    }
  }
  if (sync == NULL) {
    std::cerr << "Unknown synchronization " << sname << " when handling addSyncPart(" 
	      << sname << ","
	      << subnetname << ","
	      << tname << ")\n";
    return false;
  }
  const NetInstance * subnet = NULL;
  for (vector<NetInstance>::const_iterator it = subnets_.begin();
       it != subnets_.end() ;
       ++it) {
    if (it->getName() == subnetname ) {
      subnet = &(*it);
      break;
    }
  }
  if (subnet == NULL) {
    std::cerr << "Unknown subnet " << subnetname << " when handling addSyncPart(" 
	      << sname << ","
	      << subnetname << ","
	      << tname << ")\n";
    return false;
  }
  const Event * event = subnet->getType()->getEvent(tname);
  if (event == NULL) {
    std::cerr << "Unknown event " << tname << " when handling addSyncPart(" 
	      << sname << ","
	      << subnetname << ","
	      << tname << ")\n";
    return false;
  }
  
  sync->addSlave (subnet,event);
  return true;
}

bool CompositeNet::addInstance (const string & name, const string & type_name, const IPNModel & model) {

  const NetType * type = model.findType (type_name);
  if (type == NULL) {
    std::cerr << "Unknown net type " << type_name 
	      << " when handling subnet declaration : subnet " << type_name << " " << name 
	      << std::endl;
    return false;
  }
  
  for (vector<NetInstance>::const_iterator it = subnets_.begin();
       it != subnets_.end() ;
       ++it) {
    if (it->getName() == name ) {
      std::cerr << "Duplicate subnet instance name " << name 
		<< " when handling subnet declaration : subnet " << type_name << " " << name 
		<< std::endl;
      return false;
    }
  }
  
  subnets_.push_back(NetInstance(name,type)); 
  return true;
}

/** member print */
std::ostream & CompositeNet::print (std::ostream & os) const {
  os << "CompositeNet " << getName() << " {\n\n";

  os << "    // subnets\n" ;
  for (vector<NetInstance>::const_iterator it = subnets_.begin();
       it != subnets_.end() ;
       ++it) {
    os << "    subnet " << it->getType()->getName() << " " << it->getName() << " ;\n";
  }

  os << "    // states definition\n" ;
  if (states_.empty())
    getDefaultState();
  for (stmaptype::const_iterator it=states_.begin();
       it != states_.end();
       ++it) {
    os << "    state " << it->first << "{ ";
    const NamedCompositeNetState * vec = (const NamedCompositeNetState *) it->second;
    vector<NetInstance>::const_iterator pit = subnets_.begin();
    for (NamedCompositeNetState::const_iterator jt = vec->begin();
 	 jt != vec->end();
 	 ++jt,++pit) {
//      if ((*jt)->getName() != "default") 
           os << pit->getName() << " = " << (*jt)->getName() << "; ";
    }
    os << "}\n";
  }

  os << "    // synchronizations\n";
  for (vector<Synchronization>::const_iterator it = syncs_.begin();
       it != syncs_.end();
       ++it ) {
    it->print(os);
  }

  os << "}\n";
  return os;
}

/** pretty print */
std::ostream & operator<< (std::ostream & os, const CompositeNet & net) {
  return net.print(os);
}

std::ostream & Synchronization::print (std::ostream & os) const {
  os << "    "
     << (getVisibility() == PUBLIC ? "public " : "private ")
     << (isAND_?"AND":"OR")
     << " synchronization " << getName() << " {\n";
  for (vector<syncPart>::const_iterator it = slaves_.begin();
       it != slaves_.end();
       ++it ) {
    os << "      " << it->first->getName() << "." << it->second->getName() << " ;\n";
  }
  os << "    }\n";
  return os;
}

/************ SDD representations ***********/

SDD CompositeNet::buildSDDState (int var, const State *s) const {
  const NamedCompositeNetState * state = (const NamedCompositeNetState *) s;
  SDD m = SDD::one;
  int index = 0;
  vector<NetInstance>::const_iterator subit = subnets_.begin();
  for (NamedCompositeNetState::const_iterator it = state->begin();
       it != state->end() ;
       ++it, ++index, ++subit) {
    m = subit->getType()->getSDDState(index,*it) ^ m;
  }

  return SDD ( var, m);
}

GShom CompositeNet::getEvent (const Synchronization & sync) const {
  GShom hsync ;
  std::set<GShom> orSet;
  if (sync.isAND_)
    hsync = GShom::id ;
  for (vector<Synchronization::syncPart>::const_iterator partit = sync.slaves_.begin() ;
       partit != sync.slaves_.end() ;
       ++partit) {
    int instindex =  findNameIndex(partit->first->getName(),subnets_);
    Shom hpart = partit->first->getType()->getEvent(instindex,partit->second->getName());
    if (sync.isAND_)
      hsync = hpart & hsync;
    else
      orSet.insert(hpart);
  }
  if (! sync.isAND_)
    hsync = GShom::add(orSet);
  
  return hsync;
}

Shom CompositeNet::getEvent (int var, const std::string & evname) const {
  GShom hsync ;
  std::map<std::string,GShom>::const_iterator cacheit = hevents.find(evname);
  if (cacheit == hevents.end()) {
    vector<Synchronization>::const_iterator it = findName(evname,syncs_);
    if (it == syncs_.end() ) {
      std::cerr << "Event not found when handling CompositeNet::getEvent(var,evname). Houston, we have a problem...\n";
    } else {
      hsync = getEvent (*it);
    }
    // update cache
    hevents.insert(std::make_pair(evname,hsync));
  } else {
    // cache hit
    hsync = cacheit->second;
  }
  return localApply(hsync,var);
}


Shom CompositeNet::getPrivateEvents (int var) const {
  GShom locals = private_ev;
  if (private_ev == GShom::id) {
    // handle private events of subnets
    int index = 0;
    std::set<GShom> localSet;
    for (vector<NetInstance>::const_iterator subit = subnets_.begin() ;
	 subit != subnets_.end() ;
	 ++subit, ++index) {
      localSet.insert(subit->getType()->getPrivateEvents(index));
      // locals = subit->getType()->getPrivateEvents(index) + locals;
    }
    
    // handle private syncs of this level
    for (vector<Synchronization>::const_iterator it = syncs_.begin() ;
	 it != syncs_.end() ;
	 ++it ) {
      GShom hsync ;
      if (it->getVisibility() == PRIVATE) {
	hsync = getEvent(*it);
	// locals = hsync + locals;
	localSet.insert(hsync);
      }
    }
    locals = GShom::add(localSet);
    // update cache
    private_ev = locals;
  } 
  return localApply(locals,var);
}
