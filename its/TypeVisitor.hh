#ifndef __TYPE__VISITOR__HH__
#define __TYPE__VISITOR__HH__

#include "its/ITSModel.hh"

namespace its {

  class TypeVisitor {
  public :
    virtual ~TypeVisitor() {};
    // or also could be a TPNet
    virtual void visitPNet (const class PNet &) = 0;
    virtual void visitComposite (const class Composite &) = 0;
    virtual void visitScalar (const class ScalarSet &) = 0;
    virtual void visitCircular (const class CircularSet &) = 0;
    virtual void visitGAL (const class GAL &) = 0;
  };

  // A default empty behavior for visitors not supporting all types.
  class BasicTypeVisitor : public TypeVisitor {
  public :
    virtual ~BasicTypeVisitor() {};
    // or also could be a TPNet
    virtual void visitPNet (const class PNet &) {};
    virtual void visitComposite (const class Composite &) {};
    virtual void visitScalar (const class ScalarSet &) {};
    virtual void visitCircular (const class CircularSet &) {};
    virtual void visitGAL (const class GAL &) {};
  };


}

#endif
