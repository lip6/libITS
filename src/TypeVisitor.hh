#ifndef __TYPE__VISITOR__HH__
#define __TYPE__VISITOR__HH__

#include "ITSModel.hh"

namespace its {

  class TypeVisitor {
  public :
    virtual ~TypeVisitor() {};
    // or also could be a TPNet
    virtual void visitPNet (const class PNet & net) = 0;
    virtual void visitComposite (const class Composite & net) = 0;
    virtual void visitScalar (const class ScalarSet & net) = 0;
    virtual void visitCircular (const class CircularSet & net) = 0;
    virtual void visitGAL (const class GAL & net) = 0;
  };

  // A default empty behavior for visitors not supporting all types.
  class BasicTypeVisitor : public TypeVisitor {
  public :
    virtual ~BasicTypeVisitor() {};
    // or also could be a TPNet
    virtual void visitPNet (const class PNet & net) {};
    virtual void visitComposite (const class Composite & net) {};
    virtual void visitScalar (const class ScalarSet & net) {};
    virtual void visitCircular (const class CircularSet & net) {};
    virtual void visitGAL (const class GAL & net) {};
  };


}

#endif
