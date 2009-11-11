#ifndef __COMPOSITE_NET_HH__
#define __COMPOSITE_NET_HH__

#include <iosfwd>
#include <vector>
using std::vector;
#include <string>
using std::string;

#include "NetType.hh"

// predeclaration
class IPNModel;

class Synchronization : public Event {
  typedef std::pair<const NetInstance *,const Event *> syncPart;
  vector<syncPart> slaves_;
  // true => AND composition semantics, false => OR composition semantics
  bool isAND_ ;
  friend class CompositeNet;
 public :
  Synchronization (const string & name, Visibility vis, bool isAND=true) : Event(name,vis),isAND_(isAND) {};
  virtual ~Synchronization () {};
  
  bool isEnabled (const State * s) ;
  const State * fire (const State * s) ;

  void addSlave (const NetInstance * subnet, const Event * slave) { slaves_.push_back(std::make_pair(subnet,slave)); }
  std::ostream & print (std::ostream & os) const;
};

class NamedCompositeNetState : public NamedState {
  typedef const NamedState * statePart;
  std::vector<statePart> state_;
public :
  typedef std::vector<statePart>::const_iterator const_iterator;
  const_iterator begin() const { return state_.begin(); }
  const_iterator end() const { return state_.end(); }

  NamedCompositeNetState (const std::string & name, int size) : NamedState(name),state_(size,(const NamedState *)NULL) {};
  virtual ~NamedCompositeNetState () {};

  State * copy () const { return new NamedCompositeNetState(*this) ; }
  void setPart (int index, const NamedState * st) { state_[index] = st;}
  const State * getPart (int index) { return state_[index];}
};


class CompositeNet : public NetType {
  vector<NetInstance> subnets_;
  vector<Synchronization> syncs_;
  mutable NamedCompositeNetState * build_state_;
  // caches
  mutable GShom private_ev;
  mutable std::map<std::string,GShom> hevents;

  GShom getEvent (const Synchronization & ) const;
public :
  CompositeNet (const string & name) : NetType(name),build_state_(NULL) {};

  const Event* getEvent(const string & event_name) const ;
  bool addInstance (const std::string & name, const std::string & type_name, const IPNModel & model);

  bool addSynchronization (const std::string & sname, Visibility vis, bool isAND=true);
  bool addSyncPart (const std::string & sname, const std::string & subnetname, const std::string & tname);

  bool openStateDef (const string & name) const ;
  bool updateStateDef (const string & subnet, const string & substate) const ;
  bool closeStateDef () const ;

  SDD buildSDDState (int var, const State *s) const ;
  Shom getEvent (int var, const std::string & evname) const ;
  Shom getPrivateEvents (int var) const ;

  std::ostream & print (std::ostream & os) const ;
};

std::ostream & operator<< (std::ostream & os, const CompositeNet & net);


#endif
