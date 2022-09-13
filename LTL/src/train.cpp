#include "train.hh"
#include "TPNet.hh"
#include "TComposite.hh"
#include <cmath>

using namespace its;

namespace trains {

void loadTrainType (ITSModel & model) {
  TPNet net ("Train0");
  
  net.addPlace("far");
  net.addPlace("close");
  net.addPlace("on");
  net.addPlace("left");

  vLabel tname = "App";
  net.addTransition(tname, PUBLIC);
  // input arc
  net.addArc(PNet::ArcVal("far",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("close",1), tname, OUTPUT);

  tname = "In";
  net.addTransition(tname, PRIVATE);
  // input arc
  net.addArc(PNet::ArcVal("close",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("on",1), tname, OUTPUT);
  // clock
  net.addClock(tname,3,5);


  tname = "Ex";
  net.addTransition(tname, PRIVATE);
  // input arc
  net.addArc(PNet::ArcVal("on",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("left",1), tname, OUTPUT);
  // clock
  net.addClock(tname,2,4);

  tname = "Exit";
  net.addTransition(tname, PUBLIC);
  // input arc
  net.addArc(PNet::ArcVal("left",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("far",1), tname, OUTPUT);

  net.setMarking("init","far",1);
  
  // Add an atomic property tester
  tname = "InSection";
  net.addTransition(tname, PUBLIC);
  net.addArc(PNet::ArcVal("on",1), tname, TEST);

  tname = "FarAway";
  net.addTransition(tname, PUBLIC);
  net.addArc(PNet::ArcVal("far",1), tname, TEST);
  
  

  model.declareType(net);
}
// the i-th instance of a component is r(i-1)
std::string instanceName (int n) {
  return "t" + to_string(n);
}

/** a type to represent a group of trains homogeneously to a single train */
void loadTrainGroup (int n, ITSModel & model) {
  // ensure train type is declared
  loadTrainType (model);

  TComposite net("TrainGroup");
  
  for (int i=0; i < n ; ++i) {
    net.addInstance(instanceName(i),"Train0",model);
  }
  
  /** the sync App */
  vLabel sname = "App";
  // isAND = false !!
  net.addSynchronization(sname,PUBLIC,false);
  for (int i=0; i < n ; ++i) {
    net.addSyncPart(sname,instanceName(i),sname);
  }

  /** the sync Exit */
  sname = "Exit";
  // isAND = false !!
  net.addSynchronization(sname,PUBLIC,false);
  for (int i=0; i < n ; ++i) {
    net.addSyncPart(sname,instanceName(i),sname);
  }

  for (int i=0; i < n ; ++i) {
    net.updateStateDef("init",instanceName(i),"init");
  }


  // Export Atomic property
  sname = "InSection";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,instanceName(0),sname);

  sname = "FarAway";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,instanceName(0),sname);
  
  model.declareType(net);
}


/** a type to represent a group of trains homogeneously to a single train */
void loadTrainGroupRec (int cur, ITSModel & model) {
  // ensure train type is declared
  if (cur == 1) {
    loadTrainType (model);
  } else {
    loadTrainGroupRec (cur-1,model);
  }
  int n=2;
  TComposite net("Train"+to_string(cur));
  
  for (int i=0; i < n ; ++i) {
    net.addInstance(instanceName(i),"Train"+to_string(cur-1),model);
  }
  
  /** the sync App */
  vLabel sname = "App";
  // isAND = false !!
  net.addSynchronization(sname,PUBLIC,false);
  for (int i=0; i < n ; ++i) {
    net.addSyncPart(sname,instanceName(i),sname);
  }

  /** the sync Exit */
  sname = "Exit";
  // isAND = false !!
  net.addSynchronization(sname,PUBLIC,false);
  for (int i=0; i < n ; ++i) {
    net.addSyncPart(sname,instanceName(i),sname);
  }

  for (int i=0; i < n ; ++i) {
    net.updateStateDef("init",instanceName(i),"init");
  }


  // Export Atomic property
  sname = "InSection";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,instanceName(0),sname);

  sname = "FarAway";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,instanceName(0),sname);


  model.declareType(net);
}



void loadGateType (ITSModel & model) {
  TPNet net ("Gate");
  
  net.addPlace("open");
  net.addPlace("coming");
  net.addPlace("lowering");
  net.addPlace("closed");
  net.addPlace("leaving");
  net.addPlace("raising");

  /*
  vLabel tname = "Down";
  net.addTransition(tname, PRIVATE);
  // hyper input arc
  PNet::ArcValSet as;
  as.push_back(PNet::ArcVal("open",1));
  as.push_back(PNet::ArcVal("raising",1));
  net.addHArc(as, tname, INPUT);
  // input arc
  net.addArc(PNet::ArcVal("coming",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("lowering",1), tname, OUTPUT);
  // clock
  net.addClock(tname,0,0);
  */
    vLabel tname = "Down1";
  net.addTransition(tname, PRIVATE);
  // hyper input arc
  net.addArc(PNet::ArcVal("open",1), tname, INPUT);
  // input arc
  net.addArc(PNet::ArcVal("coming",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("lowering",1), tname, OUTPUT);
  // clock
  net.addClock(tname,0,0);
  
  tname = "Down2";
  net.addTransition(tname, PRIVATE);
  // hyper input arc
  net.addArc(PNet::ArcVal("raising",1), tname, INPUT);
  // input arc
  net.addArc(PNet::ArcVal("coming",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("lowering",1), tname, OUTPUT);
  // clock
  net.addClock(tname,0,0);
  
  
  
  tname = "L";
  net.addTransition(tname, PRIVATE);
  // input arc
  net.addArc(PNet::ArcVal("lowering",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("closed",1), tname, OUTPUT);
  // clock
  net.addClock(tname,1,2);

  tname = "up";
  net.addTransition(tname, PRIVATE);
  // input arc
  net.addArc(PNet::ArcVal("leaving",1), tname, INPUT);
  net.addArc(PNet::ArcVal("closed",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("raising",1), tname, OUTPUT);
  // clock
  net.addClock(tname,0,0);

  tname = "R";
  net.addTransition(tname, PRIVATE);
  // input arc
  net.addArc(PNet::ArcVal("raising",1), tname, INPUT);
  // output arc
  net.addArc(PNet::ArcVal("open",1), tname, OUTPUT);
  net.addClock(tname,1,2);

  // detect an incoming train
  tname = "App";
  net.addTransition(tname, PUBLIC);
  // output arc
  net.addArc(PNet::ArcVal("coming",1), tname, OUTPUT);

  // detect an outgoing train
  tname = "Exit";
  net.addTransition(tname, PUBLIC);
  // output arc
  net.addArc(PNet::ArcVal("leaving",1), tname, OUTPUT);

  net.setMarking("init","open",1);

  // Add an atomic property tester
  tname = "IsOpen";
  net.addTransition(tname, PUBLIC);
  net.addArc(PNet::ArcVal("open",1), tname, TEST);

  model.declareType(net);
}

/** a controller type for n trains */
void loadControllerType (ITSModel & model, int n) {
  TPNet net ("Controller");
  
  net.addPlace("far");
  net.addPlace("in");

  // first train to go in
  vLabel tname = "Enter_first";
  net.addTransition(tname, PUBLIC);
  // input arc : everyone is "far"
  net.addArc(PNet::ArcVal("far",n), tname, INPUT);
  // output arcs
  // one is now "in"
  net.addArc(PNet::ArcVal("in",1), tname, OUTPUT);
  // others are still far
  net.addArc(PNet::ArcVal("far",n-1), tname, OUTPUT);

  // a normal train entry
  tname = "Enter";
  net.addTransition(tname, PUBLIC);
  // input arc : at least someone already in
  net.addArc(PNet::ArcVal("in",1), tname, INPUT);
  // the token representing this train
  net.addArc(PNet::ArcVal("far",1), tname, INPUT);
  // output arc
  // put two tokens in "in" (the one tested by pre + the one taken from far)
  net.addArc(PNet::ArcVal("in",2), tname, OUTPUT);

  // Last train to exit
  tname = "Exit_last";
  net.addTransition(tname, PUBLIC);
  // the token representing this train
  net.addArc(PNet::ArcVal("in",1), tname, INPUT);
  // input arc : everyone else is already "far"
  net.addArc(PNet::ArcVal("far",n-1), tname, INPUT);
  // output arc : everyone is now far
  net.addArc(PNet::ArcVal("far",n), tname, OUTPUT);
  
  // Normal train exit
  tname = "Exit";
  net.addTransition(tname, PUBLIC);
  // input arc : at least two trains are currently "in"
  net.addArc(PNet::ArcVal("in",2), tname, INPUT);
  // output arcs : return one to "in" move one to "far"
  net.addArc(PNet::ArcVal("in",1), tname, OUTPUT);
  net.addArc(PNet::ArcVal("far",1), tname, OUTPUT);

  net.setMarking("init","far",n);

  model.declareType(net);
}

void loadControlledGateType (ITSModel & model, int n) {
  // ensure gate and controller declarations are done.
  loadGateType(model);
  loadControllerType (model,n);
  
  TComposite net("ControlledGate");

  // contains a gate instance
  net.addInstance("gate","Gate",model);
  // and a controller instance
  net.addInstance("control","Controller",model);
  
  // Two public synchronizations are declared : App and Exit.

  // App is Used to indicate approach of a train
  // App = (control.Enter_first & gate.App) | control.Enter 

  // App_first = control.Enter_first & gate.App
  vLabel sname = "App_first";
  net.addSynchronization(sname,PUBLIC);
  net.addSyncPart(sname,"control","Enter_first");
  net.addSyncPart(sname,"gate","App");
  
  // the real thing : App = App_first  | control.Enter 
  sname = "App";
  // isAND = false !!
  net.addSynchronization(sname,PUBLIC,false);
  // empty instance name denotes self reference
  net.addSyncPart(sname,"","App_first");
  net.addSyncPart(sname,"control","Enter");

  // Exit is used to indicate a train leaving
  // Exit = (control.Exit_last & gate.Exit) | control.Exit 

  // Exit_last = control.Exit_last & gate.Exit
  sname = "Exit_last";
  net.addSynchronization(sname,PUBLIC);
  net.addSyncPart(sname,"control","Exit_last");
  net.addSyncPart(sname,"gate","Exit");

  // Exit = Exit_last | control.Exit 
  sname = "Exit";
  // isAND = false !!
  net.addSynchronization(sname,PUBLIC,false);
  // empty instance name denotes self reference
  net.addSyncPart(sname,"","Exit_last");
  net.addSyncPart(sname,"control","Exit");
  
  net.updateStateDef("init","control","init");
  net.updateStateDef("init","gate","init");


  // Export Atomic property
  sname = "IsOpen";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,"gate",sname);

  model.declareType(net);  
}

} // namespace
using namespace trains;

void loadTrains (int n, ITSModel & model, bool isRec) {

  TComposite net("Trains");
  // and a trains group
  if (! isRec) {
    loadTrainGroup(n,model);
    // ensure existence of nested types
    loadControlledGateType(model,n);
    net.addInstance("trains","TrainGroup",model);
  } else {
    loadTrainGroupRec (n,model);
    // ensure existence of nested types
    loadControlledGateType(model,pow(2,n));
    net.addInstance("trains","Train"+to_string(n),model);
  }
  
  // contains a controlled gate instance
  net.addInstance("gate","ControlledGate",model);

  // Exit  
  vLabel sname = "Exit";
  net.addSynchronization(sname,PRIVATE);
  net.addSyncPart(sname,"trains",sname);
  net.addSyncPart(sname,"gate",sname);
  
  // App
  sname = "App";
  net.addSynchronization(sname,PRIVATE);
  net.addSyncPart(sname,"trains",sname);
  net.addSyncPart(sname,"gate",sname);    

  net.updateStateDef("init","trains","init");
  net.updateStateDef("init","gate","init");

  // Export Atomic property
  sname = "IsOpen";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,"gate",sname);


  // Export Atomic property
  sname = "InSection";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,"trains",sname);

  sname = "FarAway";
  net.addSynchronization(sname, PUBLIC);
  net.addSyncPart(sname,"trains",sname);


  model.declareType(net);
}
