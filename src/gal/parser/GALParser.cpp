#include "GALParser.hh"
#include "ITSModel.hh"

#include "gal/parser/exprParserLexer.h"
#include "gal/parser/exprParserParser.h"
#include <antlr3.h>

using std::cerr;
using std::endl;

namespace its {

  

  void GALParser::loadCGAL(Label pathinputff, ITSModel & model) {
       // opens the file as an input stream for the lexer
      pANTLR3_INPUT_STREAM input = antlr3FileStreamNew((pANTLR3_UINT8)(pathinputff.c_str()), 0);
      if (input == NULL) {
	cerr << "Unable to open the file of permutations : " << pathinputff << endl;
	exit(1);
      }
      
      // the lexer
      pexprParserLexer lexer = exprParserLexerNew(input);
      if (lexer == NULL) {
	cerr << "Unable to create the lexer for the GAL file" << endl;
	exit(1);
      }
    
      // the token stream produces by the lexer
      pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
      if (tstream == NULL) {
	cerr << "Unable to allocate token stream for GAL parsing" << endl;
	exit(1);
      }
    
      // the parser
      pexprParserParser parser = exprParserParserNew(tstream);
      if (parser == NULL) {
	cerr << "Unable to create the parser for the GAL" << endl;
	exit(1);
      }
    
      // do the parsing
      // The parse updates the provided model as needed
      parser->setModel (parser, & model);
      parser->specification(parser);

      if (parser->pParser->rec->state->errorCount > 0) {
	cerr << "The parser returned " << parser->pParser->rec->state->errorCount << " errors, parsing aborted" << endl;
	exit(1);
      }
    
      // free memory
      parser->free(parser); parser = NULL;
      tstream->free(tstream); tstream = NULL;
      lexer->free(lexer); lexer = NULL;
      input->close(input); input = NULL;
  }
  

  its::GAL * GALParser::loadGAL(const std::string & pathinputff) {
       // opens the file as an input stream for the lexer
      pANTLR3_INPUT_STREAM input = antlr3FileStreamNew((pANTLR3_UINT8)(pathinputff.c_str()), 0);
      if (input == NULL) {
	cerr << "Unable to open the file of permutations : " << pathinputff << endl;
	exit(1);
      }
      
      // the lexer
      pexprParserLexer lexer = exprParserLexerNew(input);
      if (lexer == NULL) {
	cerr << "Unable to create the lexer for the GAL file" << endl;
	exit(1);
      }
    
      // the token stream produces by the lexer
      pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
      if (tstream == NULL) {
	cerr << "Unable to allocate token stream for GAL parsing" << endl;
	exit(1);
      }
    
      // the parser
      pexprParserParser parser = exprParserParserNew(tstream);
      if (parser == NULL) {
	cerr << "Unable to create the parser for the GAL" << endl;
	exit(1);
      }
    
      // do the parsing
      GAL * result = parser->system(parser);
      if (parser->pParser->rec->state->errorCount > 0) {
	cerr << "The parser returned " << parser->pParser->rec->state->errorCount << " errors, parsing aborted" << endl;
	exit(1);
      }
    
      // free memory
      parser->free(parser); parser = NULL;
      tstream->free(tstream); tstream = NULL;
      lexer->free(lexer); lexer = NULL;
      input->close(input); input = NULL;

      return result;

  }
  
  BoolExpression
  GALParser::parsePredicate (Label pred, const its::GAL * const gal)
  {
    // to support old-fashioned syntax, first turn the '=' into '=='
    std::stringstream tmp;
    tmp << pred[0];
    for (size_t i=1 ; i < pred.size() -1; i++) {
      // '=' cannot be the first or last character of 'pred'
      // so that accessing pred[i+1] or pred[i-1] would fail only if 'pred' is not well-formed
      // if current character is a single '=', turn it into '=='
      if (pred[i] == '=' && pred[i+1] != '=' && pred[i-1] != '=' && pred[i-1] != '<' && pred[i-1] != '!' && pred[i-1] != '>')
        tmp << "==";
      else
        tmp << pred[i];
    }
    tmp << pred[pred.size()-1];
    std::string predicate = tmp.str ();
    
    // reads the string as an input stream for the lexer
    pANTLR3_INPUT_STREAM input = antlr3StringStreamNew((pANTLR3_UINT8)(predicate.c_str()), 0, predicate.size (), (pANTLR3_UINT8)"predicate");
    if (input == NULL) {
      std::cerr << "Unable to read predicate: " << predicate << std::endl;
      exit(1);
    }
    
    // the lexer
    pexprParserLexer lexer = exprParserLexerNew(input);
    if (lexer == NULL) {
      std::cerr << "Unable to create the lexer for the predicate" << std::endl;
      exit(1);
    }
    
    // the token stream produced by the lexer
    pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    if (tstream == NULL) {
      std::cerr << "Unable to allocate token stream for predicate parsing" << std::endl;
      exit(1);
    }
    
    // the parser
    pexprParserParser parser = exprParserParserNew(tstream);
    if (parser == NULL) {
      std::cerr << "Unable to create the parser for the predicate" << std::endl;
      exit(1);
    }
    
    // set the parsing context
    parser->setGAL (parser, gal);
    // do the parsing
    BoolExpression result = parser->boolOr (parser);
    if (parser->pParser->rec->state->errorCount > 0) {
      std::cerr << "The parser returned " << parser->pParser->rec->state->errorCount << " errors, parsing aborted" << std::endl;
      exit(1);
    }
    
    // free memory
    parser->free(parser); parser = NULL;
    tstream->free(tstream); tstream = NULL;
    lexer->free(lexer); lexer = NULL;
    input->close(input); input = NULL;
    
    return result;
  }

}
