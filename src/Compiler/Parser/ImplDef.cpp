#include <unordered_map>
#include "Compiler/CodeGen/Internal.hpp"

Errors parseFail(StmtBase *args, StmtBase *body, StmtBase *in, StmtBase *name) {
  if (args) delete args;
  if (body) delete body;
  if (in) delete in;
  if (name) delete name;
  return E_PARSE_FAIL;
}

Errors parse_impl_decl(ParseHelper &ph, StmtBase *&loc) {
  bool argsDone = false;
  StmtBase *args = nullptr, *body = nullptr,
           *in = nullptr, *name = nullptr;
  size_t idx = ph.peak()->pos;

  if(!ph.accept(TOK_IMPL)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "Expected 'impl' keyword");
    return parseFail(args, body, in, name);
  }
  ph.next();

  if(!ph.accept(TOK_IDENT)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "Expected identifier as struct name");
    return parseFail(args, body, in, name);
  }
  in = new StmtSimple(ph.peak());
  ph.next();

  if(!ph.accept(TOK_DOT)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "Expected '.' before member name");
    return parseFail(args, body, in, name);
  }
  ph.next();

  if(!ph.accept(TOK_IDENT) && !ph.acceptoper()) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "Expected identifier or operator as member name");
    return parseFail(args, body, in, name);
  }
  ph.sett(TOK_STRING);
  name = new StmtSimple(ph.peak());
  ph.next();

  if(!ph.accept(TOK_LPAREN)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "Expected '(' after member name");
    return parseFail(args, body, in, name);
  }
  ph.next();

  while(true) {
    if(ph.accept(TOK_RPAREN)) {
      ph.next();
      break;
    }

    if(argsDone) {
      Err::set(E_PARSE_FAIL, ph.peak()->pos,
               "Expected ')' after arguments");
      return parseFail(args, body, in, name);
    }

    if(parse_fn_decl_args(ph, args) != E_OK) {
      return parseFail(args, body, in, name);
    }
    argsDone = true;
  }

  if(parse_block(ph, body) != E_OK) {
    return parseFail(args, body, in, name);
  }

  loc = new StmtImplDef((StmtFnDefArgs*)args, (StmtBlock*)body, (StmtSimple*)in, (StmtSimple*)name, idx);
  return E_OK;
}