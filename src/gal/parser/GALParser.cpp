#include "GALParser.hh"
#include "gal/parser/exprParserLexer.h"
#include "gal/parser/exprParserParser.h"
#include <antlr3.h>

using std::cerr;
using std::endl;

namespace its {

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

}
