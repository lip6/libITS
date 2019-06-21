#ifndef __TO_TRANS_REL_HH__
#define __TO_TRANS_REL_HH__

#include "its/ITSModel.hh"

its::State getTransRel (const its::ITSModel & m);

its::State toRelation (const its::State & s, const its::Transition & t);

#endif
