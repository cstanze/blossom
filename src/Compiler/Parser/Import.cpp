

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_import_decl(ParseHelper &ph, StmtBase *&loc) {
  std::vector<const StmtImportBase *> decls;
  StmtBase *decl = nullptr;

  size_t idx = ph.peak()->pos;
  if (!ph.accept(TOK_IMPORT)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected keyword 'import' here, but found: '%s'",
             TokStrs[ph.peakt()]);
    goto fail;
  }
  ph.next();

begin:
  if (parse_import_decl_base(ph, decl) != E_OK) {
    goto fail;
  }
  decls.push_back((StmtImportBase *)decl);
  decl = nullptr;

  if (ph.accept(TOK_COMMA)) {
    ph.next();
    goto begin;
  }

  if (ph.accept(TOK_COLS)) {
    // Err::set(E_PARSE_FAIL, ph.peak()->pos,
    //          "expected semicolon after import declaration, found: '%s'",
    //          TokStrs[ph.peakt()]);
    // goto fail;
    ph.next();
  }
  // ph.next();
  loc = new StmtImport(decls, idx);
  return E_OK;
fail:
  for (auto &decl : decls)
    delete decl;
  return E_PARSE_FAIL;
}

Errors parse_import_decl_base(ParseHelper &ph, StmtBase *&loc) {
  StmtBase *mod = nullptr;
  StmtSimple *name = nullptr;
  size_t idx = ph.peak()->pos;

  if (!ph.accept(TOK_STRING)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected string here, but found: '%s'",
             TokStrs[ph.peakt()]);
    goto fail;
  }
  mod = new StmtSimple(ph.peak());
  ph.next();

  if (ph.accept(TOK_AS)) {
    ph.next();
    if (!ph.accept(TOK_IDENT)) {
      Err::set(E_PARSE_FAIL, ph.peak()->pos,
               "expected identifier here, but found: '%s'",
               TokStrs[ph.peakt()]);
      goto fail;
    }
    ph.sett(TOK_STRING);
    name = new StmtSimple(ph.peak());
    ph.next();
  }

  loc = new StmtImportBase((StmtSimple*)mod, name, idx);
  return E_OK;
fail:
  if (mod)
    delete mod;
  return E_PARSE_FAIL;
}