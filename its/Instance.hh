/*
 * Instance.hh
 *
 *  Created on: 25 nov. 2008
 *      Author: yann
 */

#ifndef ___INSTANCE__HH__
#define ___INSTANCE__HH__

#include "its/Type.hh"
#include <iostream>

namespace its {

/** immutable structure used to represent an instance */
class Instance : public NamedElement {
	pType type_;
public :
	/** constructor : provide a label as name and a type */
	Instance ( Label name, pType type ) : NamedElement(name),type_(type) {};

	/** const accessor */
	pType getType () const { return type_ ; }
	/** pretty print using name:type style presentation */
  std::ostream & print (std::ostream & os) const { return ( os << getName() << ":" << getType()->getName() ); }
};

typedef const Instance * pInstance;

}

#endif /* INSTANCE_HH_ */
