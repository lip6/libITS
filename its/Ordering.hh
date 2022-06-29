/*
 * Ordering.hh
 *
 *  Created on: 2 déc. 2008
 *      Author: yann
 */

#ifndef ORDERING_HH_
#define ORDERING_HH_

#include <map>
#include <iosfwd>
#include "its/Naming.hh"

namespace its {

  /** A default variable used to label single node DDD or SDD. */
  extern const int DEFAULT_VAR;

/** A class to represent a variable ordering, i.e. a mapping from variable name to int.
 * Supports forward and backward lookup, and reindexing provided the variable set does not change.
 * It is initialized by passing a set of strings (=var names).
 */
class VarOrder {
	typedef std::map<vLabel,size_t> map_t;
	map_t indexes_;

	/** helper function to initialize the map */
	void setFromCollection (const labels_t & vars);
public :
  VarOrder (const labels_t & vars = labels_t() );
	/** return the index of a variable in this order */
	int getIndex (Label l) const;
	/** return the label asociated to an index.
	 * WARNING IF INDEX UNAPPROPRIATE asserts false !*/
	Label getLabel (size_t index) const;
	/** checks that vars is a permutation of the current variables, and sets to the new order.
	 * returns false and aborts if vars is not a permutation.
	 */
	bool updateOrder (const labels_t & vars);
  /** Add a single variable to index mapping. 
    * Checks are made to ensure index unicity, statement has no effect if the variable already has an index !!
    * so don't use this to reindex a VarOrder */
  bool addVariable (Label var, int index ) ;

  std::ostream & print (std::ostream & os) const ;
  
  size_t size() const { return indexes_.size() ; }
 };


}


#endif /* ORDERING_HH_ */
