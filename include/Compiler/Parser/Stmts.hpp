

#ifndef COMPILER_PARSER_STMTS_HPP
#define COMPILER_PARSER_STMTS_HPP

#include <vector>

#include "../../Common/IO.hpp" // for disp() functions
#include "../../VM/OpCodes.hpp"
#include "../Lex.hpp"

enum GramType {
  GT_SIMPLE,
  GT_EXPR,
  GT_BLOCK,
  GT_VAR_DECL_BASE,
  GT_VAR_DECL,
  GT_FN_ASSN_ARG,
  GT_FN_ARGS,
  GT_FN_DEF,
  GT_SINGLE_OPERAND_STMT,
  GT_CONDITIONAL,
  GT_FOR,
  GT_FOREACH,
  GT_WHILE,
};

class StmtBase {
  size_t m_idx;
  GramType m_type;

public:
  StmtBase(const GramType type, const size_t &idx);
  virtual ~StmtBase();

  virtual void disp(const bool has_next) const = 0;

  // flags are used when necessary
  // f1 = flag1, f2 = flag2
  virtual bool codegen(Bytecode &bc) const = 0;

  size_t idx() const;
  GramType type() const;
};

class StmtSimple : public StmtBase {
  const lex::tok_t *m_val;

public:
  StmtSimple(const lex::tok_t *val);

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const lex::tok_t *val() const;
};

class StmtBlock : public StmtBase {
  std::vector<const StmtBase *> m_stmts;
  bool m_no_brace;

public:
  StmtBlock(const std::vector<const StmtBase *> &stmts, const size_t &idx);
  ~StmtBlock();

  void set_no_brace(const bool &no_brace);

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const std::vector<const StmtBase *> &stmts() const;
  const bool &no_brace() const;
  void clear_stmts();
};

typedef StmtBlock ParseTree;

class StmtExpr : public StmtBase {
  const StmtBase *m_lhs, *m_rhs;
  const lex::tok_t *m_oper;
  StmtBase *m_or_blk;
  const lex::tok_t *m_or_blk_var;
  size_t m_commas;
  bool m_with_cols;

public:
  StmtExpr(const StmtBase *lhs, const lex::tok_t *oper, const StmtBase *rhs,
           const size_t &idx);
  ~StmtExpr();

  void set_or_blk(StmtBase *or_blk, const lex::tok_t *or_blk_var);
  void set_with_cols(const bool &with_cols);

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const StmtBase *lhs() const;
  const StmtBase *rhs() const;
  const lex::tok_t *oper() const;
  const StmtBase *or_blk() const;
  const lex::tok_t *or_blk_var() const;
  size_t commas() const;
  const bool &with_cols() const;
  void commas_set(const size_t &commas);
};

class StmtVarDeclBase : public StmtBase {
  const StmtSimple *m_lhs;
  const StmtBase *m_in;
  const StmtBase *m_rhs;

public:
  StmtVarDeclBase(const StmtSimple *lhs, const StmtBase *in,
                  const StmtBase *rhs);
  ~StmtVarDeclBase();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const StmtSimple *lhs() const;
  const StmtBase *in() const;
  const StmtBase *rhs() const;

  bool has_in() const;
};

class StmtVarDecl : public StmtBase {
  const std::vector<const StmtVarDeclBase *> m_decls;

public:
  StmtVarDecl(const std::vector<const StmtVarDeclBase *> &decls,
              const size_t &idx);
  ~StmtVarDecl();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const std::vector<const StmtVarDeclBase *> &decls() const;
};

class StmtFnDefArgs : public StmtBase {
  const std::vector<const StmtBase *> m_args;
  const StmtSimple *m_kwarg, *m_vaarg;

public:
  StmtFnDefArgs(const std::vector<const StmtBase *> &args,
                const StmtSimple *kwarg, const StmtSimple *vaarg,
                const size_t &idx);
  ~StmtFnDefArgs();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const std::vector<const StmtBase *> &args() const;
  const StmtSimple *kwarg() const;
  const StmtSimple *vaarg() const;
};

class StmtFnDef : public StmtBase {
  const StmtFnDefArgs *m_args;
  const StmtBlock *m_body;
  const StmtSimple *m_name;
  const bool m_isAnon;

public:
  StmtFnDef(const StmtFnDefArgs *args, const StmtBlock *body,
            const StmtSimple *name, const bool isAnon, const size_t &idx);
  ~StmtFnDef();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const StmtFnDefArgs *args() const;
  const StmtBlock *body() const;
};

class StmtImplDef : public StmtBase {
  const StmtFnDefArgs *m_args;
  const StmtBlock *m_body;
  const StmtSimple *m_in;
  const StmtSimple *m_name;

public:
  StmtImplDef(const StmtFnDefArgs *args, const StmtBlock *body,
              const StmtSimple *in, const StmtSimple *name, const size_t &idx);
  ~StmtImplDef();

  void disp(const bool hasNext) const;
  bool codegen(Bytecode &bc) const;

  const StmtFnDefArgs *args() const;
  const StmtBlock *body() const;
  const StmtSimple *in() const;
};

class StmtFnAssnArg : public StmtBase {
  const StmtSimple *m_lhs;
  const StmtBase *m_rhs;

public:
  StmtFnAssnArg(const StmtSimple *lhs, const StmtBase *rhs);
  ~StmtFnAssnArg();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const StmtSimple *lhs() const;
  const StmtBase *rhs() const;
};

class StmtFnCallArgs : public StmtBase {
  const std::vector<const StmtBase *> m_args;
  const std::vector<const StmtFnAssnArg *> m_assn_args;
  const bool m_va_unpack;

public:
  StmtFnCallArgs(const std::vector<const StmtBase *> &args,
                 const std::vector<const StmtFnAssnArg *> &assn_args,
                 const bool &va_unpack, const size_t &idx);
  ~StmtFnCallArgs();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const std::vector<const StmtBase *> &args() const;
  const std::vector<const StmtFnAssnArg *> &assn_args() const;
  const bool &va_unpack() const;
};

/*
enum SingleOperandStmtType
{
        SOST_RETURN,
        SOST_CONTINUE,
        SOST_BREAK,
};
*/
class StmtSingleOpStmt : public StmtBase {
  // SingleOperandStmtType
  const lex::tok_t *m_sost;
  const StmtBase *m_operand;

public:
  StmtSingleOpStmt(const lex::tok_t *sost, const StmtBase *operand);
  ~StmtSingleOpStmt();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const lex::tok_t *sost() const;
  const StmtBase *operand() const;
};

struct conditional_t {
  size_t idx;
  StmtBase *condition;
  StmtBase *body;
};

class StmtConditional : public StmtBase {
  const std::vector<conditional_t> m_conds;

public:
  StmtConditional(const std::vector<conditional_t> &conds, const size_t &idx);
  ~StmtConditional();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const std::vector<conditional_t> &conds() const;
};

class StmtFor : public StmtBase {
  const StmtBase *m_init, *m_cond, *m_incr;
  const StmtBase *m_body;

public:
  StmtFor(const StmtBase *init, const StmtBase *cond, const StmtBase *incr,
          const StmtBase *body, const size_t &idx);
  ~StmtFor();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const StmtBase *init() const;
  const StmtBase *cond() const;
  const StmtBase *incr() const;
  const StmtBase *body() const;
};

class StmtForEach : public StmtBase {
  const lex::tok_t *m_loop_var;
  const StmtBase *m_expr;
  const StmtBase *m_body;

public:
  StmtForEach(const lex::tok_t *loop_var, const StmtBase *expr,
              const StmtBase *body, const size_t &idx);
  ~StmtForEach();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const lex::tok_t *loop_var() const;
  const StmtBase *expr() const;
  const StmtBase *body() const;
};

class StmtWhile : public StmtBase {
  const StmtBase *m_expr;
  const StmtBase *m_body;

public:
  StmtWhile(const StmtBase *expr, const StmtBase *body, const size_t &idx);
  ~StmtWhile();

  void disp(const bool has_next) const;

  bool codegen(Bytecode &bc) const;

  const StmtBase *expr() const;
  const StmtBase *body() const;
};

#endif // COMPILER_PARSER_STMTS_HPP
