#ifndef cdToken
#define cdToken

#include "../lib/cxx.h"
 
typedef enum e_sym
{
    sym_end         ,   // 0
    sym_weof        ,   // 1
    sym_char        ,   // 2    //    chracter C ''    wchar_t
    sym_string      ,   // 3    //    string   C ""    wchar_t*
    sym_id          ,   // 4
    sym_integer     ,   // 5
    sym_real        ,   // 6
    
    //              keyword    
    
    sym_kw=100      ,
    sym_kw_while    ,   // +1
    sym_kw_for      ,   // +2
    sym_kw_const    ,   // +3
    sym_kw_var      ,   // +4
    sym_kw_integer  ,   // +5  
    sym_kw_real     ,   // +6      
    sym_kw_char     ,   // +7
    sym_kw_byte     ,   // +8    
    sym_kw_array    ,   // +9  
    sym_kw_type     ,   // +10      
    sym_kw_function ,   // +11   
                  
    //              special operator  
    sym_op0=150    , 
          
    sym_p0         ,    // +1    )
    sym_p1         ,    // +2    (
    sym_pv         ,    // +3    ;
    sym_v          ,    // +4    ,
    sym_dp         ,    // +5    :
    sym_pq0        ,    // +6    [
    sym_pq1        ,    // +7    ]
    sym_pg0        ,    // +8    {
    sym_pg1        ,    // +9    }  
    sym_diesis	   ,    // +10   #  
       
    //              operator
    
    sym_op2=200    ,    
 
    //              operator 3
     
    sym_shiftLeftEq		,
    sym_shiftRightEq	,
        
    //              operator 2
        
    sym_assign		,    // :=	assign
    sym_eq			,    // ?=	is equal
    sym_scope		,    // ::	scope resolution
	sym_inc			,    // ++	increment (next)
	sym_dec      	,    // --  decrement (prev)
	sym_ptr      	,    // ->  pointer access
	sym_shiftLeft	,    // <<  shift left
	sym_shiftRight	,    // >>  shift right
	sym_le			,    // <=  less equal
	sym_ge			,    // >=  greater equal
	sym_ne			,    // !=  not equal
	sym_and			,    // &&	and
	sym_or			,    // ||	or
	sym_xor			,    // ^^	xor

	sym_addEq		,    // +=
	sym_subEq		,    // -=
	
	sym_mulEq		,    // *=
	sym_divEq		,    // /=
	sym_modEq		,    // %=

	sym_bitAndEq	,    // &=
	sym_bitOrEq		,    // |=
	sym_bitXorEq	,    // ^=
										                     
    //              operator 1
    
    sym_op1=250    ,
    
    sym_not        ,   // +1 prefix         !    
         
    sym_add        ,   // +2 binop/prefix   +
    sym_sub        ,   // +3 binop/prefix   -
   
    sym_mul        ,   // +4 binop          * / %
    sym_div        ,   // +5
    sym_mod        ,   // +6

    sym_dot			,	// +7 				.				.
    sym_neg			,   // +8				~	        
	sym_sizeof		,	// +9				§
	sym_lt			,	// +10				>
	sym_gt			,	// +11				<

	sym_bitAnd		,	// +12				&
	sym_bitOr		,	// +13				|
	sym_bitXor		,	// +14				^

	sym_qm			,	// +15				?	Question Mark
						        
    //
    
    sym_lexema      ,  // carattere non riconosciuto
      
} sym_t ; 



#endif

 
 
/**/


