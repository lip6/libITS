grammar exprParser;

/* 
 * GAL Antlr Grammar for C++
 */
options {
  language = C;
  memoize = true;
}

@parser::header {

  #include "its/gal/GAL.hh"
  #include "its/composite/Composite.hh"
  #include "its/ITSModel.hh"
  #include <iostream>
  #include <cstdlib>
}

@members {

  // "result" is the root of GAL elements.
  its::ITSModel * model = NULL;
  // "result" is the root of GAL elements.
  its::GAL* result = NULL;
  // pointer on the current transition (GuardedAction) found
  its::GuardedAction* current_ga = NULL ;
  
  /// Gets the text of a token and returns it as a string
  static std::string toString(pANTLR3_COMMON_TOKEN i) {
    return std::string((const char *)i->getText(i)->chars);
  }

  static std::string toStringTok(pANTLR3_COMMON_TOKEN i) {
    char buff [2048];
    strncpy(buff,((const char *)i->getText(i)->chars) + 1,2047);
    buff[2047] = '\0';
    buff[strlen(buff)-1]='\0';
    return std::string(buff);
  }
  
  // Returns an integer from a token, expected as a string of int.
  static int toInt(pANTLR3_COMMON_TOKEN i) {
    int n = atoi((const char *)i->getText(i)->chars);
    return n;
  }
  
  // Check if there is a variable named "str" in GAL system.  
  void checkVariableExistance(std::string str)
  {
	  assert (result != NULL);
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
  	assert (result != NULL);
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


  // Grab the size of the Array in GAL def, supposed to be called after above checkExistence.
  int getArraySize(std::string str)
  {
  	assert (result != NULL);
    for (its::GAL::arrays_it v = result->arrays_begin() ; 
                           v != result->arrays_end() ; 
                           v++) 
    {
       if(v->getName() == str)
       {
           return v->size();
       }
    }
    return -1;
  }


}

/// This is a hack to get around the ANTLR hiding all "members" inside the .cpp
/// Sets the result
/// It allows to set a context (an already built GAL) in order to parse a single IntExpr or BoolExpr in that context
setGAL[const its::GAL * g]: { result = const_cast<its::GAL *> (g); } ;
setModel[const its::ITSModel * g]: { model = const_cast<its::ITSModel *> (g); } ;

specification : 
	(
	gal=system { model->declareType(* gal);  }
	| comp=composite { model->declareType(* comp);  }
	) *
	'main' name=qualifiedName { model->setInstance($name.res,"main"); }
		( 
		  ( '(' initState=qualifiedName {}  ')' { model->setInstanceState($initState.res); }  )
		  | { model->setInstanceState("init"); }
		) ';'
;

composite returns [its::Composite * r] 
@init{r=NULL;} :
	'composite' cname=qualifiedName 
		{
		  $r = new its::Composite ( $cname.res ) ;
		}
	'{' 
		(
		(  
		type=qualifiedName instName=qualifiedName  ';'
		{
			$r -> addInstance($instName.res , $type.res, *model);
			$r -> updateStateDef ("init",$instName.res,"init"); 
		}
		
		) | (
		type=qualifiedName '[' size=integer ']'  instName=qualifiedName ';'
		{
		for (int i=0; i < $size.res ; i++) {
			$r -> addInstance($instName.res + "["+ to_string(i)+ "]" , $type.res, *model);
			$r -> updateStateDef ("init",$instName.res + "["+ to_string(i)+ "]" ,"init");
		}			
		}
		)
		 )*
		( 
		'synchronization' name=qualifiedName 'label' label=STRING 
		{
			$r -> addSynchronization($name.res,toStringTok($label));
		}
		'{'
			(
			instance=qualifiedName '.' slabel=STRING ';'
			{
				$r -> addSyncPart ($name.res, $instance.res, toStringTok($slabel));
			}
			| ( 'self' '.' slabel=STRING ';'  
                  {
				$r -> addSyncPart ($name.res, "self", toStringTok($slabel));
                    }  
           ) | (
			instance=qualifiedName '[' index=integer ']' '.' slabel=STRING ';'
			{
				$r -> addSyncPart ($name.res, $instance.res + "["+ to_string($index.res) + "]", toStringTok($slabel));
			}	
			)
			)*
		'}'
		)*
	'}' 
;

system returns [its::GAL* r] 
@init{r = NULL;} :
  ('gal'|'GAL') name=qualifiedName
  
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


transition : 
  'transition' name=qualifiedName 
  '[' gard=boolOr ']'
  {
  current_ga = new its::GuardedAction($name.res);
  current_ga->setGuard($gard.bres);
  }
  
  ('label' label=STRING
  {
   current_ga->setLabel( toStringTok($label) );
  }
  )?
  
  trbody=body
	{
	  current_ga->getAction() = $trbody.seq ;
		
	  result->addTransition(*current_ga);
	  delete current_ga ; 
  }

  ;

body returns [its::Sequence seq ] 
@init{int op=0;}
:	
'{'
(
 // Normal actions (assignments)
 (
  var=varAccess  ('=' { op=0 ; } | '+=' { op =1;} | '-=' {op=2;}) val=bit_or ';'
 {
   if (op==0) {
              $seq.add(its::Assignment($var.ires, $val.ires));
   } else if (op==1) { 
              $seq.add(its::IncrAssignment($var.ires, $val.ires));
   } else {
           $seq.add(its::IncrAssignment($var.ires, its::IntExpressionFactory::createBinary(its::MINUS, 0, $val.ires)));
   }
 }
  )
 |
 (
  'self' '.' lab=STRING ';'
 {
   $seq.add(  its::Call(toStringTok($lab)) );
 }
  )
 |
 (
  'abort' ';'
 {
   $seq.add(  its::AbortStatement() );
 }
  )
 |
 (
  ifthenelse=iteAction
 {
   $seq.add($ifthenelse.ite);
 }
  )
 |
 (
  'fixpoint' fixBody=body
 {
   $seq.add(its::FixStatement( $fixBody.seq));
 }
  )
 )*
'}'
;

iteAction returns  [ its::Ite ite ] :
  'if' '(' cond=boolOr ')' iftrue=body
 {
   $ite = its::Ite($cond.bres, $iftrue.seq );
 }
  ( 'else' iffalse=body 
  {
    $ite.getIfFalse() = $iffalse.seq;
  }
    )? 
;

/* --------------- NOT IMPLEMENTED SECTION -------------------------
   |
   (
   listPop | listPush
   )
   ------------------------------------------------------------------*/



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
  'false'
  {$bres = its::BoolExpressionFactory::createConstant(false);}
  | 
  'true'
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
@init{its::IntExprType op = its::BITAND;}
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
@init{its::IntExprType op=its::LSHIFT;}
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
@init{its::IntExprType op = its::PLUS;}
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
@init{its::IntExprType op = its::MULT;}
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
  ('-' value=power
   {$ires = its::IntExpressionFactory::createBinary(
      its::MINUS,
      0,
      $value.ires);}
  )
  |
  ('~' value=power
   {$ires = its::IntExpressionFactory::createUnary(
      its::BITCOMP,
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
    $ires = its::IntExpressionFactory::createArrayAccess(v, _index, getArraySize($name.res)) ;
    
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

comparisonOperators returns [its::BoolExprType opres]
@init{opres= its::EQ;}
:

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
  
  (
  '.'
  ((next=ID
  {$res=$res + "."+ toString($next);}
  )
  |
  (next=INT
  {$res=$res + "." + toString($next);}
  ))
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
  
