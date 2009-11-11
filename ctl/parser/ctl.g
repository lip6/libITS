grammar ctl;

options
{
  language = C;
  backtrack = true;
  memoize = true;
}

@lexer::header
{
#define	ANTLR3_INLINE_INPUT_ASCII
}

@parser::includes
{
#include <iostream>
#include <boost/tr1/memory.hpp>
#include "ctl/formula.hh"
#include "ctl/parser/makers.hh"
#include "ctl/property.hh"
}

@members
{
  const ctl::property_factory* factory;
}

/***********************************************************************/

start 
[ ctl::formulae& res , const ctl::property_factory& pf ]
@init
{
  factory = &pf;
}
        :
        f=formula
        {
          res.push_back(*($f.res));
          delete $f.res;
        }
        ( ';' f=formula 
        {
          res.push_back(*($f.res));
          delete $f.res;
        }
        )* ';'?
        ;

/***********************************************************************/

formula
returns [ ctl::formula* res ]
@init
{
  typedef std::vector< std::pair< ANTLR3_UINT32 , ctl::formula* > > operands_type;
  operands_type operands;
}
        :
        
          t1=temporal 
        {
          operands.push_back(std::make_pair( AND , $t1.res ));
        }
          
          ( op=(AND|OR|XOR) t2=temporal 
        {
          operands.push_back( std::make_pair( $op.type , $t2.res ));
        }
          )+
        {
          ctl::formula* res_to_free;
          ctl::formula* res_to_free_2;
          
          operands_type::const_iterator cit = operands.begin();
          operands_type::const_iterator head = cit;
          ++cit;

          res_to_free   = head->second;
          res_to_free_2 = cit->second;

          switch( cit->first )
          {
            case AND:
              res = new ctl::formula( ctl::make_and( *(head->second) , *(cit->second) ) );
              break;
              
            case OR:
              res = new ctl::formula( ctl::make_or( *(head->second) , *(cit->second) ) );
              break;
              
            case XOR:
              res = new ctl::formula( ctl::make_xor( *(head->second) , *(cit->second) ) );            
              break;
          }
          
          delete res_to_free;
          delete res_to_free_2;
          
          for( ++cit ; cit != operands.end() ; ++cit )
          {
            res_to_free   = res;
            res_to_free_2 = cit->second;
            
            switch( cit->first )
            {
              case AND:
                res = new ctl::formula( ctl::make_and( *res , *(cit->second) ) );
                break;
                
              case OR:
                res = new ctl::formula( ctl::make_or( *res , *(cit->second) ) );
                break;
                
              case XOR:
                res = new ctl::formula( ctl::make_xor( *res , *(cit->second) ) );            
                break;
            }
            
            delete res_to_free;
            delete res_to_free_2;
          }
        }

        | lhs=temporal IMPLIES rhs=temporal
        {
          res = new ctl::formula(ctl::make_implies( *($lhs.res) , *($rhs.res) ));
          delete $lhs.res;
          delete $rhs.res;
        }

        | lhs=temporal EQ rhs=temporal
        {
          res = new ctl::formula(ctl::make_eq( *($lhs.res) , *($rhs.res) ));
          delete $lhs.res;
          delete $rhs.res;
        }

        | NOT temporal
        {
          res = new ctl::formula(ctl::make_not(*($temporal.res)));
          delete $temporal.res;
        }
        
        | temporal
        {
            res = $temporal.res;
        }
        
        ;

/***********************************************************************/

temporal
returns [ ctl::formula* res ]
        :
          atom
        {
          res = $atom.res;
        }

        | FORALL NEXT '(' atom ')'
        {
          res = new ctl::formula(ctl::make_AX( *($atom.res) ));
          delete $atom.res;
        }
        
        | EXISTS NEXT '(' atom ')'
        {
          res = new ctl::formula( ctl::make_EX( *($atom.res) ) );
          delete $atom.res;
        }
        
        | FORALL FINALLY '(' atom ')'
        {
          res = new ctl::formula( ctl::make_AF( *($atom.res) ) );
          delete $atom.res;
        }        
        
        | EXISTS FINALLY '(' atom ')'
        {
          res = new ctl::formula( ctl::make_EF( *($atom.res) ) );
          delete $atom.res;
        }        
        
        | FORALL GLOBALLY '(' atom ')'
        {
          res = new ctl::formula( ctl::make_AG( *($atom.res) ) );
          delete $atom.res;
        }        
        
        | EXISTS GLOBALLY '(' atom ')'
        {
          res = new ctl::formula( ctl::make_EG( *($atom.res) ) );
          delete $atom.res;
        }        
        
        | FORALL '[' lhs=atom UNTIL rhs=atom ']'
        {
          res = new ctl::formula(ctl::make_AU( *($lhs.res) , *($rhs.res) ));
          delete $lhs.res;
          delete $rhs.res;
        }
        
        | EXISTS '[' lhs=atom UNTIL rhs=atom ']'
        {
          res = new ctl::formula(ctl::make_EU( *($lhs.res) , *($rhs.res) ));
          delete $lhs.res;
          delete $rhs.res;
        }

        ;

/***********************************************************************/

atom
returns [ ctl::formula* res ]
        :
          PROPERTY
        {
          // temporary
          // we should count the number of parenthesis to
          // delimit a property
          std::string prop( (char*)($PROPERTY.text->chars) );
          prop = prop.erase( 0 , 1);
          prop = prop.erase( prop.size() - 1 , 1 );
          res = new ctl::formula( (*factory)( prop ) );
        }
        
        | '(' formula ')'
        {
          res = $formula.res;
        }
        
        | TRUE
        {
          res = new ctl::formula(ctl::make_constant(true));
        }
        
        | FALSE
        {
          res = new ctl::formula(ctl::make_constant(false));
        }
        
        ;

/***********************************************************************/

/* LEXER RULES */

/* LOGICAL OPERATORS*/
TRUE    : 'true';
FALSE   : 'false';
NOT     : 'not' ;
AND     : 'and';
OR      : 'or' ;
XOR     : 'xor' ;
IMPLIES : '=>' ;
EQ      : '<=>' ;

/* TEMPORAL OPERATORS */
FORALL    : 'A' ;
EXISTS    : 'E' ;
NEXT      : 'X' ;
FINALLY   : 'F' ;
GLOBALLY  : 'G' ;
UNTIL     : 'U' ;

// temporary, we should count the number of parenthesis to delimit a property
PROPERTY  :
          '#' .* '#'
          ;

WS  :   (' '|'\t'|'\n'|'\r')+ {$channel=HIDDEN;}
    ;
