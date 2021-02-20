#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif

// ................................................... save token info

static
void parserSaveToken( ptoken_t tSave , pparser_t this )
{
	tSave->sym 	= this->lexer->sym 				;
	tSave->row 	= this->lexer->row_start 		;
	tSave->col 	= this->lexer->col_start 		;
	tSave->token = gcWcsDup(this->lexer->token) ;
}
static
void parserRestoreToken( node_t* tRestore , token_t tSave)
{
	tRestore->row 	= tSave.row  				;
	tRestore->col 	= tSave.col-1 				;
	tRestore->token = tSave.token 				;
}

// ................................................... private prefix in expr

static
psPrefixOp_t    parserPrefixNew( plexer_t lexer )
{
    psPrefixOp_t    prefix    = gcMalloc( sizeof(struct sPrefixOp_s) ) ;
    
    if ( prefix == NULL )
    {
         $astInternal ( malloc , outOfMemory , L"expr.c" , L"parserMakePrefix") ;
    }
    else
    {
        prefix->sym       		= lexer->sym ;
        prefix->row_start  		= lexer->row_start ;
        prefix->col_start  		= lexer->col_start ;
        prefix->token      		= gcWcsDup( lexer->token ) ;
        prefix->fileInputName	= gcWcsDup( (wchar_t*)lexer->fileInputName ) ;
    }
    return prefix ;
}

static
psPrefixOp_t    parserPrefixDelete( psPrefixOp_t prefix )
{
    if ( prefix != NULL )
    {
         gcFree(prefix->token);
         prefix = NULL ;
    }
    return prefix ;
}

static 
node_t* parserPostFixDot( pparser_t this , node_t* left )
{
	node_t* vStruct = astMakeNodeBlock( this->ast ) ;
	node_t* exprNode  = NULL ;

	size_t rowSave = this->lexer->row_start;
	size_t colSave = this->lexer->col_start;

	// .

	do
	{
		parserGetToken(this);
		// expr
		exprNode = parserExpr(this);
		// .
		astPushNodeBlock( this->ast , vStruct , exprNode ) ;

	} while ( 
			(this->lexer->sym == sym_dot	)	//	.
	) ;

	if ( vStruct->block.next.size ) // è presente vettore array
	{
		// crea un nodo post fisso ed inserisce operatore post fisso ( con riferimento al blocco lista parametri
		
		node_t* node = astMakeNodePostfix( this->ast , this->lexer , left ) ;
		
		node->postfix.sym  	 	= sym_dot ; 			// operatore post fisso .
		node->token 		 	= gcWcsDup(L".");  
		node->postfix.vStruct   = vStruct ;
		node->row		 	 	= rowSave ;
		node->col			 	= colSave -1 ;

		return node ;
	}
	return left ;
}


static 
node_t* parserPostFixParamSub( pparser_t this , node_t* left )
{
	node_t* vParamSub = astMakeNodeBlock( this->ast ) ;
	node_t* exprNode  = NULL ;

	size_t rowSave = this->lexer->row_start;
	size_t colSave = this->lexer->col_start;

	// (

	do
	{
		parserGetToken(this);
		// expr
		exprNode = parserExpr(this);
		// ,
		astPushNodeBlock( this->ast , vParamSub , exprNode ) ;

	} while ( this->lexer->sym == sym_v ) ;
	
	$MATCH(  sym_p1 , L')');

	if ( vParamSub->block.next.size ) // è presente vettore array
	{
		// crea un nodo post fisso ed inserisce operatore post fisso ( con riferimento al blocco lista parametri
		
		node_t* node = astMakeNodePostfix( this->ast , this->lexer , left ) ;
		
		node->postfix.sym  	 = sym_p0 ; 			// operatore post fisso /
		node->token 		 = gcWcsDup(L"(");  
		node->postfix.param  = vParamSub ;
		node->row		 	 = rowSave ;
		node->col			 = colSave -1 ;

		return node ;
	}
	return left ;
}

static 
node_t* parserPostFixArray( pparser_t this , node_t* left )
{
	node_t* vArray = astMakeNodeBlock( this->ast ) ;
	node_t* exprNode  = NULL ;

	size_t rowSave = this->lexer->row_start;
	size_t colSave = this->lexer->col_start;

	while ( this->lexer->sym == sym_pq0 )
	{
		parserGetToken(this);
		
		exprNode = parserExpr(this);
		
		astPushNodeBlock( this->ast , vArray , exprNode ) ;
		
		$MATCH(  sym_pq1 , L']' ) ;
	}

	if ( vArray->block.next.size ) // è presente vettore array
	{
		// crea un nodo post fisso ed inserisce operatore post fisso [ con riferimento al blocco array
		node_t* node = astMakeNodePostfix( this->ast , this->lexer , left ) ;
		
		node->postfix.sym  	 = sym_pq0 ; 			// operatore post fisso [
		node->token 		 = gcWcsDup(L"[");  
		node->postfix.array  = vArray ;
		node->row		 	 = rowSave ;
		node->col			 = colSave -1 ;

		return node ;
	}
	return left ;
}

static 
node_t* parserPostFix( pparser_t this , node_t* n )
{
	//
	// lvalue required as postfix operand -> assembler
	//
	// ++ -- [] () .

	switch ( this->lexer->sym )
	{
		case sym_inc :
			n = astMakeNodePostfix( this->ast , this->lexer , n );		
			parserGetToken(this);
		break ;
		
		case sym_dec :
			n = astMakeNodePostfix( this->ast , this->lexer , n );	;
			parserGetToken(this);
		break ;
		
		case sym_pq0 :
			n= parserPostFixArray( this , n ) ; // n = [ E
			// esce con nuovo token
		break ;
		
		case sym_p0 :
			n= parserPostFixParamSub( this , n ) ; // n = ( E
			// esce con nuovo token
		break ;	

		case sym_dot :
		//case sym_ptr :
			n = parserPostFixDot( this , n ); // n = E .	
			// esce con nuovo token			
		break ;	

		
		default:
		break;							
	}
	
	return n;
}

// ................................................... TERM

node_t* parserTerm( pparser_t this )
{
	// ................................................... EXPR[2]
	
    // PREFIX PUSH

		stackTypeDef(psPrefixOp_t,sPrefixOp);
		stack_sPrefixOp_t	sPrefixOp ;
		stackAlloc(sPrefixOp,16) ;

        while 
        (		this->lexer->sym == sym_add		//	+
			||	this->lexer->sym == sym_sub		//	-
			||	this->lexer->sym == sym_not		//	!
			||	this->lexer->sym == sym_bitAnd  //	&  
			||	this->lexer->sym == sym_neg  	//	~  
			||	this->lexer->sym == sym_mul  	//	*
			||	this->lexer->sym == sym_inc  	//	++
			||	this->lexer->sym == sym_dec  	//	--
			||	this->lexer->sym == sym_sizeof  //	sizeof $                                                                       
        )
        {
            if ( this->lexer->sym != sym_add ) // prefix -> skip '+'
            {
                if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserPrefix push [%d]\n",this->lexer->sym );
                
                psPrefixOp_t pprefix = parserPrefixNew( this->lexer ) ;
                
                stackPush ( sPrefixOp , pprefix  ) ;
            }
            parserGetToken(this);
        }

    // END PREFIX PUSH

    node_t* n=NULL ;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserTerm\n" );

    switch ( this->lexer->sym )
    {
        case sym_p0 :

            if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parser Match ( .\n" );

            $MATCH( sym_p0, L'(' ) ;

            n=parserExpr(this);

            $MATCH( sym_p1, L')' ) ;

            if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parser Match ) . \n" );

        break ;

        case sym_integer :

            fwprintf ( this->pFileOutputParser , L"%-30ls :: [%lld].\n" ,L"sym_integer",this->lexer->value.integer );

            n = astMakeNodeTermInteger( this->ast,this->lexer,this->lexer->value.integer ) ;

            parserGetToken(this);

        break;

        case sym_real:

            fwprintf ( this->pFileOutputParser , L"%-30ls :: [%lf].\n",L"sym_real",this->lexer->value.real );

            n = astMakeNodeTermReal( this->ast,this->lexer,this->lexer->value.real ) ;

            parserGetToken(this);

        break;

        case sym_char :

            fwprintf ( this->pFileOutputParser , L"%-30ls :: [%lc].\n",L"sym_char",g.outputSpecialCharInChar(this->lexer->value.wchar) ) ;

            n = astMakeNodeTermChar( this->ast,this->lexer, this->lexer->value.wchar ) ;

            parserGetToken(this);

        break;

        case sym_string :

            fwprintf ( this->pFileOutputParser , L"%-30ls :: [%ls].\n",L"sym_string",g.outputSpecialCharInString(this->lexer->value.wstring) );

            n = astMakeNodeTermString( this->ast,this->lexer, this->lexer->value.wstring ) ;

            parserGetToken(this);

        break;
        

        case sym_id :
        
            fwprintf ( this->pFileOutputParser , L"%-30ls :: [%ls].\n",L"sym_string",g.outputSpecialCharInString(this->lexer->value.id) );

            n = astMakeNodeTermID( this->ast,this->lexer, this->lexer->value.id ) ;

            parserGetToken(this);
            
        break;

        default:
            
            /*
               Le espressioni come le frasi del parser possono ritornare NULL, 
               come possiamo vedere nella definizione degli array C : int a[] ...
               per tanto andra' gestita l'espressione NULL e con quanto ne consgue. 
            */
            
            // ! $syntaxError

            n=NULL ; // non trova espressione allora ritorna NULL !
            
        break;
    }

    // PREFIX POP

        while(stackSize(sPrefixOp))
        {
            if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserPrefix pop [%d]\n",this->lexer->sym );
            
            n = astMakeNodePrefix( this->ast,stackTop(sPrefixOp) , n ) ;
            
            parserPrefixDelete(stackTop(sPrefixOp));
            
            stackPop(sPrefixOp);
        }

    // END PREFIX POP

	// POSTFIX

	if ( n != NULL )	// se c'è un terminale
	{
		n=parserPostFix(this,n);
	}
	
	// END POSTFIX


 return n ;
}

// ................................................... EXPR[3]	mul div mod * / % 

node_t* parserMulDivMod( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserMulDivMod\n" );

    left=parserTerm(this);

    while (     this->lexer->sym == sym_mul
        || 		this->lexer->sym == sym_div
        ||    	this->lexer->sym == sym_mod
    )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;

        node_t *right=NULL;

        parserGetToken(this);

        right=parserTerm(this);
        
        if ( ( tSave.sym  == sym_div ) || ( tSave.sym  == sym_mod ) )
        {
            if ( right->type == nTypeTermInteger) 
                if ( right->term.integer == 0 )
                    $parserError( parseExpr , division_by_zero );

            if ( right->type == nTypeTermReal ) 
                if ( right->term.real == 0.0 )
                    $parserError( parseExpr , division_by_zero );

        } ;
        
        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym  , right,left ) ;
        
        parserRestoreToken( left , tSave ) ;
    } 

 return left ;
}

// ................................................... EXPR[4]	Add Sub + - 

node_t* parserAddSub( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserAddSub\n" );

    left=parserMulDivMod(this);

    while (	this->lexer->sym == sym_add
        ||	this->lexer->sym == sym_sub
    )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
     
        node_t *right=NULL;

        parserGetToken(this);

        right=parserMulDivMod(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym  , right,left ) ;
        
        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[5]	 << >>

node_t* parserShiftLR( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserShiftLR\n" );

    left=parserAddSub(this);

    while (	this->lexer->sym == sym_shiftLeft
        ||	this->lexer->sym == sym_shiftRight
    )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
  
        node_t *right=NULL;

        parserGetToken(this);

        right=parserAddSub(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym  , right,left ) ;
        
        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[6]	 < <= > >=

node_t* parserOpRelLG( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserOpRelLG\n" );

    left=parserShiftLR(this);

    while (	this->lexer->sym == sym_lt
		||	this->lexer->sym == sym_le
		||	this->lexer->sym == sym_gt
		||	this->lexer->sym == sym_ge
    )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserShiftLR(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[7]	 ==/?= !=

node_t* parserOpRelEqNe( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserOpRelEqNe\n" );

    left=parserOpRelLG(this);

    while (	this->lexer->sym == sym_eq
		||	this->lexer->sym == sym_ne
    )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserOpRelLG(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[8]	 &

node_t* parserBitAnd( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserBitAnd\n" );

    left=parserOpRelEqNe(this);

    while (	this->lexer->sym == sym_bitAnd )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserOpRelEqNe(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[9]	 |

node_t* parserBitOr( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserBitOr\n" );

    left=parserBitAnd(this);

    while (	this->lexer->sym == sym_bitOr )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserBitAnd(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[10]	 ^

node_t* parserBitXor( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserBitXor\n" );

    left=parserBitOr(this);

    while (	this->lexer->sym == sym_bitXor )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserBitOr(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[11]	 &&

node_t* parserAnd( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserAnd\n" );

    left=parserBitXor(this);

    while (	this->lexer->sym == sym_and )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserBitXor(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[12]	 ||

node_t* parserOr( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserOr\n" );

    left=parserAnd(this);

    while (	this->lexer->sym == sym_or )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserAnd(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[12b]	^^ precedence less than != even if is the same thing

node_t* parserXor( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserXor\n" );

    left=parserOr(this);

    while (	this->lexer->sym == sym_xor )
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        node_t *right=NULL;

        parserGetToken(this);

        right=parserOr(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,tSave.sym , right,left ) ;

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR[13]	? : 

node_t* parserTerOp( pparser_t this )
{
    node_t *cond=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserTerOp\n" );

    cond=parserXor(this);

    while (	this->lexer->sym == sym_qm )
    {
		node_t* left  = NULL ;
		node_t* right = NULL ;
		
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
   
        parserGetToken(this);

        left=parserExpr(this);
        
		$MATCH( sym_dp, L':' ) ;

        right=parserExpr(this);
                		
        cond = astMakeNodeTerOP( this->ast,this->lexer,tSave.sym , cond, left,right ) ;

        parserRestoreToken( cond , tSave ) ;
    } ;
 
 return cond ;
}

// ................................................... EXPR[14]	Assign := += -= *= /= %= <<= >>= &= |= ^=

node_t* parserAssign( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserAssign\n" );

    left=parserTerOp(this);

    while ( this->lexer->sym == sym_assign 
		||	this->lexer->sym == sym_addEq
		||	this->lexer->sym == sym_mulEq
		||	this->lexer->sym == sym_divEq
		||	this->lexer->sym == sym_modEq	
		||	this->lexer->sym == sym_shiftLeftEq
		||	this->lexer->sym == sym_shiftRightEq
		||	this->lexer->sym == sym_bitAndEq
		||	this->lexer->sym == sym_bitOrEq
		||	this->lexer->sym == sym_bitXorEq				 
	)
    {
        token_t tSave ;
        parserSaveToken( &tSave , this ) ;
           
        node_t *right=NULL;

        parserGetToken(this);

        right=parserTerOp(this);

        left = astMakeNodeAssign( this->ast,this->lexer,left,right ) ; // invertiti

        parserRestoreToken( left , tSave ) ;
    } ;

 return left ;
}

// ................................................... EXPR

node_t* parserExpr( pparser_t this )
{
    if ( this->lexer->sym == sym_end ) return NULL ;

    node_t *n=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserExpr\n" );

    n=parserAssign(this);

 return n ;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif



/**/


