// Jack compatible (unaryOp term)

case T_SYMBOL: {
  if (is_unary_op(t)) {
    expect(p, T_SYMBOL, NULL);
    compileTerm(p);
  }
  //'('expression')'
  else if (t.type == T_SYMBOL && *(t.lexeme) == '(') {
    expect(p, T_SYMBOL, "(");
    compileExpression(p);
    expect(p, T_SYMBOL, ")");
  }
  break;
}
}

void compileExpression(ParserState *p) {
  writeTag(p, "<expression>");

  compileTerm(p);
  while (is_binary_op(peek(p))) {
    expect(p, T_SYMBOL, NULL);
    compileTerm(p);
  }
  writeTag(p, "</expression>");
}
