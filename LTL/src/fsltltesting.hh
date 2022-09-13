#ifndef __FSLTLTGTA__HH__
#define __FSLTLTGTA__HH__

#include "fsltl.hh"

#include "etf/ETFTestingType.hh"

namespace its
{

  /** This class handles the construction of a synchronized product of an ITSModel with an ITS representation of a TGBA.
   *  It builds a composite adapted to the problem and contains the SCC search algorithm. */
  class fsltlTestingModel : public fsltlModel
  {

    EtfTestingType* model_labled_by_changesets_;

    State initState_;

    Transition allStutteringTrans_;

  public:

    fsltlTestingModel() :
      model_labled_by_changesets_(NULL), initState_(State::null),
          allStutteringTrans_(Transition::null)
    {
    }
    ;

    virtual bool
    declareETFType(Label path);

    virtual State
    getInitState();

    virtual bool
    buildComposedSystem();

    virtual EtfTestingType*
    getTestingModel()
    {
      return model_labled_by_changesets_;
    }

  };

}

#endif
