#ifndef __CTL_PROPERTY_HH__
#define __CTL_PROPERTY_HH__

// #include <memory>

namespace ctl {

////////////////////////////////////////////////////////////////////

struct property
{
  
  virtual
  std::string
  to_string()
  const = 0;
  
};

////////////////////////////////////////////////////////////////////

struct property_factory
{

  virtual
  // const std::auto_ptr<property*>
  property*
  operator()(const std::string&)
  const = 0;

};

////////////////////////////////////////////////////////////////////

} // namespace ctl

#endif // __CTL_PROPERTY_HH__
