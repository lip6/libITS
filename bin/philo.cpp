#include <stdio.h>
#include <string.h>
#include <stdint.h>


#include "gal/GAL.hh"

namespace its
{
    class Philo : public GAL 
    {
        public:
        Philo():GAL("philo") 
        {
                addVariable(Variable("choosing[0]"),0);
                addVariable(Variable("choosing[1]"),0);
                addVariable(Variable("number[0]"),0);
                addVariable(Variable("number[1]"),0);
                addVariable(Variable("P_0.state"),0);
                addVariable(Variable("P_0.j"),0);
                addVariable(Variable("P_0.max"),0);
                addVariable(Variable("P_1.state"),0);
                addVariable(Variable("P_1.j"),0);
                addVariable(Variable("P_1.max"),0);
            
            if ( false )
            {
            }
            else
            {
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_0.state") , IntExpressionFactory::createConstant(0));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                        ;
                        GuardedAction ga("t0");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("choosing[0]"), IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("P_0.j"), IntExpressionFactory::createConstant(0))));
                        (ga.addAction(Assignment(Variable("P_0.max"), IntExpressionFactory::createConstant(0))));
                        // read : 0,0,0,0,1,0,0,0,0,0
                        // write: 1,0,0,0,1,1,1,0,0,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_0.state") , IntExpressionFactory::createConstant(1));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( LT, Variable("P_0.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( GT, Variable("number[2]") , Variable("P_0.max"))) ) ;
                        GuardedAction ga("t1");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("P_0.max"), Variable("number[2]"))));
                        (ga.addAction(Assignment(Variable("P_0.j"), Variable("P_0.j") +  IntExpressionFactory::createConstant(1))));
                        // read : 0,0,1,1,1,1,1,0,0,0
                        // write: 0,0,0,0,1,1,1,0,0,0
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( LT, Variable("P_0.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("number[2]") , Variable("P_0.max"))) ) ;
                        GuardedAction ga("t2");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("P_0.j"), Variable("P_0.j") +  IntExpressionFactory::createConstant(1))));
                        // read : 0,0,1,1,1,1,1,0,0,0
                        // write: 0,0,0,0,1,1,0,0,0,0
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, Variable("P_0.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( LT, Variable("P_0.max") , IntExpressionFactory::createConstant(9))) ) ;
                        GuardedAction ga("t3");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 2 ));
                        (ga.addAction(Assignment(Variable("number[0]"), Variable("P_0.max") +  IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("P_0.j"), IntExpressionFactory::createConstant(0))));
                        (ga.addAction(Assignment(Variable("choosing[0]"), IntExpressionFactory::createConstant(0))));
                        // read : 0,0,0,0,1,1,1,0,0,0
                        // write: 1,0,1,0,1,1,0,0,0,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_0.state") , IntExpressionFactory::createConstant(2));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( LT, Variable("P_0.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( EQ, Variable("choosing[2]") , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t4");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 3 ));
                        // read : 1,1,0,0,1,1,0,0,0,0
                        // write: 0,0,0,0,1,0,0,0,0,0
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, Variable("P_0.j") , IntExpressionFactory::createConstant(2))) ) ;
                        GuardedAction ga("t5");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 4 ));
                        // read : 0,0,0,0,1,1,0,0,0,0
                        // write: 0,0,0,0,1,0,0,0,0,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_0.state") , IntExpressionFactory::createConstant(3));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, Variable("number[2]") , IntExpressionFactory::createConstant(0)) ||  (BoolExpressionFactory::createComparison ( GT, Variable("number[2]") , Variable("number[0]"))) ||  (BoolExpressionFactory::createComparison ( EQ, Variable("number[2]") , Variable("number[0]")) &&  BoolExpressionFactory::createComparison ( LEQ, IntExpressionFactory::createConstant(0) , Variable("P_0.j")))) ) ;
                        GuardedAction ga("t6");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 2 ));
                        (ga.addAction(Assignment(Variable("P_0.j"), Variable("P_0.j") +  IntExpressionFactory::createConstant(1))));
                        // read : 0,0,1,1,1,1,0,0,0,0
                        // write: 0,0,0,0,1,1,0,0,0,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_0.state") , IntExpressionFactory::createConstant(4));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                        ;
                        GuardedAction ga("t7");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("number[0]"), IntExpressionFactory::createConstant(0))));
                        // read : 0,0,0,0,1,0,0,0,0,0
                        // write: 0,0,1,0,1,0,0,0,0,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_1.state") , IntExpressionFactory::createConstant(0));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                        ;
                        GuardedAction ga("t8");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 1 ));
                        (ga.addAction(Assignment(Variable("choosing[1]"), IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("P_1.j"), IntExpressionFactory::createConstant(0))));
                        (ga.addAction(Assignment(Variable("P_1.max"), IntExpressionFactory::createConstant(0))));
                        // read : 0,0,0,0,0,0,0,1,0,0
                        // write: 0,1,0,0,0,0,0,1,1,1
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_1.state") , IntExpressionFactory::createConstant(1));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( LT, Variable("P_1.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( GT, Variable("number[4]") , Variable("P_1.max"))) ) ;
                        GuardedAction ga("t9");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 1 ));
                        (ga.addAction(Assignment(Variable("P_1.max"), Variable("number[4]"))));
                        (ga.addAction(Assignment(Variable("P_1.j"), Variable("P_1.j") +  IntExpressionFactory::createConstant(1))));
                        // read : 0,0,1,1,0,0,0,1,1,1
                        // write: 0,0,0,0,0,0,0,1,1,1
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( LT, Variable("P_1.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("number[4]") , Variable("P_1.max"))) ) ;
                        GuardedAction ga("t10");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 1 ));
                        (ga.addAction(Assignment(Variable("P_1.j"), Variable("P_1.j") +  IntExpressionFactory::createConstant(1))));
                        // read : 0,0,1,1,0,0,0,1,1,1
                        // write: 0,0,0,0,0,0,0,1,1,0
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, Variable("P_1.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( LT, Variable("P_1.max") , IntExpressionFactory::createConstant(9))) ) ;
                        GuardedAction ga("t11");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 2 ));
                        (ga.addAction(Assignment(Variable("number[1]"), Variable("P_1.max") +  IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("P_1.j"), IntExpressionFactory::createConstant(0))));
                        (ga.addAction(Assignment(Variable("choosing[1]"), IntExpressionFactory::createConstant(0))));
                        // read : 0,0,0,0,0,0,0,1,1,1
                        // write: 0,1,0,1,0,0,0,1,1,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_1.state") , IntExpressionFactory::createConstant(2));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( LT, Variable("P_1.j") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( EQ, Variable("choosing[4]") , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t12");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 3 ));
                        // read : 1,1,0,0,0,0,0,1,1,0
                        // write: 0,0,0,0,0,0,0,1,0,0
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, Variable("P_1.j") , IntExpressionFactory::createConstant(2))) ) ;
                        GuardedAction ga("t13");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 4 ));
                        // read : 0,0,0,0,0,0,0,1,1,0
                        // write: 0,0,0,0,0,0,0,1,0,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_1.state") , IntExpressionFactory::createConstant(3));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, Variable("number[4]") , IntExpressionFactory::createConstant(0)) ||  (BoolExpressionFactory::createComparison ( GT, Variable("number[4]") , Variable("number[1]"))) ||  (BoolExpressionFactory::createComparison ( EQ, Variable("number[4]") , Variable("number[1]")) &&  BoolExpressionFactory::createComparison ( LEQ, IntExpressionFactory::createConstant(1) , Variable("P_1.j")))) ) ;
                        GuardedAction ga("t14");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 2 ));
                        (ga.addAction(Assignment(Variable("P_1.j"), Variable("P_1.j") +  IntExpressionFactory::createConstant(1))));
                        // read : 0,0,1,1,0,0,0,1,1,0
                        // write: 0,0,0,0,0,0,0,1,1,0
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("P_1.state") , IntExpressionFactory::createConstant(4));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                        ;
                        GuardedAction ga("t15");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("P_1.state") , 0 ));
                        (ga.addAction(Assignment(Variable("number[1]"), IntExpressionFactory::createConstant(0))));
                        // read : 0,0,0,0,0,0,0,1,0,0
                        // write: 0,0,0,1,0,0,0,1,0,0
                        addTransition(ga);
                    }
                }
            }
        }
    }
    ;
}
extern "C" its::GAL * createGAL()
{
    return new its::Philo();
}
