#include "JSException.hh"

/**
 * \author HONG Silien
 * \version 1.0
 * \date 03/04/2009
 * \brief Definition module of the JSON Exception application.
 * We define here after all error exception that's application can throw.
 * Note :
 * All Exception are attached to an interface Action for execute special thing
 * that's user wants if error have been throw.
 */

// pretty print
std::ostream & operator << (std::ostream & os, const JSException& e) {
  os << e.str();
  return os;
}
