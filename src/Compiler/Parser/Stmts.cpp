

#include "Compiler/Parser/Stmts.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// BASE
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtBase::StmtBase(const GramType type, const size_t &idx)
    : m_idx(idx), m_type(type) {}
StmtBase::~StmtBase() {}

size_t StmtBase::idx() const { return m_idx; }
GramType StmtBase::type() const { return m_type; }

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// SIMPLE
////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtSimple::StmtSimple(const lex::tok_t *val)
    : StmtBase(GT_SIMPLE, val->pos), m_val(val) {}

void StmtSimple::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Simple at: %p\n", this);
  io::tadd(false);
  if (m_val) {
    io::print(false, "Value: %s (type: %s)\n",
              !m_val->data.empty() ? m_val->data.c_str() : TokStrs[m_val->type],
              TokStrs[m_val->type]);
  }
  io::trem(2);
}

const lex::tok_t *StmtSimple::val() const { return m_val; }

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// BLOCK
////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtBlock::StmtBlock(const std::vector<const StmtBase *> &stmts,
                     const size_t &idx)
    : StmtBase(GT_BLOCK, idx), m_stmts(stmts), m_no_brace(false) {}
StmtBlock::~StmtBlock() {
  for (auto &s : m_stmts)
    delete s;
}

void StmtBlock::set_no_brace(const bool &no_brace) { m_no_brace = no_brace; }

void StmtBlock::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Block at: %p (top level: %s)\n", this,
            m_no_brace ? "yes" : "no");
  for (size_t i = 0; i < m_stmts.size(); ++i) {
    m_stmts[i]->disp(i != m_stmts.size() - 1);
  }
  io::trem();
}

const std::vector<const StmtBase *> &StmtBlock::stmts() const {
  return m_stmts;
}
const bool &StmtBlock::no_brace() const { return m_no_brace; }

void StmtBlock::clear_stmts() { m_stmts.clear(); }

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// EXPR
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtExpr::StmtExpr(const StmtBase *lhs, const lex::tok_t *oper,
                   const StmtBase *rhs, const size_t &idx)
    : StmtBase(GT_EXPR, idx), m_lhs(lhs), m_rhs(rhs), m_oper(oper),
      m_or_blk(nullptr), m_commas(0), m_with_cols(false) {}

StmtExpr::~StmtExpr() {
  if (m_lhs)
    delete m_lhs;
  if (m_rhs)
    delete m_rhs;
  if (m_or_blk)
    delete m_or_blk;
}

void StmtExpr::set_or_blk(StmtBase *or_blk, const lex::tok_t *or_blk_var) {
  m_or_blk = or_blk;
  m_or_blk_var = or_blk_var;
}
void StmtExpr::set_with_cols(const bool &with_cols) { m_with_cols = with_cols; }

const StmtBase *StmtExpr::lhs() const { return m_lhs; }
const StmtBase *StmtExpr::rhs() const { return m_rhs; }
const lex::tok_t *StmtExpr::oper() const { return m_oper; }
const StmtBase *StmtExpr::or_blk() const { return m_or_blk; }
const lex::tok_t *StmtExpr::or_blk_var() const { return m_or_blk_var; }
size_t StmtExpr::commas() const { return m_commas; }
const bool &StmtExpr::with_cols() const { return m_with_cols; }
void StmtExpr::commas_set(const size_t &commas) { m_commas = commas; }

void StmtExpr::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Expression at: %p (commas: %zu) (with semicolon: %s)\n",
            this, m_commas, m_with_cols ? "yes" : "no");

  io::tadd(m_lhs != nullptr || m_rhs != nullptr);
  io::print(m_lhs != nullptr || m_rhs != nullptr, "Operator: %s\n",
            m_oper != nullptr ? TokStrs[m_oper->type] : "(null)");
  io::trem();
  if (m_lhs != nullptr) {
    io::tadd(m_rhs != nullptr);
    io::print(m_rhs != nullptr, "LHS:\n");
    m_lhs->disp(false);
    io::trem();
  }
  if (m_rhs != nullptr) {
    io::tadd(m_or_blk != nullptr);
    io::print(m_or_blk != nullptr, "RHS:\n");
    m_rhs->disp(false);
    io::trem();
  }
  io::tadd(false);
  if (m_or_blk != nullptr) {
    io::print(false, "Or Block (var: %s):\n",
              m_or_blk_var ? m_or_blk_var->data.c_str() : "<none>");
    m_or_blk->disp(false);
  }
  io::trem(2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// VAR_DECL_BASE
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtVarDeclBase::StmtVarDeclBase(const StmtSimple *lhs, const StmtBase *in,
                                 const StmtBase *rhs)
    : StmtBase(GT_VAR_DECL_BASE, lhs->val()->pos), m_lhs(lhs), m_in(in),
      m_rhs(rhs) {}

StmtVarDeclBase::~StmtVarDeclBase() {
  delete m_lhs;
  if (m_in)
    delete m_in;
  delete m_rhs;
}

void StmtVarDeclBase::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Var Decl Base at: %p\n", this);
  io::tadd(true);
  io::print(true, "LHS:\n");
  m_lhs->disp(false);
  io::trem();
  if (m_in) {
    io::tadd(true);
    io::print(true, "In:\n");
    m_in->disp(false);
    io::trem();
  }
  io::tadd(false);
  io::print(false, "RHS:\n");
  m_rhs->disp(false);
  io::trem(2);
}

const StmtSimple *StmtVarDeclBase::lhs() const { return m_lhs; }
const StmtBase *StmtVarDeclBase::in() const { return m_in; }
const StmtBase *StmtVarDeclBase::rhs() const { return m_rhs; }

bool StmtVarDeclBase::has_in() const { return m_in != nullptr; }

StmtVarDecl::StmtVarDecl(const std::vector<const StmtVarDeclBase *> &decls,
                         const size_t &idx)
    : StmtBase(GT_VAR_DECL, idx), m_decls(decls) {}

StmtVarDecl::~StmtVarDecl() {
  for (auto &decl : m_decls)
    delete decl;
}

void StmtVarDecl::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Var Decl(s) at: %p\n", this);
  for (size_t i = 0; i < m_decls.size(); ++i) {
    m_decls[i]->disp(i != m_decls.size() - 1);
  }
  io::trem();
}

const std::vector<const StmtVarDeclBase *> &StmtVarDecl::decls() const {
  return m_decls;
}


StmtImportBase::StmtImportBase(const StmtSimple *mod, const StmtSimple *name, const size_t &idx)
  : StmtBase(GT_VAR_DECL_BASE, idx), m_mod(mod), m_name(name) {}

void StmtImportBase::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Import Base at: %p\n", this);
  m_mod->disp(true);
  if(m_name)
    m_name->disp(false);
  io::trem();
}

StmtImportBase::~StmtImportBase() {
  delete m_mod;
  if(m_name)
    delete m_name;
}

const StmtSimple *StmtImportBase::mod() const { return m_mod; }
const StmtSimple *StmtImportBase::name() const { return m_name; }


StmtImport::StmtImport(const std::vector<const StmtImportBase*> &decls, const size_t &idx)
  : StmtBase(GT_VAR_DECL, idx), m_decls(decls) {}

StmtImport::~StmtImport() {
  for (auto &d : m_decls)
    delete d;
}

void StmtImport::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Import at: %p\n", this);
  for (size_t i = 0; i < m_decls.size(); ++i) {
    m_decls[i]->disp(i != m_decls.size() - 1);
  }
  io::trem();
}

const std::vector<const StmtImportBase*> &StmtImport::decls() const { return m_decls; }

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// FUNC_ASSN_ARG
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtFnAssnArg::StmtFnAssnArg(const StmtSimple *lhs, const StmtBase *rhs)
    : StmtBase(GT_FN_ASSN_ARG, lhs->val()->pos), m_lhs(lhs), m_rhs(rhs) {}
StmtFnAssnArg::~StmtFnAssnArg() {
  delete m_lhs;
  delete m_rhs;
}

void StmtFnAssnArg::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Assigned Argument at: %p\n", this);
  io::tadd(true);
  io::print(m_rhs, "Parameter:\n");
  m_lhs->disp(false);
  io::trem();
  if (m_rhs) {
    io::tadd(false);
    io::print(false, "Value:\n");
    m_rhs->disp(false);
    io::trem();
  }
  io::trem();
}

const StmtSimple *StmtFnAssnArg::lhs() const { return m_lhs; }
const StmtBase *StmtFnAssnArg::rhs() const { return m_rhs; }

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// FUNC_DEF_ARGS
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtFnDefArgs::StmtFnDefArgs(const std::vector<const StmtBase *> &args,
                             const StmtSimple *kwarg, const StmtSimple *vaarg,
                             const size_t &idx)
    : StmtBase(GT_FN_ARGS, idx), m_args(args), m_kwarg(kwarg), m_vaarg(vaarg) {}
StmtFnDefArgs::~StmtFnDefArgs() {
  for (auto &a : m_args)
    delete a;
  if (m_kwarg)
    delete m_kwarg;
  if (m_vaarg)
    delete m_vaarg;
}

void StmtFnDefArgs::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Arguments at: %p\n", this);
  if (m_kwarg) {
    io::tadd(true);
    io::print(m_vaarg || m_args.size() > 0, "Keyword Argument:\n");
    m_kwarg->disp(m_vaarg || m_args.size() > 0);
    io::trem();
  }
  if (m_vaarg) {
    io::tadd(true);
    io::print(m_args.size() > 0, "Variadic Argument:\n");
    m_vaarg->disp(m_args.size() > 0);
    io::trem();
  }
  for (size_t i = 0; i < m_args.size(); ++i) {
    m_args[i]->disp(i != m_args.size() - 1);
  }
  io::trem();
}
const std::vector<const StmtBase *> &StmtFnDefArgs::args() const {
  return m_args;
}
const StmtSimple *StmtFnDefArgs::kwarg() const { return m_kwarg; }
const StmtSimple *StmtFnDefArgs::vaarg() const { return m_vaarg; }

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// FUNC_DEF
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtFnDef::StmtFnDef(const StmtFnDefArgs *args, const StmtBlock *body,
                     const StmtSimple *name, const bool isAnon,
                     const size_t &idx)
    : StmtBase(GT_FN_DEF, idx), m_args(args), m_body(body), m_name(name),
      m_isAnon(isAnon) {}
StmtFnDef::~StmtFnDef() {
  if (m_args)
    delete m_args;
  if (m_name)
    delete m_name;
  delete m_body;
}

void StmtFnDef::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "%sFunction definition at: %p\n",
            m_isAnon ? "Anonymous " : "", this);
  if (m_args) {
    m_args->disp(true);
  }
  if (m_name) {
    m_name->disp(true);
  }
  m_body->disp(false);
  io::trem();
}
const StmtFnDefArgs *StmtFnDef::args() const { return m_args; }
const StmtBlock *StmtFnDef::body() const { return m_body; }

StmtImplDef::StmtImplDef(const StmtFnDefArgs *args, const StmtBlock *body,
                         const StmtSimple *in, const StmtSimple *name,
                         const size_t &idx)
    : StmtBase(GT_FN_DEF, idx), m_args(args), m_body(body), m_in(in),
      m_name(name) {}
StmtImplDef::~StmtImplDef() {
  if (m_args)
    delete m_args;
  delete m_in;
  delete m_name;
  delete m_body;
}

void StmtImplDef::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Implementation definition at: %p\n", this);
  if (m_args) {
    m_args->disp(true);
  }
  m_in->disp(true);
  m_name->disp(true);
  m_body->disp(false);
  io::trem();
}
const StmtFnDefArgs *StmtImplDef::args() const { return m_args; }
const StmtBlock *StmtImplDef::body() const { return m_body; }
const StmtSimple *StmtImplDef::in() const { return m_in; }

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// FUNC_CALL_ARGS
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtFnCallArgs::StmtFnCallArgs(
    const std::vector<const StmtBase *> &args,
    const std::vector<const StmtFnAssnArg *> &assn_args, const bool &va_unpack,
    const size_t &idx)
    : StmtBase(GT_FN_ARGS, idx), m_args(args), m_assn_args(assn_args),
      m_va_unpack(va_unpack) {}
StmtFnCallArgs::~StmtFnCallArgs() {
  for (auto &a : m_args)
    delete a;
  for (auto &a : m_assn_args)
    delete a;
}

void StmtFnCallArgs::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Arguments at: %p (va_unpack: %s)\n", this,
            m_va_unpack ? "yes" : "no");
  for (size_t i = 0; i < m_args.size(); ++i) {
    m_args[i]->disp(i != m_args.size() - 1 || m_assn_args.size() > 0);
  }
  for (size_t i = 0; i < m_assn_args.size(); ++i) {
    m_assn_args[i]->disp(i != m_assn_args.size() - 1);
  }
  io::trem();
}
const std::vector<const StmtBase *> &StmtFnCallArgs::args() const {
  return m_args;
}
const std::vector<const StmtFnAssnArg *> &StmtFnCallArgs::assn_args() const {
  return m_assn_args;
}
const bool &StmtFnCallArgs::va_unpack() const { return m_va_unpack; }

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// SINGLE_EXPR_STMT
/////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtSingleOpStmt::StmtSingleOpStmt(const lex::tok_t *sost,
                                   const StmtBase *operand)
    : StmtBase(GT_SINGLE_OPERAND_STMT, sost->pos), m_sost(sost),
      m_operand(operand) {}
StmtSingleOpStmt::~StmtSingleOpStmt() {
  if (m_operand)
    delete m_operand;
}

void StmtSingleOpStmt::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "%s at: %p\n", TokStrs[m_sost->type], this);
  if (m_operand) {
    io::tadd(false);
    io::print(false, "Operand:\n");
    m_operand->disp(false);
    io::trem();
  }
  io::trem();
}

const lex::tok_t *StmtSingleOpStmt::sost() const { return m_sost; }
const StmtBase *StmtSingleOpStmt::operand() const { return m_operand; }

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// CONDITIONAL_STMT
/////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtConditional::StmtConditional(const std::vector<conditional_t> &conds,
                                 const size_t &idx)
    : StmtBase(GT_CONDITIONAL, idx), m_conds(conds) {}
StmtConditional::~StmtConditional() {
  for (auto &c : m_conds) {
    if (c.condition)
      delete c.condition;
    delete c.body;
  }
}

void StmtConditional::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "Conditional at: %p\n", this);
  for (size_t i = 0; i < m_conds.size(); ++i) {
    io::tadd(i != m_conds.size() - 1);
    if (i == 0) {
      io::print(i != m_conds.size() - 1, "If:\n");
    } else if (m_conds[i].condition == nullptr) {
      io::print(i != m_conds.size() - 1, "Else:\n");
    } else {
      io::print(i != m_conds.size() - 1, "Elif:\n");
    }
    if (m_conds[i].condition) {
      m_conds[i].condition->disp(true);
    }
    m_conds[i].body->disp(false);
    io::trem();
  }
  io::trem();
}

const std::vector<conditional_t> &StmtConditional::conds() const {
  return m_conds;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// FOR_STMT
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtFor::StmtFor(const StmtBase *init, const StmtBase *cond,
                 const StmtBase *incr, const StmtBase *body, const size_t &idx)
    : StmtBase(GT_FOR, idx), m_init(init), m_cond(cond), m_incr(incr),
      m_body(body) {}
StmtFor::~StmtFor() {
  if (m_init)
    delete m_init;
  if (m_cond)
    delete m_cond;
  if (m_incr)
    delete m_incr;
  delete m_body;
}

void StmtFor::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "For loop at: %p\n", this);

  if (m_init) {
    io::tadd(true);
    io::print(true, "Initialization:\n");
    m_init->disp(false);
    io::trem();
  }

  if (m_cond) {
    io::tadd(true);
    io::print(true, "Condition:\n");
    m_cond->disp(false);
    io::trem();
  }

  if (m_incr) {
    io::tadd(true);
    io::print(true, "Increment:\n");
    m_incr->disp(false);
    io::trem();
  }

  io::tadd(false);

  io::print(false, "Body:\n");
  m_body->disp(false);

  io::trem(2);
}

const StmtBase *StmtFor::init() const { return m_init; }
const StmtBase *StmtFor::cond() const { return m_cond; }
const StmtBase *StmtFor::incr() const { return m_incr; }
const StmtBase *StmtFor::body() const { return m_body; }

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// FOREACH_STMT
/////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtForEach::StmtForEach(const lex::tok_t *loop_var, const StmtBase *expr,
                         const StmtBase *body, const size_t &idx)
    : StmtBase(GT_FOREACH, idx), m_loop_var(loop_var), m_expr(expr),
      m_body(body) {}
StmtForEach::~StmtForEach() {
  delete m_expr;
  delete m_body;
}

void StmtForEach::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "For loop at: %p\n", this);

  io::tadd(true);
  io::print(true, "Loop var: %s\n", m_loop_var->data.c_str());
  io::trem();

  io::tadd(true);
  io::print(true, "Expression:\n");
  m_expr->disp(false);
  io::trem();

  io::tadd(false);

  io::print(false, "Body:\n");
  m_body->disp(false);

  io::trem(2);
}

const lex::tok_t *StmtForEach::loop_var() const { return m_loop_var; }
const StmtBase *StmtForEach::expr() const { return m_expr; }
const StmtBase *StmtForEach::body() const { return m_body; }

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// WHILE_STMT
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

StmtWhile::StmtWhile(const StmtBase *expr, const StmtBase *body,
                     const size_t &idx)
    : StmtBase(GT_WHILE, idx), m_expr(expr), m_body(body) {}
StmtWhile::~StmtWhile() {
  delete m_expr;
  delete m_body;
}

void StmtWhile::disp(const bool has_next) const {
  io::tadd(has_next);
  io::print(has_next, "While loop at: %p\n", this);

  io::tadd(true);
  io::print(true, "Condition:\n");
  m_expr->disp(false);
  io::trem();

  io::tadd(false);
  io::print(false, "Body:\n");
  m_body->disp(false);
  io::trem(2);
}

const StmtBase *StmtWhile::expr() const { return m_expr; }
const StmtBase *StmtWhile::body() const { return m_body; }
