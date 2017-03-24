#ifndef __CIRCULAR_SET__HH__
#define __CIRCULAR_SET__HH__

#include "its/scalar/ScalarSet.hh"
#include "its/scalar/CircularSync.hh"


namespace its {


  class CircularSet : public ScalarSet {
    
  public :

    // circular syncs
  typedef std::vector<CircularSync> circs_t;
  typedef circs_t::const_iterator circs_it;

  private :
    circs_t circs_;

  public :
    circs_it circs_begin() const { return circs_.begin() ; }
    circs_it circs_end() const { return circs_.end() ; }

    CircularSet (Label name) : ScalarSet(name) {};

    
    /** Add a circular synchronization to this composite.
     * Specify name (should be unique among circular synchronizations) */
    bool addSynchronization (Label sname, Label slabel);
    
    enum InstanceID { CURRENT, NEXT };
    /** A synchronization part : <instanceName,transitionLabels>. 
     *  Instance name may be CUR or NEXT the instances targeted in the transition
     */
    bool addSyncPart (Label sname, InstanceID subnetname, const labels_t & tname);

    // overloaded
    std::ostream & print (std::ostream & os) const ;
  };


}



#endif


