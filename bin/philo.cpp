#include "philo.hh"


namespace its {

  class Philo : public GAL {
  public:
    Philo():GAL("philo") {
      
      // Variable declarations
      {
	Variable var("fork[0]");
	addVariable(var,0);
      }
      {
	Variable var("fork[1]");
	addVariable(var,0);
      }
      {
	Variable var("fork[2]");
	addVariable(var,0);
      }
      {
	Variable var("fork[3]");
	addVariable(var,0);
      }
      {
	Variable var("phil_0.state");
	addVariable(var,0);
      }
      {
	Variable var("phil_1.state");
	addVariable(var,0);
      }
      {
	Variable var("phil_2.state");
	addVariable(var,0);
      }
      {
	Variable var("phil_3.state");
	addVariable(var,0);
      }

      // Transitions
      {
	GuardedAction ga ("t1" );
	BoolExpression procState = BoolExpressionFactory::createComparison ( EQ, Variable("phil_0.state"), IntExpressionFactory::createConstant(0) );
	BoolExpression guard = BoolExpressionFactory::createComparison ( EQ, Variable("fork[0]"), IntExpressionFactory::createConstant(0) );
	ga.setGuard(procState && guard);

	// actions
	ga.addAction(Assignment (Variable("phil_0.state"),1));
	ga.addAction(Assignment (Variable("fork[0]"),1));
	
	addTransition(ga);

      }


    };
    
    

  };


}

extern "C" its::GAL * createGAL() {
  return new its::Philo();
}
