#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ctl/formula.hh"
#include "ctl/parser/parser.hh"
#include "ctl/tools/dot.hh"
#include "ctl/property.hh"

////////////////////////////////////////////////////////////////////

struct property
  : public ctl::property
{
  
  std::string text;
  
  property( const std::string& s)
    : text(s)
  {}
  
  std::string
  to_string()
  const
  {
    return text;
  }
};

////////////////////////////////////////////////////////////////////

struct property_factory
  : public ctl::property_factory
{

  // const std::auto_ptr<ctl::property*>
  ctl::property*
  operator()(const std::string& s)
  const
  {
    property* p = new property(s);
    return p;
  }

};

////////////////////////////////////////////////////////////////////

int
main()
{
  ctl::formulae f = ctl::parse_file( "./test.ctl" , property_factory() );

  unsigned int i = 0;
  for( ctl::formulae::const_iterator cit = f.begin()
     ; cit != f.end() ; ++cit )
  {
    std::stringstream file_name;
    file_name << "file_" << i << ".dot" ;
    ++i;
    std::ofstream file( file_name.str().c_str() );
    file << *cit;
  }
  
  return 0;
}

////////////////////////////////////////////////////////////////////
