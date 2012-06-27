grammar exprParser;

/* 
 * GAL Antlr Grammar for C++
 */
options {
  language = C;
}

@parser::header {

  #include "gal/GAL.hh"
  #include <iostream>
  #include <cstdlib>
}

@members {

  // "result" is the root of GAL elements.
  its::GAL* result;
  // pointer on the current transition (GuardedAction) found
  its::GuardedAction* current_ga = NULL ;
  
  
  /// Gets the text of a token and returns it as a string
  static std::string toString(pANTLR3_COMMON_TOKEN i) {
    return std::string((const char *)i->getText(i)->chars);
  }
  
  // Returns an integer from a token, expected as a string of int.
  static int toInt(pANTLR3_COMMON_TOKEN i) {
    int n = atoi((const char *)i->getText(i)->chars);
    return n;
  }
  
  // Check if there is a variable named "str" in GAL system.  
  void checkVariableExistance(std::string str)
  {
    int found = 0 ; 
    for (its::GAL::vars_it v = result->vars_begin() ; 
                           v != result->vars_end() ; 
                           v++) 
    {
       if(v->getName() == str)
       {
        found = 1 ; 
        break ; 
       }
    }
    if (!found)
    {
      std::cerr << "Variable '" << str << "' does not exists." << std::endl ;
      exit(1) ; 
    }
   }
  
  // Check if there is an array named "str" in GAL system.  
  void checkArrayExistance(std::string str)
  {
    int found = 0 ; 
    for (its::GAL::arrays_it v = result->arrays_begin() ; 
                           v != result->arrays_end() ; 
                           v++) 
    {
       if(v->getName() == str)
       {
        found = 1 ; 
        break ; 
       }
    }
    if (!found)
    {
      std::cerr << "Array named '" << str << "' does not exist" << std::endl ;
      exit(1) ; 
    }
  }
}


system returns [its::GAL* r] :
  'GAL' name=qualifiedName
  
  {result = new its::GAL ( $name.res );}
  '{'
    // Variable declarations
    (variableDeclaration
    | arrayDeclaration 
/*------------ NOT IMPLEMENTED SECTION --------------
    | listDeclaration 
 ---------------------------------------------------*/
    )*
    
    // Transitions
    (transition)+
    
    // Transient
    (transient)?
  '}'
  {$r = result;}
;

variableDeclaration :
  'int' name=qualifiedName '=' initVal=integer ';'
  {
    its::Variable var ($name.res);
    int val = $initVal.res;
    
    result->addVariable(var, val);
  }
  ;
  
arrayDeclaration :
  'array' '[' INT ']' name=qualifiedName '=' '(' inits=initValues ')' ';'
  {
    result->addArray($name.res, $inits.vres);
  }
  ;


/* --------------------- NOT IMPLEMENTED SECTION   ------------------
  //List declaration not implemented in C++
  listDeclaration :
   'list' name=qualifiedName ('=' '(' inits=initValues ')' )? ';'
  ;
 -------------------------------------------------------------------*/


transition 
@init {
    its::Assignment* ptrAction = NULL ;
} : 
  'transition' name=qualifiedName 
  '[' gard=boolOr ']'
  {
  current_ga = new its::GuardedAction($name.res);
  current_ga->setGuard($gard.bres);
  }
  
  ('label' label=STRING)?
  
  '{'
  (
    // Normal actions (assignments)
	  (
		  currentAction=action 
		  {
		  ptrAction = $currentAction.as ;
		  current_ga->addAction(*ptrAction);
		  delete ptrAction ;
		  }
	  )
/* --------------- NOT IMPLEMENTED SECTION -------------------------
	  |
	  (
	    listPop | listPush
	  )
------------------------------------------------------------------*/
  )*
  '}'
  {
    result->addTransition(*current_ga);
    delete current_ga ; 
  }
  ;

action returns [its::Assignment* as = NULL]:
  var=varAccess  '=' val=bit_or ';'
  {as = new its::Assignment($var.ires, $val.ires);}
;

transient :
  'TRANSIENT' '=' value=boolOr ';'
  {result->setTransientPredicate($value.bres);}
;


/*-------------------  NOT IMPLEMENTED SECTION ------------------*
//List actions not implemented in C++
listPop :
  qualifiedName '.pop' '(' ')' ';'
;

listPush :
  qualifiedName '.push' '(' bit_or ')' ';'
;
----------------------------------------------------------------*/


//////////////////// Boolean Expressions
boolOr returns [its::BoolExpression bres] :
  left=boolAnd
  {$bres = $left.bres;}
  
  ('||' right=boolAnd
  {$bres = its::BoolExpressionFactory::createBinary(
    its::OR, $bres, $right.bres);}
  )*
;

boolAnd returns [its::BoolExpression bres] :
  left=boolNot
  {$bres = $left.bres;}
  
  ('&&' right=boolNot
  {$bres = its::BoolExpressionFactory::createBinary(
    its::AND, $bres, $right.bres);}
  )*
;

boolNot returns [its::BoolExpression bres] :
  ('!'
    current = boolPrimary
    {
    $bres = its::BoolExpressionFactory::createNot($current.bres);
    }
  ) 
  |
  current = boolPrimary 
  {$bres = $current.bres;}
;

boolPrimary returns [its::BoolExpression bres] : 
  'False'
  {$bres = its::BoolExpressionFactory::createConstant(false);}
  | 
  'True'
  {$bres = its::BoolExpressionFactory::createConstant(true);}
  |
  ((comparison) =>
  current = comparison 
  {$bres = $current.bres;})
  |
  ('(' current = boolOr ')'
  {$bres = $current.bres;})
;

comparison returns [its::BoolExpression bres] :
  left=bit_or op=comparisonOperators right=bit_or
  
  {$bres = 
    its::BoolExpressionFactory::createComparison($op.opres,$left.ires,$right.ires);} 
;

/////////////////// Int Expressions
bit_or returns [its::IntExpression ires]
@init{its::IntExprType op;}
:
  left = bitxor
  {$ires = $left.ires;}
  
  (('|' {op = its::BITOR;}) 
    right = bitxor
    {$ires = its::IntExpressionFactory::createBinary(
      op,
      $ires,
      $right.ires);
    }
  )*
;

bitxor returns [its::IntExpression ires]
@init{its::IntExprType op;}
:
  left = bit_and
  {$ires = $left.ires;}
  
  (('^' {op = its::BITXOR;}) 
    right = bit_and
    {$ires = its::IntExpressionFactory::createBinary(
      op,
      $ires,
      $right.ires);
    }
  )*
;

bit_and returns [its::IntExpression ires]
@init{its::IntExprType op;}
:
  left = bitshift
  {$ires = $left.ires;}
  
  (('&' {op = its::BITAND;}) 
    right = bitshift
    {$ires = its::IntExpressionFactory::createBinary(
      op,
      $ires,
      $right.ires);
    }
  )*
;

bitshift returns [its::IntExpression ires]
@init{its::IntExprType op;}
:
  left = addition
  {$ires = $left.ires;}
  
  (('<<' {op = its::LSHIFT;} | '>>' {op = its::RSHIFT;}) 
    right = addition
    {$ires = its::IntExpressionFactory::createBinary(
      op,
      $ires,
      $right.ires);
    }
  )*
;

addition returns [its::IntExpression ires]
@init{its::IntExprType op;}
:
  left = multiplication
  {$ires = $left.ires;}
  
  (('+' {op = its::PLUS;} | '-' {op = its::MINUS;}) 
    right = multiplication
    {$ires = its::IntExpressionFactory::createBinary(
      op,
      $ires,
      $right.ires);
    }
  )*
;

multiplication returns [its::IntExpression ires]
@init{its::IntExprType op;}
:
  left = unaryMinus
  {$ires = $left.ires;}
  
  (('*' {op = its::MULT;} | '/' {op = its::DIV;} | '%' {op = its::MOD;})
    right = unaryMinus
    {$ires = its::IntExpressionFactory::createBinary(
      op,
      $ires,
      $right.ires);
    }
  )*
;


unaryMinus returns [its::IntExpression ires] :
  (sign='-' value=power
   {$ires = its::IntExpressionFactory::createBinary(
      its::MINUS,
      0,
      $value.ires);}
  )
  |
  (value=power
  { $ires = $value.ires ;}
  )
; 
power returns [its::IntExpression ires]
@init{its::IntExprType op;}
:
  left = intPrimary
  {$ires = $left.ires;}
  
  (('**' {op = its::POW;})
    right = intPrimary
    {$ires = its::IntExpressionFactory::createBinary(
      op,
      $ires,
      $right.ires);
    }
  )*
;

intPrimary returns [its::IntExpression ires] :
  (intconst=INT
  {$ires = its::IntExpressionFactory::createConstant(toInt($intconst));}
  )
  |
  (current=varAccess
  {$ires = $current.ires;}
  )
/* ---------------- NOT IMPLEMENTED SECTION ----------------------
//Peek on lists
  |
  (listPeek
   {$ires = its::IntExpressionFactory::createConstant(0); }
  )
-----------------------------------------------------------------*/
  |
  (
    (
      ('(' bit_or ')') => 
      '(' current=bit_or ')'
      {$ires = $current.ires;}
    )
    |
    ('(' current=wrapBool {$ires = $current.ires;} ')'  )
  )
 
;

/* ---------------- NOT IMPLEMENTED SECTION ------------------
// Should return intExpression if used in C++.
listPeek : 
  qualifiedName '.peek' '(' ')'  
;
--------------------------------------------------------------*/


varAccess returns [its::IntExpression ires]:
  (
  vref=variableRef       {$ires = $vref.ires;} 
  | 
  aaccess=arrayVarAccess {$ires = $aaccess.ires;}
  )  
  ;

variableRef returns [its::IntExpression ires]:
  name=qualifiedName
  {
     checkVariableExistance($name.res) ;
  }
  {
  its::Variable v ($name.res);
  $ires = its::IntExpressionFactory::createVariable(v);
  }
;

arrayVarAccess returns [its::IntExpression ires] : 
  name=qualifiedName '[' index=bit_or ']' 
  {
    checkArrayExistance($name.res) ; 
  }
  {
    its::Variable v ($name.res);
    its::IntExpression _index ($index.ires) ; 
    $ires = its::IntExpressionFactory::createArrayAccess(v, _index) ;
    
  }
;

wrapBool returns [its::IntExpression ires] :
  current=boolOr
  {$ires = its::IntExpressionFactory::wrapBoolExpr($current.bres);}
;

/////////////////// utils & tokens
initValues returns [std::vector<int> vres] :
    val=integer 
    {vres.push_back($val.res);}
    (',' valSuite=integer
    {vres.push_back($valSuite.res);}
    )*
;

comparisonOperators returns [its::BoolExprType opres]:
  '>'
  {$opres = its::GT;}
  |
  '<'
  {$opres = its::LT;}
  |
  '>='
  {$opres = its::GEQ;}
  |
  '<='
  {$opres = its::LEQ;}
  |
  '=='
  {$opres = its::EQ;}
  |
  '!='
  {$opres = its::NEQ;}
;

qualifiedName returns [std::string res]:
  start=ID 
  {$res = toString($start);}
  
  (('.'next=ID
  {$res=$res + "."+ toString($next);})
  |('.'next=INT
  {$res=$res + "." + toString($next);})
  )*
;


integer returns [int res = 1] :
  (
  '-' { $res = -1 ;}
  )?
  value=INT
  { $res = $res * toInt($value); }
;

ID :
  ('a'..'z' |'A'..'Z' | '_')
  ('a'..'z' | 'A'..'Z' | '_' | '0'..'9')*
  ;

INT : ('0'..'9')+ ;

WS :
  (' ' | '\t' | '\r' | '\n' | '\u000C')+ {$channel = HIDDEN;}
  ;

STRING : ('"' ('\\' ('b'|'t'|'n'|'f'|'r'|'u'|'"'|'\''|'\\')|~(('\\'|'"')))* '"'|'\'' ('\\' ('b'|'t'|'n'|'f'|'r'|'u'|'"'|'\''|'\\')|~(('\\'|'\'')))* '\'');

ML_COMMENT : ('/*' ( options {greedy=false;} : . )*'*/') {$channel = HIDDEN;}; 

SL_COMMENT : ('//' ~(('\n'|'\r'))* ('\r'? '\n')?) {$channel = HIDDEN;};
  