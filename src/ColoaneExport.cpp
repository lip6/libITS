#include "ColoaneExport.hh"

#include <iostream>
#include <fstream>

#include "petri/TPNet.hh"
#include "composite/Composite.hh"
#include "TypeVisitor.hh"

using namespace its;

namespace its {
  namespace coloane  {

    class TypeExporter : public BasicTypeVisitor {
      // work directory
      vLabel wd;
     public :
      TypeExporter(Label path) : wd(path){};
      virtual ~TypeExporter() {};
      // or also could be a TPNet


    void exportComposite (const Composite & comp, std::ostream & os, Label stateName) {
      // next free id
      int id = 2;
      // Store name to  Coloane ID correspondance
      std::map<vLabel,int> ids;
      // Current state
      Composite::cstates_it state = comp.cstates_find(stateName);

      // XML file header
      os << "<?xml version='1.0' encoding='UTF-8'?>\n"
	 << "<model xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://coloane.lip6.fr/resources/schemas/model.xsd' formalism='ITSComposite' xposition='0' yposition='0'>\n"
	 << "<attributes>\n</attributes>\n";

      // Nodes
      os << "<nodes>\n";
      for (Composite::comps_it i = comp.comps_begin() ; i != comp.comps_end() ; ++i) {
	ids[i->getName()] = ++id;
	os << "<node nodetype='instance' id='"<< id << "' xposition='0' yposition='0' scale='100' interface='false' alt='0' foreground='#000000' background='#ffffff'>\n";
	os << "<attributes>\n";
	os << "<attribute name='name' xposition='0' yposition='0'>" << i->getName() << "</attribute>\n";
	os << "<attribute name='type' xposition='0' yposition='0'>"<< i->getType()->getName() << "_" << state->second.getSubState(i->getName()) <<"</attribute>\n" ;
	os << "</attributes>\n";	
	os << "</node>\n";
      }
      for (Composite::syncs_it sync = comp.syncs_begin() ; sync != comp.syncs_end() ; ++sync) {
	ids[sync->getName()] = ++id;
	os << "<node nodetype='synchronization' id='" << id << "' xposition='0' yposition='0' scale='100' interface='false' alt='0' foreground='#000000' background='#000000'>\n";
	os << "<attributes>\n";
	os << "<attribute name='label' xposition='0' yposition='0'>" << sync->getLabel() << "</attribute>\n";
	os << "<attribute name='name' xposition='0' yposition='0'>" << sync->getName() << "</attribute>\n";
	os << "</attributes>\n";
	os << "</node>\n";
      }
      os << "</nodes>\n";
      // ARCS
      os << "<arcs>\n";
      for (Composite::syncs_it sync = comp.syncs_begin() ; sync != comp.syncs_end() ; ++sync) {
	for (Synchronization::parts_it part = sync->begin() ; part != sync->end() ; ++part) {
	  os << "<arc arctype='arc' id='" << ++id << "' startid='" << ids[part->first] << "' endid='" << ids[sync->getName()] << "' color='#000000' curved='false'>\n";
	  os << "<attributes>\n"
	     << "<attribute name='labels' xposition='0' yposition='0'>" << part->second << "</attribute>\n";
	  os  << "</attributes>\n"
	     << "</arc>\n";
	}
      }
      os << "</arcs>\n";

      os << "<stickys>\n"
	 << "</stickys>\n"
	 << "</model>\n" << std::endl;
    }
    

    
    void exportTPN (const PNet & net, std::ostream & os, Label state)  {
      // next free id
      int id = 2;
      // Store name to  Coloane ID correspondance
      std::map<vLabel,int> pids,tids;
      // Current marking
      PNet::markings_it mark = net.markings_find(state);

      // XML file header
      os << "<?xml version='1.0' encoding='UTF-8'?>\n"
	 << "<model xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://coloane.lip6.fr/resources/schemas/model.xsd' formalism='Time Petri Net' xposition='0' yposition='0'>\n"
	 << "<attributes>\n</attributes>\n";
      // Nodes
      os << "<nodes>\n";
      for (PNet::trans_it t = net.transitions_begin() ; t != net.transitions_end() ; ++t ) {
	tids[t->getName()] = ++id;
	os << "<node nodetype='transition' "
	   << "id='" << id << "' xposition='0' yposition='0' scale='100' interface='false' alt='0' foreground='#000000' background='#ffffff'>\n";
	os << "<attributes>\n";
	os << "<attribute name='latestFiringTime' xposition='0' yposition='0'>" << TimeConstraint::infString(t->getClock().lft) << "</attribute>\n";
	os << "<attribute name='earliestFiringTime' xposition='0' yposition='0'>"<< t->getClock().eft <<"</attribute>\n";
	os << "<attribute name='visibility' xposition='0' yposition='0'>"<< (t->getVisibility()==PUBLIC?"public":"private") <<"</attribute>\n";
	os << "<attribute name='label' xposition='0' yposition='0'>"<< t->getLabel() <<"</attribute>\n";
	os << "</attributes>\n";
	os << "</node>\n";
      }
      for (PNet::places_it p=net.places_begin() ; p != net.places_end() ; ++p) {
	pids[p->getName()] = ++id;
	os << "<node nodetype='place' id='"<< id <<"' xposition='0' yposition='0' scale='100' interface='false' alt='0' foreground='#000000' background='#ffffff'>\n"
	   << "<attributes>\n"
	   << "<attribute name='name' xposition='0' yposition='0'>"<< p->getName()  <<"</attribute>\n"
	   << "<attribute name='marking' xposition='0' yposition='0'>"<< mark->second.getMarking(p->getName())  << "</attribute>\n"
	   << "</attributes>\n";	
	os << "</node>\n";
      }
      os << "</nodes>\n";

      // ARCS
      os << "<arcs>\n";
      for (PNet::trans_it t = net.transitions_begin() ; t != net.transitions_end() ; ++t ) {
	for (PTransition::arcs_it arc = t->begin(); arc != t->end() ; ++arc) {
	  ++id;
	  vLabel type;
	  int src=0,dest=0,val=0;
	  if (arc->getType() == INPUT) {
	    type = "arc";
	    src = pids[arc->begin()->getPlace()];
	    dest = tids[t->getName()];
	    val = arc->begin()->getValuation();
	  } else if (arc->getType() == OUTPUT) {
	    type = "arc";
	    dest = pids[arc->begin()->getPlace()];
	    src = tids[t->getName()];
	    val = arc->begin()->getValuation();
	  } else if (arc->getType() == INHIBITOR) {
	    type = "inhibitor";
	    src = pids[arc->begin()->getPlace()];
	    dest = tids[t->getName()];
	    val = arc->begin()->getValuation();
	  } else if (arc->getType() == TEST) {
	    type = "test";
	    src = pids[arc->begin()->getPlace()];
	    dest = tids[t->getName()];
	    val = arc->begin()->getValuation();
	  } else if (arc->getType() == RESET) {
	    type = "reset";
	    src = pids[arc->begin()->getPlace()];
	    dest = tids[t->getName()];
	    val = 0;
	  } 	  
	  os << "<arc arctype='" << type << "' id='"<< id << "' startid='" << src << "' endid='" << dest << "' color='#000000' curved='true'>\n" ;
	  os << "<attributes>\n";	  
	  if (arc->getType() != RESET) {
	    os << "<attribute name='valuation' xposition='0' yposition='0'>" <<  val  << "</attribute>\n" ;
	  }
	  os << "</attributes>\n";
	  os << "</arc>\n";
	}
      }
      os << "</arcs>\n";

      os << "<stickys>\n"
	 << "</stickys>\n"
	 << "</model>\n" << std::endl;
      
    }

      void visitPNet (const class PNet & net) {
	for (PNet::markings_it state = net.markings_begin(); state != net.markings_end() ; ++state) {
	  std::ofstream os ( std::string(wd + "/" + net.getName() + "_" + state->first + ".model").c_str() );
	  exportTPN(net,os, state->first);
	  os.close();
	}
      }
      void visitComposite (const class Composite & net) {
	for (Composite::cstates_it state = net.cstates_begin() ; state != net.cstates_end() ; ++state) {
	  std::ofstream os (std::string(wd + "/" + net.getName() + "_" + state->first + ".model").c_str());
	  exportComposite(net,os, state->first);
	  os.close();
	}
      }
      void visitScalar (const class ScalarSet & net) { std::cerr << "Export to coloane not implemented for Scalar Set !!" << std::endl; }
      void visitCircular (const class CircularSet & net) { std::cerr << "Export to coloane not implemented for Circular Set !!" << std::endl; }


    };
    void exportToColoane (const ITSModel & model, Label directory) {
      // export each Type to a separate .model file
      TypeExporter te(directory);
      model.visitTypes(&te);
    }


  }
}
