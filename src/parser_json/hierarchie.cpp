#include "hierarchie.hh"

#include <iostream>

namespace json {

EltType PName::getType() {
  return PLA;
}

/**
 * Print element
 * \param cpt : Number of space to print
 */
void PName::print(int cpt) const  {
  for (int i = 0; i < cpt; ++i)
    std::cout << " ";
  std::cout << place << "[" << this->level << "]" << std::endl;
}

/**
 * Default constructor
 */
Hierarchie::Hierarchie() {
  level = 0;
  nbplace = 0;
}

/**
 * Destructor :
 * Delete all element in the vector of element
 */
Hierarchie::~Hierarchie() {
  for (std::vector<ItfElement *>::iterator iter = elts.begin(); iter != elts.end(); ++iter) {
    delete (*iter);
  }
}

/**
 * Add new Place Name
 * \param name Name of the place
 */
void Hierarchie::addPlace(const std::string& name) {
  PName* p = new PName();
  p->place = name;
  p->level = nbplace++;
  elts.push_back(p);
}

/**
 * Add a new hierarchy
 * \param h The reference of the new hierarchy
 */
void Hierarchie::addHierarchie(Hierarchie& h) {
  h.level = nbplace++;
  elts.push_back(&(h));
}

/**
 * Print the current element
 * \param cpt : Number of space
 */
void Hierarchie::print(int cpt) const {
  ++cpt;
  for (std::vector<ItfElement *>::const_iterator iter = elts.begin(); iter != elts.end(); ++iter) {
    (*iter)->print(cpt);
  }
}

/**
 * \return Return the type of the class
 */
EltType Hierarchie::getType() {
  return HIE;
}


}
