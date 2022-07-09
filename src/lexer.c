#include "lexer.h"
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

static char advance(Lexer *lexer) { return *(lexer->current++); }

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
    while (true) {
        switch (peek(lexer)) {
        case ' ':
        case '\r':
        case '\t':
            advance(lexer);
            break;
        case '\n':
            skip(lexer);
            advance(lexer);
            break;
        case '#':
            while (peek(lexer) != '\n' && !is_at_end(lexer))
                advance(lexer);
            break;
        default:
            return;
        }
    }
}

static TokenKind check_kw(Lexer *lexer, int start, int length, const char *rest,
                          TokenKind type) {
    if (lexer->current - lexer->start == start + length &&
        memcmp(lexer->start + start, rest, length) == 0) {
        return type;
    }

    return TK_IDENT;
}

TokenKind ident_type(Lexer *lexer) {
    switch (lexer->current[0]) {
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
    while (is_ident(peek(lexer) || is_digit(peek(lexer))))
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
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n')
            lexer->line++;
        skip(lexer);
    }

    if (is_at_end(lexer))
        return TK_INVALID;

    skip(lexer); // closing quote
    return TK_STRING;
}

Token next_token(Lexer *lexer) {
    skip_whitespace(lexer);

    lexer->start = lexer->current;
    TokenKind kind;

    if (is_at_end(lexer))
        kind = TK_EOF;

    char c = advance(lexer);
    if (is_ident(c)) {
        kind = ident_type(lexer);
    }
    else if (is_digit(c)) {
        kind = number(lexer);
    }
    else {
        switch (c) {
    case '(': kind = TK_LPAREN;
    case ')': kind = TK_RPAREN;
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case '-': return makeToken(TOKEN_MINUS);
    case '+': return makeToken(TOKEN_PLUS);
    case '/': return makeToken(TOKEN_SLASH);
    case '*': return makeToken(TOKEN_STAR);
    case '!':
      return makeToken(
          match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return makeToken(
          match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return makeToken(
          match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return makeToken(
          match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

    case '"': return string();
  }
    }

    return (Token){
        .kind = kind,
        .start = lexer->start,
        .length = lexer->current - lexer->start,
        .line = lexer->line,
    };
}