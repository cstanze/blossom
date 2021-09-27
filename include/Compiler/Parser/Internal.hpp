

#ifndef COMPILER_PARSER_INTERNAL_HPP
#define COMPILER_PARSER_INTERNAL_HPP

#include "ParseHelper.hpp" // for all the parse function sources
#include "Stmts.hpp"

using namespace blossom;

Errors parse_block(ParseHelper &ph, StmtBase *&loc,
                   const bool with_brace = true);

Errors parse_expr_cols(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_cols_or_rbrace(
    ParseHelper &ph,
    StmtBase *&loc); // consider expression done on right brace
Errors parse_expr(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_16(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_15(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_14(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_13(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_12(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_11(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_10(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_09(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_08(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_07(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_06(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_05(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_04(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_03(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_02(ParseHelper &ph, StmtBase *&loc);
Errors parse_expr_01(ParseHelper &ph, StmtBase *&loc);
// make_const if true, changes type of token from IDEN to STR (useful for, say,
// after TOK_DOTs)
Errors parse_term(ParseHelper &ph, StmtBase *&loc,
                  const bool make_const = false);

Errors parse_var_decl(ParseHelper &ph, StmtBase *&loc);
Errors parse_var_decl_base(ParseHelper &ph, StmtBase *&loc);

Errors parse_import_decl(ParseHelper &ph, StmtBase *&loc);
Errors parse_import_decl_base(ParseHelper &ph, StmtBase *&loc);

Errors parse_impl_decl(ParseHelper &ph, StmtBase *&loc);
Errors parse_func_decl(ParseHelper &ph, StmtBase *&loc);
Errors parse_fn_decl_args(ParseHelper &ph, StmtBase *&loc);

Errors parse_fn_call_args(ParseHelper &ph, StmtBase *&loc);

Errors parse_single_operand_stmt(ParseHelper &ph, StmtBase *&loc);

Errors parse_conditional(ParseHelper &ph, StmtBase *&loc);

Errors parse_for(ParseHelper &ph, StmtBase *&loc);
Errors parse_foreach(ParseHelper &ph, StmtBase *&loc);
Errors parse_while(ParseHelper &ph, StmtBase *&loc);

#endif // COMPILER_PARSER_INTERNAL_HPP
