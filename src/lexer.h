#ifndef LLAMBDA_LEXER_H
#define LLAMBDA_LEXER_H

#include "vec.h"
#include <stdint.h>
#include <stdlib.h>

// Stores lexer state
typedef struct {
  const char *start;
  const char *current;
  uint64_t line;
} Lexer;

// An enumeration of the different kinds of tokens
typedef enum {
  /* KEYWORDS */
  TK_LET,   // "let"
  TK_IN,    // "in"
  TK_FUN,   // "fun"
  TK_IF,    // "if"
  TK_THEN,  // "then"
  TK_ELSE,  // "else"
  TK_PRINT, // "print"

  /* LITERALS */
  TK_TRUE,   // "true"
  TK_FALSE,  // "false"
  TK_UNIT,   // "unit"
  TK_NUMBER, // Numeric literal
  TK_STRING, // String literal

  /* MISC */
  TK_IDENT,  // Identifier
  TK_ASSIGN, // "="
  TK_ARROW,  // "=>"
  TK_LPAREN, // "("
  TK_RPAREN, // ")"
  TK_COMMA,  // ","
  TK_FNPIPE, // "|>"

  /* ARITHMETIC OPS */
  // TK_SUB (unary)
  TK_ADD, // "+"
  TK_SUB, // "-"
  TK_MUL, // "*"
  TK_DIV, // "/"

  /* BOOLEAN OPS */
  TK_NOT, // "not"
  TK_AND, // "and"
  TK_OR,  // "or"

  /* COMPARISON OPS */
  TK_LT,  // "<"
  TK_LEQ, // "<="
  TK_GT,  // ">"
  TK_GEQ, // ">="
  TK_EQ,  // "=="
  TK_NEQ, // "!="

  /* SPECIAL */
  TK_INVALID, // Invalid token
  TK_EOF,     // End Of File
} TokenKind;

// A singular token
typedef struct {
  TokenKind kind;
  const char *start;
  size_t length;
  uint64_t line;
} Token;

VecHeader(Token)

    // Create a new lexer that operates on `source`
    Lexer new_lexer(const char *source);

// Scan the next token from `lexer->source`
Token next_token(Lexer *lexer);

// Convert a token to a string
const char *token_to_string(const char *source, Token token);

#endif