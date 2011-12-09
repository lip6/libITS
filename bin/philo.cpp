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
                addVariable(Variable("time"),0);
                addVariable(Variable("done[0]"),0);
                addVariable(Variable("at_modul"),0);
                addVariable(Variable("body"),0);
                addVariable(Variable("solved[0]"),0);
                addVariable(Variable("solved[1]"),0);
                addVariable(Variable("solved[2]"),0);
                addVariable(Variable("solved[3]"),0);
                addVariable(Variable("solved[4]"),0);
                addVariable(Variable("solved[5]"),0);
                addVariable(Variable("solved[6]"),0);
                addVariable(Variable("solved[7]"),0);
                addVariable(Variable("solved[8]"),0);
                addVariable(Variable("solved[9]"),0);
                addVariable(Variable("solved[10]"),0);
                addVariable(Variable("solved[11]"),0);
                addVariable(Variable("solved[12]"),0);
                addVariable(Variable("solved[13]"),0);
                addVariable(Variable("solved[14]"),0);
                addVariable(Variable("solved[15]"),0);
                addVariable(Variable("solved[16]"),0);
                addVariable(Variable("solved[17]"),0);
                addVariable(Variable("solved[18]"),0);
                addVariable(Variable("solved[19]"),0);
                addVariable(Variable("Timer.state"),0);
                addVariable(Variable("Person_0.state"),0);
            
            if ( false )
            {
            }
            else
            {
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Timer.state") , IntExpressionFactory::createConstant(0));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( LT, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t0");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Timer.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(0))));
                        (ga.addAction(Assignment(Variable("time"), Variable("time") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0
                        // write: 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(0));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t1");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t2");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 3 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t3");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 5 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(3)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(2)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t4");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[3]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(6)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(3)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t5");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[6]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(9)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(7)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t6");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[9]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(10)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(6)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t7");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[10]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(11)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(7)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t8");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[11]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(12)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(9)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t9");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[12]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(15)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(11)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t10");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[15]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(1));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t11");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t12");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 8 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t13");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 7 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t14");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 3 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(19)) , IntExpressionFactory::createConstant(1)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(17)) , IntExpressionFactory::createConstant(1))) ) ;
                        GuardedAction ga("t15");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 9 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(7)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(6)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t16");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[7]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(16)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t17");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[18]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(3));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t18");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t19");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 6 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t20");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 4 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t21");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(5)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(5)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(5))) ) ;
                        GuardedAction ga("t22");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 3 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[5]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(7)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(6)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t23");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 3 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[7]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( GEQ, Variable("body") , IntExpressionFactory::createConstant(10)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t24");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 3 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[17]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(4));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t25");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 3 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t26");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 5 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(2))) ) ;
                        GuardedAction ga("t27");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 4 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(4)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(5)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(5))) ) ;
                        GuardedAction ga("t28");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 4 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[4]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( GEQ, Variable("body") , IntExpressionFactory::createConstant(10)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t29");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 4 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[16]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(5));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t30");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 4 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t31");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 0 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(1)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(4))) ) ;
                        GuardedAction ga("t32");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 5 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[1]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(8)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(8)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(9))) ) ;
                        GuardedAction ga("t33");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 5 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[8]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(6));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t34");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 3 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(13)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(12)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t35");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 6 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[13]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(7)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(6)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(24))) ) ;
                        GuardedAction ga("t36");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 6 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[7]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(7));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t37");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(2))) ) ;
                        GuardedAction ga("t38");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 7 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(18)) , IntExpressionFactory::createConstant(1)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t39");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 7 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[19]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(8));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t40");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 1 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(2)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(1)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(4))) ) ;
                        GuardedAction ga("t41");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 8 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[2]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("solved"),IntExpressionFactory::createConstant(14)) , IntExpressionFactory::createConstant(0)) &&  BoolExpressionFactory::createComparison ( GEQ, Variable("time") , IntExpressionFactory::createConstant(9)) &&  BoolExpressionFactory::createComparison ( LEQ, Variable("time") , IntExpressionFactory::createConstant(13))) ) ;
                        GuardedAction ga("t42");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 8 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("solved[14]"),IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("body"), Variable("body") +  IntExpressionFactory::createConstant(1))));
                        // read : 1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1
                        // write: 0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(9));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t43");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 10 ));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
                        addTransition(ga);
                    }
                }
                {
                    BoolExpression procState =BoolExpressionFactory::createComparison ( EQ,Variable("Person_0.state") , IntExpressionFactory::createConstant(10));
                    {
                        BoolExpression guard = BoolExpressionFactory::createConstant(true) 
                         &&  ( (BoolExpressionFactory::createComparison ( EQ, IntExpressionFactory::createArrayAccess(Variable("done"),IntExpressionFactory::createConstant(0)) , IntExpressionFactory::createConstant(0))) ) ;
                        GuardedAction ga("t44");
                        ga.setGuard(guard && procState);
                        ga.addAction( Assignment(Variable("Person_0.state") , 11 ));
                        (ga.addAction(Assignment(Variable("at_modul"), Variable("at_modul") +  IntExpressionFactory::createConstant(1))));
                        (ga.addAction(Assignment(Variable("done[0]"),IntExpressionFactory::createConstant(1))));
                        // read : 0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        // write: 0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
                        
                        
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
