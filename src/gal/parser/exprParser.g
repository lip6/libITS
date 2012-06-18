grammar exprParser;

options {
  language = C;
}

@parser::header {

#include "gal/GAL.hh"

}


@members {
	
	/// gets the text of a token and returns it as a string
	static std::string toString(pANTLR3_COMMON_TOKEN i) {
		return std::string((const char *)i->getText(i)->chars);
	}

  its::GAL * result;
}

system returns [its::GAL * r] : 'GAL' name=ID 
	{ 
	  result = new its::GAL ( "name" );
	}
	'{' decl=declaration* 
	transition* '}'	
	{
	 $r = result;
	}
	;

declaration : 'int' name=ID '=' val=INT ;

transition : 'transition' name=ID '{' action* '}';

action : var=ID  '=' val= INT ';' ;

ID : 'S' ;

INT : '0';

