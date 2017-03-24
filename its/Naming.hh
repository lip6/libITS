/** Some typedefs and template functions to work with
 * vLabel=string
 * Label = const string &
 * And also named elements.
 */
#ifndef __NAMING__H__
#define __NAMING__H__

#include <string>
#include <vector>
#include <sstream>


/// an object label
typedef const std::string & Label;
typedef std::string vLabel;

/**	A type to represent a set of labels */
typedef std::vector<vLabel> labels_t;
typedef labels_t::const_iterator labels_it;

template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}



class NamedElement {
  std::string name_;
public :
  NamedElement (Label name) : name_(name) {}
  virtual ~NamedElement() {};

  Label getName () const { return name_ ; }
};

template<typename iterator>
iterator findName (Label name, const iterator & begin, const iterator & end) {
  for (iterator it = begin;
       it != end;
       ++it ) {
    if (it->getName() == name)
      return it;
  }
  return end;
};

template<typename container>
typename container::const_iterator findName (Label name, const container & cont) {
  return findName(name,cont.begin(),cont.end());
}


template<typename iterator>
int findNameIndex (Label name, const iterator & begin, const iterator & end) {
  int index = 0;
  for (iterator it = begin;
       it != end;
       ++it,++index ) {
    if (it->getName() == name)
      return index;
  }
  return -1;
};

template<typename container>
int findNameIndex (Label name, const container & cont) {
  return findNameIndex(name,cont.begin(),cont.end());
}

#endif
