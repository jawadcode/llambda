#include "lexer.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

inline Lexer new_lexer(const char *source) {
  return (Lexer){
      .start = source,
      .current = source,
      .line = 1,
  };
}

static bool is_ident(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool is_at_end(Lexer *lexer) { return *lexer->current == '\0'; }

static char peek(Lexer *lexer) { return *lexer->current; }

static void skip(Lexer *lexer) { lexer->current++; }

static char advance(Lexer *lexer) {
  lexer->current++;
  return lexer->current[-1];
}

static char safe_advance(Lexer *lexer) {
  if (is_at_end(lexer)) {
    return '\0';
  } else {
    return lexer->current[1];
  }
}

static bool match(Lexer *lexer, char expected) {
  if (is_at_end(lexer))
    return false;
  if (*lexer->current != expected)
    return false;
  skip(lexer);
  return true;
}

void skip_whitespace(Lexer *lexer) {
  for (;;) {
    putc(peek(lexer) == '\0', stdout);
    switch (peek(lexer)) {
    case ' ':
    case '\r':
    case '\t':
      skip(lexer);
      break;
    case '\n':
      lexer->line++;
      skip(lexer);
      break;
    case '#':
      skip(lexer);
      while (peek(lexer) != '\n' && !is_at_end(lexer))
        advance(lexer);
      break;
    default:
      return;
    }
  }
}

static TokenKind check_kw(Lexer *lexer, int start, int length, const char *rest,
                          TokenKind kind) {
  if (lexer->current - lexer->start == start + length &&
      memcmp(lexer->start + start, rest, length) == 0) {
    return kind;
  }

  return TK_IDENT;
}

TokenKind ident_type(Lexer *lexer) {
  switch (lexer->start[0]) {
  case 'a':
    return check_kw(lexer, 1, 2, "nd", TK_AND);
  case 'e':
    return check_kw(lexer, 1, 3, "lse", TK_ELSE);
  case 'f':
    if (lexer->current - lexer->start > 1) {
      switch (lexer->start[1]) {
      case 'a':
        return check_kw(lexer, 2, 3, "lse", TK_FALSE);
      case 'u':
        return check_kw(lexer, 2, 1, "n", TK_FUN);
      }
    }
    break;
  case 'i':
    return check_kw(lexer, 1, 1, "n", TK_IN);
  case 'l':
    return check_kw(lexer, 1, 2, "et", TK_LET);
  case 'n':
    return check_kw(lexer, 1, 2, "ot", TK_NOT);
  case 'o':
    return check_kw(lexer, 1, 1, "r", TK_OR);
  case 'p':
    return check_kw(lexer, 1, 4, "rint", TK_PRINT);
  case 't':
    return check_kw(lexer, 1, 3, "hen", TK_THEN);
  case 'u':
    return check_kw(lexer, 1, 3, "nit", TK_UNIT);
  }

  return TK_IDENT;
}

static TokenKind ident(Lexer *lexer) {
  while (is_ident(peek(lexer)) || is_digit(peek(lexer)))
    skip(lexer);
  return ident_type(lexer);
}

static TokenKind number(Lexer *lexer) {
  while (is_digit(peek(lexer)))
    skip(lexer);

  // Look for a fractional part.
  if (peek(lexer) == '.' && is_digit(safe_advance(lexer))) {
    // Consume the ".".
    skip(lexer);

    while (is_digit(peek(lexer)))
      skip(lexer);
  }

  return TK_NUMBER;
}

static TokenKind string(Lexer *lexer) {
  bool escaping = false;

  while (!is_at_end(lexer)) {
    if (peek(lexer) == '"') {
      if (escaping) {
        skip(lexer);
        escaping = false;
        continue;
      } else {
        break;
      }
    }
    if (peek(lexer) == '\\')
      escaping = true;
    if (peek(lexer) == '\n')
      lexer->line++;

    skip(lexer);
  }

  if (is_at_end(lexer))
    return TK_INVALID;

  skip(lexer); // closing quote
  return TK_STRING;
}

static TokenKind next_kind(Lexer *lexer) {
  if (is_at_end(lexer))
    return TK_EOF;

  char c = advance(lexer);
  if (is_ident(c))
    return ident(lexer);
  if (is_digit(c))
    return number(lexer);

  switch (c) {
  case '(':
    return TK_LPAREN;
  case ')':
    return TK_RPAREN;
  case ',':
    return TK_COMMA;
  case '+':
    return TK_ADD;
  case '-':
    return TK_SUB;
  case '*':
    return TK_MUL;
  case '/':
    return TK_DIV;
  case '!':
    return match(lexer, '=') ? TK_NEQ : TK_INVALID;
  case '=':
    return match(lexer, '=') ? TK_EQ : match(lexer, '>') ? TK_ARROW : TK_ASSIGN;
  case '<':
    return match(lexer, '=') ? TK_LEQ : TK_LT;
  case '>':
    return match(lexer, '=') ? TK_GEQ : TK_GT;
  case '"':
    return string(lexer);
  default:
    return TK_INVALID;
  }
}

Token next_token(Lexer *lexer) {
  skip_whitespace(lexer);

  lexer->start = lexer->current;
  TokenKind kind = next_kind(lexer);

  return (Token){
      .kind = kind,
      .start = lexer->start,
      .length = lexer->current - lexer->start,
      .line = lexer->line,
  };
}

static unsigned int count_digits(unsigned int number) {
  return floor(log10((double)number)) + 1;
}

static const char *token_kind_to_string(TokenKind kind) {
  switch (kind) {
  case TK_LET:
    return "KW LET";
  case TK_IN:
    return "KW IN";
  case TK_FUN:
    return "KW FUN";
  case TK_IF:
    return "KW IF";
  case TK_THEN:
    return "KW THEN";
  case TK_ELSE:
    return "KW ELSE";
  case TK_PRINT:
    return "KW PRINT";
  case TK_TRUE:
    return "KW TRUE";
  case TK_FALSE:
    return "KW FALSE";
  case TK_UNIT:
    return "KW UNIT";
  case TK_NUMBER:
    return "NUMERIC LITERAL";
  case TK_STRING:
    return "STRING LITERAL";
  case TK_IDENT:
    return "IDENTIFIER";
  case TK_ASSIGN:
    return "ASSIGN";
  case TK_ARROW:
    return "ARROW";
  case TK_LPAREN:
    return "LEFT PAREN";
  case TK_RPAREN:
    return "RIGHT PAREN";
  case TK_COMMA:
    return "COMMA";
  case TK_FNPIPE:
    return "OP PIPE";
  case TK_ADD:
    return "OP ADD";
  case TK_SUB:
    return "OP SUB";
  case TK_MUL:
    return "OP MUL";
  case TK_DIV:
    return "OP DIV";
  case TK_NOT:
    return "OP NOT";
  case TK_AND:
    return "OP AND";
  case TK_OR:
    return "OP OR";
  case TK_LT:
    return "OP LT";
  case TK_LEQ:
    return "OP LEQ";
  case TK_GT:
    return "OP GT";
  case TK_GEQ:
    return "OP GEQ";
  case TK_EQ:
    return "OP EQ";
  case TK_NEQ:
    return "OP NEQ";
  case TK_INVALID:
    return "INVALID TOKEN";
  case TK_EOF:
    return "END OF FILE";
  }
  return "THIS IS LITERALLY UNREACHABLE YOU STUPID BLOODY COMPILER";
}

const char *token_to_string(const char *source, Token token) {
  const char *kind = token_kind_to_string(token.kind);
  size_t start = token.start - source;
  size_t end = start + token.length;

  char *res = (char *)malloc(sizeof(char) *
                             (
                                 // name of token (right padded with whitespace)
                                 16 +
                                 // " @ "
                                 3 +
                                 // start of span
                                 count_digits(start) +
                                 // ".."
                                 2 +
                                 // end of span
                                 count_digits(end) +
                                 // " "
                                 1 +
                                 // length of token
                                 token.length +
                                 // null terminator (i think)
                                 1));
  char *text = (char *)malloc(token.length * sizeof(char));
  text = (char *)memcpy(text, token.start, token.length);
  text = (char *)reallocate(text, token.length * sizeof(char),
                            (token.length + 1) * sizeof(char));
  text[token.length] = '\0';
  sprintf(res, "%-16s @ %zu..%zu %s", kind, start, end, text);

  return res;
}

VecImpl(Token)