#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

#include "util/dotExporter.h"
#include "IPN.hh"
#include "statistic.hpp"

// int -> string
std::string toString (int i) {
  char buff [16];
  sprintf (buff,"%d",i);
  return buff;
}
// float -> string
std::string toString (double i) {
  char buff [16];
  sprintf (buff,"%lf",i);
  return buff;
}
// name for types : Robin3 represents 3 elements
std::string typeName (const string & name,int n) {
  return name + toString(n);
}
// the i-th instance of a component is r(i-1)
std::string instanceName (int n) {
  return "r" + toString(n);
}
// client instances
std::string clientName (int n) {
  return "cli" + toString(n);
}
// server instances
std::string serverName (int n) {
  return "serv" + toString(n);
}
// RPC instances
std::string RPCName (int c, int s) {
  return "RPC(" + toString(c) + "," + toString(s) + ")";
}


std::string robinName (int n) {
  return "cli" + toString(n);
}

void buildCellType (IPNModel & m, int n=1) {
 UnitNet * res = new UnitNet("Cell") ;
 
 res->addPlace ("Mem");

 res->openStateDef("default");
 res->updateStateDef("Mem",n);
 res->closeStateDef();

 string tname;
 tname = "get";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Mem",tname,1,INPUT);

 tname = "put";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Mem",tname,1,OUTPUT);

 m.addType(res);
}


void buildMultiCellAnyType (IPNModel & m, int n) {
  CompositeNet * net = new CompositeNet(typeName("MultiCellAny",n));
  
  for (int i=0 ; i<n ; ++i){
    net->addInstance(typeName("cell",i),"Cell",m);
  }

  string inst = "cell";
  string tname;  
  tname = "put";
  // OR SYNCRO
  net->addSynchronization(tname,PUBLIC,false);
  for (int i=0; i < n ; i++) {
    net->addSyncPart(tname,typeName(inst,i),tname);
  }

  // n synchros
  string tpart;
  for (int i=0; i < n ; i++) {
    tpart = "get";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,typeName(inst,i),tpart);
  }

  m.addType(net);
}

void buildMultiCellType (IPNModel & m, int n) {
  CompositeNet * net = new CompositeNet(typeName("MultiCell",n));
  
  for (int i=0 ; i<n ; ++i){
    net->addInstance(typeName("cell",i),"Cell",m);
  }

  string inst = "cell";
  string tname;  

  // n synchros
  string tpart;
  for (int i=0; i < n ; i++) {
    tpart = "get";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,typeName(inst,i),tpart);


    tpart = "put";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,typeName(inst,i),tpart);
  }

  m.addType(net);
}


void buildClientSenderType (IPNModel & m, int ns) {
  CompositeNet * net = new CompositeNet("ClientSender") ;
 
  net->addInstance("cli","Client",m) ;  
  net->addInstance ("req",typeName("MultiCellAny",ns),m);
  net->addInstance("ack","Cell",m);

  // private syncs
  string tname;
  tname = "sendReq";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"req","put");
  net->addSyncPart(tname,"cli",tname);
  
  tname = "getReply";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"ack","get");
  net->addSyncPart(tname,"cli",tname);
  

  // PUBLIC INTERFACE
  // n synchros
  string tpart;
  for (int i=0; i < ns ; i++) {
    tpart = "call";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,"req",typeName("get",i));
  }

  // reply synchro
  tname = "return";
  net->addSynchronization(tname,PUBLIC);
  net->addSyncPart(tname,"ack","put");
  
  m.addType(net);
}

void buildServerSenderType (IPNModel & m, int nc) {
  CompositeNet * net = new CompositeNet("ServerSender") ;
 
  net->addInstance("serv","Server",m) ;  
  net->addInstance ("treat",typeName("MultiCell",nc),m);

  // public syncs
  string tname;
  // nc synchros
  string tpart;
  for (int i=0; i < nc ; i++) {
    tpart = "call";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,"treat",typeName("put",i));
    net->addSyncPart(tname,"serv",tpart);

    tpart = "return";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,"treat",typeName("get",i));
    net->addSyncPart(tname,"serv",tpart);
  }

  m.addType(net);
}





void buildClientType (IPNModel & m) {
 UnitNet * res = new UnitNet("Client") ;
 
 res->addPlace ("Idle");
 res->addPlace ("Wait");
 res->addPlace ("Work");

 res->openStateDef("default");
 res->updateStateDef("Work",1);
 res->closeStateDef();

 string tname;
 tname = "endWork";
 res->addTransition(tname, PRIVATE);
 res->addArc ("Work",tname,1,INPUT);
 res->addArc ("Idle",tname,1,OUTPUT);

 tname = "sendReq";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Idle",tname,1,INPUT);
 res->addArc ("Wait",tname,1,OUTPUT);

 tname = "getReply";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Wait",tname,1,INPUT);
 res->addArc ("Work",tname,1,OUTPUT);

 m.addType(res);
}



void buildServerType (IPNModel & m) {
 UnitNet * res = new UnitNet("Server") ;
 
 res->addPlace ("Idle");
 res->addPlace ("Treat");
 res->addPlace ("Cleanup");

 res->openStateDef("default");
 res->updateStateDef("Cleanup",1);
 res->closeStateDef();

 string tname;
 tname = "endClean";
 res->addTransition(tname, PRIVATE);
 res->addArc ("Cleanup",tname,1,INPUT);
 res->addArc ("Idle",tname,1,OUTPUT);

 tname = "call";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Idle",tname,1,INPUT);
 res->addArc ("Treat",tname,1,OUTPUT);

 tname = "return";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Treat",tname,1,INPUT);
 res->addArc ("Cleanup",tname,1,OUTPUT);

 m.addType(res);
}



void buildRPCType (IPNModel & m) {
 UnitNet * res = new UnitNet("RPC") ;
 
 res->addPlace ("MemSource");
 res->addPlace ("MemDest");
 res->addPlace ("Request");
 res->addPlace ("Reply");

 string tname;
 tname = "sendReq";
 res->addTransition(tname, PUBLIC);
 res->addArc ("MemSource",tname,1,OUTPUT);
 res->addArc ("Request",tname,1,OUTPUT);

 tname = "call";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Request",tname,1,INPUT);
 res->addArc ("MemDest",tname,1,OUTPUT);

 tname = "return";
 res->addTransition(tname, PUBLIC);
 res->addArc ("MemDest",tname,1,INPUT);
 res->addArc ("Reply",tname,1,OUTPUT);

 tname = "getReply";
 res->addTransition(tname, PUBLIC);
 res->addArc ("MemSource",tname,1,INPUT);
 res->addArc ("Reply",tname,1,INPUT);

 m.addType(res);
}



void buildAnyDestRPCType (IPNModel & m, int nbDest) {
  CompositeNet * net = new CompositeNet(typeName("RPCAny",nbDest)) ;
  
  string inst = "dest";
  for (int i=0; i < nbDest ; i++) {
    net->addInstance(typeName(inst,i),"RPC",m);
  }

  string tname;  
  tname = "sendReq";
  // OR SYNCRO
  net->addSynchronization(tname,PUBLIC,false);
  for (int i=0; i < nbDest ; i++) {
    net->addSyncPart(tname,typeName(inst,i),tname);
  }
  tname = "getReply";
  // OR SYNCRO
  net->addSynchronization(tname,PUBLIC,false);
  for (int i=0; i < nbDest ; i++) {
    net->addSyncPart(tname,typeName(inst,i),tname);
  }

  string tpart;
  // named export
  for (int i=0; i < nbDest ; i++) {
    tpart = "call";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,typeName(inst,i),tpart);

    tpart = "return";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,typeName(inst,i),tpart);
  }

  m.addType(net);
}

void buildClientCallerType (IPNModel & m, int nbDest) {
  CompositeNet * net = new CompositeNet(typeName("ClientCaller",nbDest)) ;
  
  net->addInstance("cli","Client",m) ;
  net->addInstance("rpc",typeName("RPCAny",nbDest),m) ;
  
  string tname;  
  tname = "sendReq";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"cli",tname);
  net->addSyncPart(tname,"rpc",tname);

  tname = "getReply";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"cli",tname);
  net->addSyncPart(tname,"rpc",tname);

  string tpart;
  // named export
  for (int i=0; i < nbDest ; i++) {
    tpart = "call";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,"rpc",tname);

    tpart = "return";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PUBLIC);
    net->addSyncPart(tname,"rpc",tname);
  }

  m.addType(net);
}

void buildAnyCallerType (IPNModel & m, int nbSrc, int nbDest) {
  CompositeNet * net = new CompositeNet("AnyCaller") ;

  string inst = "cli";
  for (int i=0 ; i < nbSrc ; ++i ) {
    net->addInstance(typeName(inst,i),typeName("ClientCaller",nbDest),m) ;
  }
  
  string tname;  
  string tpart;
  // OR export
  for (int j=0; j < nbDest ; j++) {
    tpart = "call";
    tname = tpart + toString(j);
    // OR sync
    net->addSynchronization(tname,PUBLIC,false);
    for (int i=0 ; i < nbSrc ; ++i ) {
      net->addSyncPart(tname,typeName(inst,i),tname);
    }

    tpart = "return";
    tname = tpart + toString(j);
    // OR sync
    net->addSynchronization(tname,PUBLIC,false);
    for (int i=0 ; i < nbSrc ; ++i ) {
      net->addSyncPart(tname,typeName(inst,i),tname);
    }
  }

  m.addType(net);
}



void buildNaiveCS (int nc, int ns, IPNModel & m, bool reorder=false) {

  CompositeNet *net = new CompositeNet ("CliServ");
  if (! reorder) {
    for (int i=0 ; i < nc ; ++i ) {
      net->addInstance(clientName(i),"Client",m);
    }
    for (int i=0 ; i < nc ; ++i ) {
      for (int j=0 ; j < ns ; ++j ) {
	net->addInstance(RPCName(i,j),"RPC",m);
      }
    }
    for (int j=0 ; j < ns ; ++j ) {
      net->addInstance(serverName(j),"Server",m);
    }
  } else {
    for (int i=0 ; i < nc ; ++i ) {
      net->addInstance(clientName(i),"Client",m);

      for (int j=0 ; j < ns ; ++j ) {
	if (i==0)
	  net->addInstance(serverName(j),"Server",m);
	net->addInstance(RPCName(i,j),"RPC",m);
      }
    }
  }

  // nc x ns syncs from cli to RPC
  // nc x ns syncs from RPC to servers
  
  
  for (int i=0 ; i < nc ; ++i ) {
    for (int j=0 ; j < ns ; ++j ) {
      string ij = toString(i)+","+toString(j);
      string tpart,tname;
      
      tpart = "sendReq";
      tname = tpart + ij;
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,clientName(i),tpart);
      net->addSyncPart(tname,RPCName(i,j),tpart);

      tpart = "getReply";
      tname = tpart + ij;
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,clientName(i),tpart);
      net->addSyncPart(tname,RPCName(i,j),tpart);

      tpart = "call";
      tname = tpart + ij;
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,serverName(j),tpart);
      net->addSyncPart(tname,RPCName(i,j),tpart);

      tpart = "return";
      tname = tpart + ij;
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,serverName(j),tpart);
      net->addSyncPart(tname,RPCName(i,j),tpart);      
    }
  }

  m.addType(net);
}


void buildAnyDestCS (int nc, int ns, IPNModel & m) {

  CompositeNet *net = new CompositeNet ("CliServ");
  string rpc = "RPCAny";

  for (int i=0 ; i < nc ; ++i ) {
    net->addInstance(clientName(i),"Client",m);
    net->addInstance(typeName(rpc,i),typeName(rpc,ns),m);
    if (i==0)
      for (int j=0 ; j < ns ; ++j ) {
	net->addInstance(serverName(j),"Server",m);
      }
  }

  // nc syncs from cli to RPCAny
  // nc x ns syncs from RPCAny to servers
  
  string tpart,tname;
  for (int i=0 ; i < nc ; ++i ) {

    tpart = "sendReq";
    tname = tpart + toString(i);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,clientName(i),tpart);
    net->addSyncPart(tname,typeName(rpc,i),tpart);
    
    tpart = "getReply";
    tname = tpart  + toString(i);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,clientName(i),tpart);
    net->addSyncPart(tname,typeName(rpc,i),tpart);
    
    for (int j=0 ; j < ns ; ++j ) {
      string ij = toString(i)+","+toString(j);

      tpart = "call";      
      tname = tpart + ij;
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,serverName(j),tpart);
      net->addSyncPart(tname,typeName(rpc,i),typeName(tpart,j));

      tpart = "return";
      tname = tpart + ij;
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,serverName(j),tpart);
      net->addSyncPart(tname,typeName(rpc,i),typeName(tpart,j));      
    }
  }

  m.addType(net);
}



void buildAnyAnyCS (int nc, int ns, IPNModel & m) {

  CompositeNet *net = new CompositeNet ("CliServ");
  string rpc = "RPCAny";

  for (int j=0 ; j < ns ; ++j ) {
    net->addInstance(serverName(j),"Server",m);
  }
  net->addInstance("cli","AnyCaller",m);

  // ns syncs from AnyCaller to servers
  
  string tpart,tname;
  for (int j=0 ; j < ns ; ++j ) {
    tpart = "call";      
    tname = tpart + toString(j);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,serverName(j),tpart);
    net->addSyncPart(tname,"cli",tname);

    tpart = "return";
    tname = tpart + toString(j);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,serverName(j),tpart);
    net->addSyncPart(tname,"cli",tname);
  }
  

  m.addType(net);
}



void buildSenderCS (int nc, int ns, IPNModel & m, bool interlace) {

  CompositeNet *net = new CompositeNet ("CliServ");
  
  if (!interlace) {
    for (int j=0 ; j < ns ; ++j ) {
      net->addInstance(serverName(j),"ServerSender",m);
    }
    for (int i=0 ; i < nc ; ++i ) {
      net->addInstance(clientName(i),"ClientSender",m);
    }
  } else {
    for (int i=0 ; i < nc || i <ns ; ++i ) {
      if (i < ns) net->addInstance(serverName(i),"ServerSender",m);
      if (i < nc) net->addInstance(clientName(i),"ClientSender",m);
    }
  }

  // nc * ns syncs from clients to servers
  string tpart,tname;
  for (int i=0 ; i < nc ; ++i ) {
    for (int j=0 ; j < ns ; ++j ) {
      string ij = toString(i)+","+toString(j);
      tpart = "call";
      tname = tpart + ij;      
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,serverName(j),typeName(tpart,i));
      net->addSyncPart(tname,clientName(i),typeName(tpart,j));

      tpart = "return";
      tname = tpart + ij;      
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,serverName(j),typeName(tpart,i));
      net->addSyncPart(tname,clientName(i),tpart);

    }
  }

  m.addType(net);
}



void buildResourceRobin (IPNModel & m, const std::string & robinType) {

  CompositeNet *nrobin = new CompositeNet ("ResourceRobin");
  
  nrobin->addInstance("robins",robinType,m);
  nrobin->addInstance("res","Resource",m);

  nrobin->openStateDef("default");
  nrobin->updateStateDef("robins","first");
  nrobin->closeStateDef();

  string tname = "tbuf";
  nrobin->addSynchronization(tname,PRIVATE);
  nrobin->addSyncPart(tname,"robins","tbuf");
  nrobin->addSyncPart(tname,"res","return");

  tname = "task";
  nrobin->addSynchronization(tname,PRIVATE);
  nrobin->addSyncPart(tname,"robins","task");
  nrobin->addSyncPart(tname,"res","get");  

  m.addType (nrobin);
}

// in a context with n internal instances, connect 0 to 1, 1 to 2 etc.. n-2 to n-1 with private syncs.
void addInternalTransitions (CompositeNet * nrobin, int n) {
  std::cerr << "Augmenting Nary n=" << n << " composition " << nrobin->getName() << " with internal private transitions." << std::endl ;
  string tname;
  for (int i=0; i< n-1; ++i) {
    int next = i + 1  ;
    
    tname = "t1send"+toString(i);
    nrobin->addSynchronization(tname,PRIVATE);
    nrobin->addSyncPart(tname,instanceName(i),"t1sendLocal");
    nrobin->addSyncPart(tname,instanceName(next),"sendNext");

    tname = "t2send"+toString(i);
    nrobin->addSynchronization(tname,PRIVATE);
    nrobin->addSyncPart(tname,instanceName(i),"t2sendLocal");
    nrobin->addSyncPart(tname,instanceName(next),"sendNext");
  }
}


void addORTransitions (CompositeNet * nrobin, int n) {
  // add OR synchro
  string tname = "task";
  // isAND = false !!!
  nrobin->addSynchronization(tname,PUBLIC,false);
  for (int i= 0; i <n ; ++i)
    nrobin->addSyncPart(tname,instanceName(i),tname);

  tname = "tbuf";
  // isAND = false !!!
  nrobin->addSynchronization(tname,PUBLIC,false);
  for (int i= 0; i <n ; ++i)
    nrobin->addSyncPart(tname,instanceName(i),tname);
}

// in a context with n internal instances, create public "hooks" on left of 0 and right of n-1.
void addPublicTransitions (CompositeNet * nrobin, int n) {
  std::cerr << "Augmenting Nary n=" << n << " composition " << nrobin->getName() << " with public visibility syncs at edges "  << std::endl ;

  // export transitions 
  string tname;
  tname = "t1sendLocal";
  nrobin->addSynchronization(tname,PUBLIC);
  nrobin->addSyncPart(tname,instanceName(n-1),tname);

  tname = "t2sendLocal";
  nrobin->addSynchronization(tname,PUBLIC);
  nrobin->addSyncPart(tname,instanceName(n-1),tname);

  tname = "sendNext";
  nrobin->addSynchronization(tname,PUBLIC);
  nrobin->addSyncPart(tname,instanceName(0),tname);

  addORTransitions(nrobin,n);

}


// in a context with n internal instances, connect n-1 to 0 with private syncs.
void addCircularTransitions (CompositeNet * nrobin, int n) {

    std::cerr << "Augmenting Nary n=" << n << " composition " << nrobin->getName() << " with circular private syncs"  << std::endl ;
    string tname;
    tname = "t1send"+toString(n-1);
    nrobin->addSynchronization(tname,PRIVATE);
    nrobin->addSyncPart(tname,instanceName(n-1),"t1sendLocal");
    nrobin->addSyncPart(tname,instanceName(0),"sendNext");

    tname = "t2send"+toString(n-1);
    nrobin->addSynchronization(tname,PRIVATE);
    nrobin->addSyncPart(tname,instanceName(n-1),"t2sendLocal");
    nrobin->addSyncPart(tname,instanceName(0),"sendNext");

   addORTransitions (nrobin,n);
}



// build a component with (n * grain) units, internally connected, no circularity
CompositeNet * buildNaryRobinType (IPNModel & m, int n, int grain=1, int complement=0) {
  // 2^0 philo = 1 philo
  int N = n*grain + complement;
  std::cerr << "Building Nary composition " << robinName(N) << " as a (non circular) composition of " << n << " blocks of size " << grain  <<  " with complement =" << complement << std::endl ;
  CompositeNet *nrobin = new CompositeNet (robinName(N));

  nrobin->addInstance(instanceName(0),robinName(grain+complement),m);
  for (int i=1; i< n; ++i) {
    nrobin->addInstance(instanceName(i),robinName(grain),m);
  }
  
  nrobin->openStateDef("first");
  nrobin->updateStateDef(instanceName(0),"first");
  nrobin->closeStateDef();

  addInternalTransitions(nrobin,n);

  return nrobin;
}


// create a loop that uses a buildNaryType strategy (without hierarchy)
void buildCircularRobinType (IPNModel & m,int n, int grain=1, int complement=0) {
  // 2^0 philo = 1 philo
  CompositeNet *nrobin = buildNaryRobinType (m,n,grain,complement);

  addCircularTransitions(nrobin,n);

  m.addType (nrobin);
}

// build a component with (n * grain) units, internally connected, no circularity
// with additionally public events for synchro at each edge
void buildGrainRobinType (IPNModel & m,int n, int grain=1) {
  // 2^0 philo = 1 philo  
  CompositeNet *nrobin = buildNaryRobinType (m,n,grain);

  addPublicTransitions (nrobin,n);

  m.addType (nrobin);
}

void build2RobinType (IPNModel & m,int n1, int n2, bool isCircular = false) {
  // 2^0 philo = 1 philo
  std::cerr << "Building type " << robinName(n1+n2) << " as binary composition of "  << robinName(n1) << " and " << robinName(n2) << std::endl ;
  CompositeNet *nrobin = new CompositeNet (robinName(n1+n2));
  
  nrobin->addInstance(instanceName(0),robinName(n1),m);
  nrobin->addInstance(instanceName(1),robinName(n2),m);

  nrobin->openStateDef("first");
  nrobin->updateStateDef(instanceName(0),"first");
  nrobin->closeStateDef();
 
  addInternalTransitions(nrobin,2); 
  if (! isCircular)
    addPublicTransitions (nrobin,2);
  else
    addCircularTransitions (nrobin,2);
  
  m.addType (nrobin);
}


void buildRobinNPlus2 (IPNModel & m, int n) {
  CompositeNet *plus2 = new CompositeNet (robinName(n+2));
  std::cerr << "Building type " << robinName(n+2) << " as a ternary composition of "  << robinName(1) << ", " << robinName(n) << " and " << robinName(1) << std::endl ;  

  plus2->addInstance(instanceName(0),robinName(1),m);
  plus2->addInstance(instanceName(1),robinName(n),m);
  plus2->addInstance(instanceName(2),robinName(1),m);

  plus2->openStateDef("first");
  plus2->updateStateDef(instanceName(0),"first");
  plus2->closeStateDef();
  

  addInternalTransitions(plus2,3);
  addPublicTransitions (plus2,3);

  m.addType(plus2);
}


void closeRobin (IPNModel & m,int n) {
  CompositeNet *loop = new CompositeNet ("Loop");
  
  std::cerr << "Building type " << "Loop" << " as a circular pair of synchros over a single component " << robinName(n) << std::endl ;  

  loop->addInstance(instanceName(0),robinName(n),m);
  loop->openStateDef("first");
  loop->updateStateDef(instanceName(0),"first");
  loop->closeStateDef();

  addCircularTransitions(loop,1);

  m.addType (loop);
}

void buildBitWiseComposite (IPNModel & model,int grain)  {
  int grainleft = grain ;
  int done = -1;
  for (int i =0; i < 32  && grainleft ; ++i ) {
    if (i > 0)
      // skip 2 pow 0 as it is handled by buildRobinType already
      build2RobinType(model,1 << (i-1), 1 << (i-1));
    if (grainleft & 0x1) {
      // a bit
      if (done == -1)
	// first non zero bit
	done =  1 << i;
      else {
	// build a composite type
	build2RobinType(model,done, 1 << i);
	done += 1 << i ;
      }	    
    }
    
    grainleft >>= 1;
  }
  assert (grain ==done);
}


void buildNWiseComposite (IPNModel & model,int n, int grain, double factor = 1.0)  {
  int part = n / grain;
  int rem = n % grain ;
  if (model.findType(robinName(n)) != NULL)
    return ;
  else if (n < grain || grain == 1) {
    // use grain 1 for final level
      buildGrainRobinType (model, n);
    return ;
  } else if (part > 1)
    buildNWiseComposite (model, part, grain * factor);

  if (rem != 0)
    buildNWiseComposite (model, part + rem, grain * factor);

  std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " blocks " 
	    << " of sizes " << (grain-1) << "*" << part << ".." << part + rem << std :: endl ;
  CompositeNet *nrobin = buildNaryRobinType (model,grain,part,rem);
  
  addPublicTransitions(nrobin,grain);

  model.addType (nrobin);
}


enum Policy {NAIVE,REORDER,ANYDEST,ANYANY, ANYSENDER, DEPTH1, DEPTH1M1, RECFOLD, DEPTHREC, DEPTHPROG};

void print_usage() {
  std::cerr << " Client Server classical symmetric net (WN) example (see : Forte'05, Thierry-Mieg et al.). Encoded with SDD/DDD variants.\n"
	    << " Options : \n"
	    << " ./cliservIPN [NCLI=5] [NSERV=2] [OPTION=NAIVE,REORDER,ANYDEST,ANYANY, ANYSENDER] [GRAIN=1] \n"
	    << " where : NCLI,NSERV is model size in number of respectively clients and servers, \n"
	    << " \t GRAIN is size of small blocks assembled (only used for some strategies)\n"
	    << " \t DEPTH1 is similar to usual modular PN encoding, with N/GRAIN components assembled. If grain is not a multiple of N, the closest approximation is used. \n"
	    << " \t DEPTH1M1 is  built by adding one element at top and bottom in layered fashion (grain unused) \n"
	    << " \t RECFOLD is  built by using philo type bitwise recursive decomposition of n (grain unused) \n"
	    << " \t DEPTHPROG is  built by grain blocks decomposed into grain/2 blocks etc...  \n"
	    << " \t DEPTHREC is  built by using (more or less) grain variables at each level \n";  
}

int main (int argc, char **argv) {
  // 2^pow philo, e.g. pow = 3 -> 8 philo
  int nc,ns ;
  //  print_usage();
  if (argc > 1) {
    nc = atoi(argv[1]);
  } else {
    nc = 5;
  }
  if (argc > 2) {
    ns = atoi(argv[2]);
  } else {
    ns = 2;
  }

  Policy p = REORDER; 
  int argi = 3;
  if (argc > argi) {
    if (! strcmp (argv[argi],"REORDER"))
      p = REORDER;
    else if (! strcmp (argv[argi],"NAIVE"))
      p = NAIVE;
    else if (! strcmp (argv[argi],"ANYDEST"))
      p = ANYDEST;
    else if (! strcmp (argv[argi],"ANYANY"))
      p = ANYANY;
    else if (! strcmp (argv[argi],"ANYSENDER"))
      p = ANYSENDER;
/*    else if (! strcmp (argv[argi],"RECFOLD"))
      p = RECFOLD;
    else if (! strcmp (argv[argi],"DEPTHPROG"))
      p = DEPTHPROG;
 */
    else {
      printf ("bad strategy name ! %s",argv[argi]);
      print_usage();
      exit(1);
    }
  }
/*
  int grain;
  if (argc > 3) {
    grain = atoi(argv[3]); 
  } else {
    grain = 1;
  }
*/
  // create a model to hold net types.
  IPNModel model ;
  
  buildClientType(model);
  buildServerType(model);
  buildRPCType(model);

  string strat ;
  if (p == NAIVE) {
    buildNaiveCS(nc,ns,model,false);
    strat = "naive";
  } else if (p == REORDER) {
    buildNaiveCS(nc,ns,model,true);
    strat = "reor";
  } else if (p == ANYDEST) {
    buildAnyDestRPCType(model,ns);
    buildAnyDestCS(nc,ns,model);
    strat = "anyD";
  } else if (p == ANYANY) {
    buildAnyDestRPCType(model,ns);
    buildClientCallerType(model,ns);
    buildAnyCallerType (model, nc, ns);
    buildAnyAnyCS(nc,ns,model);
    strat = "anyany";
  } else if (p == ANYSENDER) {
    buildCellType(model,0);
    buildMultiCellAnyType(model,ns);
    buildMultiCellType(model,nc);
    buildClientSenderType(model,ns);
    buildServerSenderType(model,nc);
    buildSenderCS (nc,ns,model,true);
    strat = "send";
  }
  model.setInstance("CliServ",strat + toString(nc) + "cli" + toString(ns) + "serv");

  if (nc <= 3)
    std::cout << model << std::endl ;


  // State manipulations tests

  SDD M0 = model.getInstance()->getSDDState(0);
  Shom trans = model.getInstance()->getSDDTransRel (0);
  
  if (nc <= 3) {
    std::cout << M0 << std::endl ;
    Shom::pstats();
    Hom::pstats();
  }

  SDD Mfinal = fixpoint(trans+GShom::id) (M0);
  Statistic S = Statistic(Mfinal,model.getInstance()->getName(),CSV);  


  if (nc <= 3) {
    Shom::pstats();
    Hom::pstats();
  }


  std::ofstream out ("stats.tex",std::ios_base::app) ; 

  S.print_line(out);
  S.print_header(std::cout);
  S.print_line(std::cout);

  exportDot(Mfinal,"cliServ"+toString(nc)+"_"+toString(ns)+strat);  
}
