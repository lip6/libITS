#ifndef __CIRCULAR__SET__TYPE__
#define __CIRCULAR__SET__TYPE__


#include "its/scalar/CircularSet.hh"
#include "its/scalar/ScalarSetType.hh"

#include "its/ITSModel.hh"

namespace its {

/* A class to present a Circular Set as an ITS Type.
 * Note: Implementation is stored in  ScalarSetType.cpp. */
class CircularSetType : public ScalarSetType {
  const CircularSet & getCComp() const { return (const CircularSet &) getComp(); }

  protected :
    virtual void setComp(const ScalarSet & comp) { comp_ = new CircularSet((const CircularSet&)comp); }


public :
  CircularSetType (const CircularSet & c) : ScalarSetType() { setComp(c);};
  virtual ~CircularSetType() {}


  /** the set T of public transition labels (a copy)*/
  labels_t getTransLabels () const ;

 // use scalar set version
//  void printState (State s, std::ostream & os) const { os << "Please implement pretty state print for Circular Set" << std::endl; }
};



} // namespace

#endif

