#ifndef clox_compiler_h
#define clox_complier_h

#include "vm.h"
#include "scanner.h"

bool compile(const char* source, Chunk* chunk);

// table of precedence
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;

typedef struct {
  Token name;
  int depth;
} Local;

typedef struct {
  Local locals[UINT8_COUNT];
  int localCount;
  int scopeDepth;
} Compiler;

// structure of parser
typedef struct {
    Token current;
    bool hadError;
    bool panicMode;
    Token previous;
} Parser;

typedef void (*ParseFn)(bool canAssign);

// entry of parse rule table 
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

// predeclaration of function related with patt parser
static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);
static uint8_t parseVariable(const char* errorMessage);
static uint8_t identifierConstant(Token* name);
static void defineVariable(uint8_t global);
static int resolveLocal(Compiler* compiler, Token* name);
static void declareVariable();
static void or_(bool canAssign);
static void and_(bool canAssign);
static void expressionStatement();
static void varDeclaration();

#endif