#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"


/*
	
	:: id
	{
		parserDeclaration
		
		parserStatement
	}

*/

node_t*  parserStatSub( pparser_t this )
{
	node_t *n	 	= NULL;
	node_t *vBlock	= astMakeNodeBlock(this->ast);
	node_t *block	= NULL ;
		
	if ( this->lexer->sym == sym_scope )	//	::
	{
		parserGetToken(this);
		if ( this->lexer->sym == sym_id )	//	id
		{
			wchar_t* idSave = gcWcsDup(this->lexer->token);
			parserGetToken(this);

			$MATCH(sym_pg0,L'{');

			block=parserProgram(this,vBlock);

			if ( this->lexer->sym == sym_pv ) parserGetToken(this); // skip
			
			$MATCH(sym_pg1,L'}');
			
			n=astMakeStatSub(this->ast,this->lexer,idSave,block);			
		}
		else
		{
			$syntaxError;
			return NULL ;
		}		
	}

	return n ;
}

/**/



