#include "Compiler/CodeGen/Internal.hpp"

std::vector<ImportWith *> parse_import_decl_with(ParseHelper &ph);

Errors parse_import_decl(ParseHelper &ph, StmtBase *&loc) {
  std::vector<const StmtImportBase *> decls;
  std::vector<ImportWith *> withs;
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

  if (!ph.accept(TOK_COLS)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected semicolon after import declaration, found: '%s'",
             TokStrs[ph.peakt()]);
    goto fail;
  }
  ph.next();
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
  std::vector<ImportWith *> with;
  size_t idx = ph.peak()->pos;

  if (!ph.accept(TOK_STRING)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected string here, but found: '%s'", TokStrs[ph.peakt()]);
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

  if (ph.accept(TOK_WITH)) {
    ph.next();
    with = parse_import_decl_with(ph);
    if (with.empty())
      goto fail;
  }

  loc = new StmtImportBase((StmtSimple *)mod, name, with, idx);
  return E_OK;
fail:
  if (mod)
    delete mod;
  if (name)
    delete name;
  return E_PARSE_FAIL;
}

std::vector<ImportWith *> parse_import_decl_with(ParseHelper &ph) {
  std::vector<ImportWith *> with;
  bool inParen = false;
  const lex::tok_t *_name = nullptr;
  const lex::tok_t *_alias = nullptr;
  size_t idx = ph.peak()->pos;

  if (!ph.accept(TOK_LPAREN) && !ph.accept(TOK_IDENT)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected '(' or identifier here, but found: '%s'",
             TokStrs[ph.peakt()]);
    return {};
  }

  if (ph.accept(TOK_IDENT)) {
    _name = ph.peak();
    ph.next();

    if (ph.accept(TOK_AS)) {
      ph.next();
      if (!ph.accept(TOK_IDENT)) {
        Err::set(E_PARSE_FAIL, ph.peak()->pos,
                 "expected identifier here, but found: '%s'",
                 TokStrs[ph.peakt()]);
        return {};
      }
      ph.sett(TOK_STRING);
      _alias = ph.peak();
      ph.next();
    }

    return {new ImportWith(_name, _alias, idx)};
  }

  ph.next();
  while (true) {
    if (ph.accept(TOK_RPAREN)) {
      ph.next();
      break;
    }

    if (!ph.accept(TOK_IDENT)) {
      Err::set(E_PARSE_FAIL, ph.peak()->pos,
               "expected identifier here, but found: '%s'",
               TokStrs[ph.peakt()]);
      return {};
    }
    ph.sett(TOK_STRING);
    _name = ph.peak();
    ph.next();

    if (ph.accept(TOK_AS)) {
      ph.next();
      if (!ph.accept(TOK_IDENT)) {
        Err::set(E_PARSE_FAIL, ph.peak()->pos,
                 "expected identifier here, but found: '%s'",
                 TokStrs[ph.peakt()]);
        return {};
      }
      ph.sett(TOK_STRING);
      _alias = ph.peak();
      ph.next();
    }

    with.push_back(new ImportWith(_name, _alias, idx));

    if (ph.accept(TOK_COMMA))
      ph.next();
    _name = nullptr;
    _alias = nullptr;
  }

  return with;
}
