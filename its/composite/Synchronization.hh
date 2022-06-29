/*
 * Synchronization.hh
 *
 *  Created on: 3 déc. 2008
 *      Author: yann
 */

#ifndef SYNCHRONIZATION_HH_
#define SYNCHRONIZATION_HH_

#include "its/Naming.hh"
#include "its/Instance.hh"



namespace its {

class Synchronization : public NamedElement {
  friend class CompositeType;
  friend class TCompositeType;

  /** A type representing an instance label : instance.tr */
  typedef std::pair<vLabel,vLabel> syncPart;
public:
  typedef std::vector<syncPart> parts_t;
  typedef parts_t::const_iterator parts_it;

private :  
  // slave/subservient transitions
  parts_t parts_;
  // synchronization label (or empty string if private sync)
  vLabel label_;

public :
  parts_it begin () const { return parts_.begin() ; }
  parts_it end () const { return parts_.end() ; }

  /** A synchronization has a name (ID), and a label or "" to denote a private synchronization */
  Synchronization (Label name, Label label )
    : NamedElement(name),label_(label) {};

  Label getLabel () const {
    return label_;
  }
  /** Add a subservient transition : firing this triggers firing of the part.*/
  void addPart (Label subnet, Label trLabel) { parts_.push_back(syncPart(subnet,trLabel)); }

  /** pretty print */
  std::ostream & print (std::ostream & os) const;
  
};

} // namespace

#endif /* SYNCHRONIZATION_HH_ */
