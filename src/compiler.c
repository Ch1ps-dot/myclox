#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

#include "object.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

Parser parser;
Compiler* current = NULL;


static Chunk* currentChunk() {
  return &current->function->chunk;
}

// mark lexical error message
static void 
errorAt(Token* token, const char* message) {

    // trigger panic mode
    if (parser.panicMode) return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // nothing
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

// mark error token just consumed
static void 
error(const char* message) {
    errorAt(&parser.previous, message);
}

// mark current error token
static void 
errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}


// scan a token and store in the current pointer of parser
static void
advance() {
    parser.previous = parser.current;


    // asking for nonerror token
    for(;;) {
        parser.current = scanToken();
        
        if (parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

// validating the type of token
static void 
consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

// validate tokentype
static bool 
check(TokenType type) {
  return parser.current.type == type;
}

// validate token and advance
static bool 
match(TokenType type) {
  if (!check(type)) return false;
  advance();
  return true;
}

// generaate bytecode to chunk
static void 
emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}

// generate bytecode for two byte instruction
static void 
emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

// generate bytecode for loop
static void 
emitLoop(int loopStart) {
  emitByte(OP_LOOP);

  int offset = currentChunk()->count - loopStart + 2;
  if (offset > UINT16_MAX) error("Loop body too large.");

  emitByte((offset >> 8) & 0xff);
  emitByte(offset & 0xff);
}

// 
static int 
emitJump(uint8_t instruction) {
    emitByte(instruction);
    emitByte(0xff);
    emitByte(0xff);
    return currentChunk()->count - 2;
}

// emit return bytecode to end expression
static void 
emitReturn() {
    emitByte(OP_NIL);
    emitByte(OP_RETURN);
}

// insert constant into constant table of currentchunk, return its index.
static uint8_t 
makeConstant(Value value) {
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

// emit bytecode of constant
static void 
emitConstant(Value value) {
    emitBytes(OP_CONSTANT,makeConstant(value));
}

// backpatch
static void 
patchJump(int offset) {
  // -2 to adjust for the bytecode for the jump offset itself.
  int jump = currentChunk()->count - offset - 2;

  if (jump > UINT16_MAX) {
    error("Too much code to jump over.");
  }

  currentChunk()->code[offset] = (jump >> 8) & 0xff;
  currentChunk()->code[offset + 1] = jump & 0xff;
}

// init compiler state
static void 
initCompiler(Compiler* compiler, FunctionType type) {
    compiler->enclosing = current;
    compiler->function = NULL;
    compiler->type = type;
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    compiler->function = newFunction();
    current = compiler;
    if (type != TYPE_SCRIPT) {
        current->function->name = copyString(parser.previous.start,
                                            parser.previous.length);
    }

    // initial internal local
    Local* local = &current->locals[current->localCount++];
    local->depth = 0;
    local->name.start = "";
    local->name.length = 0;
    local->isCaptured = false;
}

// end the process of compling
static ObjFunction* 
endCompiler() {
    emitReturn();
    ObjFunction* function = current->function;
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), function->name != NULL
        ? function->name->chars : "<script>");
    }
#endif

    current = current->enclosing;
    return function;
}

// enter a new scope
static void 
beginScope() {
    current->scopeDepth++;
}

// exit the scope and pop variable slots from stack
static void 
endScope() {
    current->scopeDepth--;
    while (current->localCount > 0 &&
            current->locals[current->localCount - 1].depth >
                current->scopeDepth) {
        if (current->locals[current->localCount - 1].isCaptured) {
            emitByte(OP_CLOSE_UPVALUE);
        } else {
            emitByte(OP_POP);
        }
        current->localCount--;
    }
}

// parse binary operator
static void 
binary(bool canAssign) {
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);

    // recursively parse cascaded binary operator
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_PLUS:    emitByte(OP_ADD); break;
        case TOKEN_MINUS:   emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:    emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:   emitByte(OP_DIVIDE); break;
        case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
        case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
        case TOKEN_GREATER:       emitByte(OP_GREATER); break;
        case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
        case TOKEN_LESS:          emitByte(OP_LESS); break;
        case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
        default: return; // Unreachable
    }
}

// parse function call
static void 
call(bool canAssign) {
    uint8_t argCount = argumentList();
    emitBytes(OP_CALL, argCount);
}

static void
literal(bool canAssign) {
    switch (parser.previous.type) {
        case TOKEN_FALSE: emitByte(OP_FALSE); break;
        case TOKEN_TRUE:  emitByte(OP_TRUE); break;
        case TOKEN_NIL: emitByte(OP_NIL); break;
    }
}

//  parse expression
static void 
expression() {
  parsePrecedence(PREC_ASSIGNMENT);
}

// parse forstmt
static void 
forStatement() {
    beginScope();
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
    // parse initializer
    if (match(TOKEN_SEMICOLON)) {
        // No initializer.
    } else if (match(TOKEN_VAR)) {
        varDeclaration();
    } else {
        expressionStatement();
    }
    
    int loopStart = currentChunk()->count;
    int exitJump = -1;
    // parse condition
    if (!match(TOKEN_SEMICOLON)) {
        expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

        // Jump out of the loop if the condition is false.
        exitJump = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP); // Condition.
    }

    // parse increment
    if (!match(TOKEN_RIGHT_PAREN)) {
        int bodyJump = emitJump(OP_JUMP);
        int incrementStart = currentChunk()->count;
        expression();
        emitByte(OP_POP);
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

        emitLoop(loopStart);
        loopStart = incrementStart;
        patchJump(bodyJump);
    }

    statement();
    emitLoop(loopStart);

    if (exitJump != -1) {
        patchJump(exitJump);
        emitByte(OP_POP); // Condition.
    }
    endScope();
}

// parse ifstmt
static void 
ifStatement() {
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition."); 

    int thenJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();

    int elseJump = emitJump(OP_JUMP);

    patchJump(thenJump);
    emitByte(OP_POP);

    // parse elsestmt
    if (match(TOKEN_ELSE)) statement();
    patchJump(elseJump);
}

// parse block
static void 
block() {
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        declaration();
    }

    consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

// helper function to parse body of function 
static void 
function(FunctionType type) {
    Compiler compiler;
    initCompiler(&compiler, type);
    beginScope(); 

    consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");

    // parse parameters
    if (!check(TOKEN_RIGHT_PAREN)) {
        do {
            current->function->arity++;
            if (current->function->arity > 255) {
                errorAtCurrent("Can't have more than 255 parameters.");
            }
            uint8_t constant = parseVariable("Expect parameter name.");
            defineVariable(constant);
        } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
    block();

    ObjFunction* function = endCompiler();
    emitBytes(OP_CLOSURE, makeConstant(OBJ_VAL(function)));

    // emit local value and upvalues
    for (int i = 0; i < function->upvalueCount; i++) {
        emitByte(compiler.upvalues[i].isLocal ? 1 : 0);
        emitByte(compiler.upvalues[i].index);
    }
}

// parse funDeclarationstmt
static void funDeclaration() {
    uint8_t global = parseVariable("Expect function name.");
    markInitialized();
    function(TYPE_FUNCTION);
    defineVariable(global);
}

// parse local and global variable
static void 
varDeclaration() {
    uint8_t global = parseVariable("Expect variable name.");

    if (match(TOKEN_EQUAL)) {
        expression();
    } else {
        emitByte(OP_NIL); // defaultly initialize variable to nil
    }
    consume(TOKEN_SEMICOLON,
            "Expect ';' after variable declaration.");

    defineVariable(global);
}

// parse expressionStatement
static void 
expressionStatement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

// parse printStatement
static void 
printStatement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

// parse returnStatement
static void 
returnStatement() {
    if (match(TOKEN_SEMICOLON)) {
        emitReturn();
    } else {
        expression();
        consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
        emitByte(OP_RETURN);
    }
}

// parse whilestmt
static void 
whileStatement() {
    int loopStart = currentChunk()->count;
    consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
    statement();
    emitLoop(loopStart);

    patchJump(exitJump);
    emitByte(OP_POP);
}

// error synchronize
static void 
synchronize() {
  parser.panicMode = false;

  while (parser.current.type != TOKEN_EOF) {
    if (parser.previous.type == TOKEN_SEMICOLON) return;
    switch (parser.current.type) {
      case TOKEN_CLASS:
      case TOKEN_FUN:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;

      default:
        ; // Do nothing.
    }

    advance();
  }
}

// parse declaration
static 
void declaration() {
    if (match(TOKEN_FUN)) {
        funDeclaration();
    }
    else if (match(TOKEN_VAR)) {
        varDeclaration();
    } 
    else {
        statement();
    }  
    if (parser.panicMode) synchronize();
}

// parse statement
static void 
statement() {
    if (match(TOKEN_PRINT)) {
        printStatement();
    }
    else if (match(TOKEN_FOR)) {
        forStatement();
    }
    else if (match(TOKEN_IF)) {
        ifStatement();
    } 
    else if (match(TOKEN_LEFT_BRACE)) {
        beginScope();
        block();
        endScope();
    }
    else if (match(TOKEN_WHILE)) {
        whileStatement();
    }
    else if (match(TOKEN_RETURN)) {
        returnStatement();
    }
    else {
        expressionStatement();
    }
}

// parse grouping expression
static void 
grouping(bool canAssign) {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

// parse number
static void 
number(bool canAssign) {
    // strtod(start, end): transfer string to double from where 'start' points to
    // store the char where double end to 'end'
    double value = strtod(parser.previous.start, NULL); 
    emitConstant(NUMBER_VAL(value));
}

// parse or operator
static void 
or_(bool canAssign) {
  int elseJump = emitJump(OP_JUMP_IF_FALSE);
  int endJump = emitJump(OP_JUMP);

  patchJump(elseJump);
  emitByte(OP_POP);

  parsePrecedence(PREC_OR);
  patchJump(endJump);
}

// add string to constant table and
// create new ObjString in heap
static void 
string(bool canAssign) {
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1,
                                    parser.previous.length - 2)));
}

// set or get variable
static void 
namedVariable(Token name, bool canAssign) {
    uint8_t getOp, setOp;
    int arg = resolveLocal(current, &name);
    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    }
    // mark all variable in enclosured function as upvalue
    // specify them through bytecode
    else if ((arg = resolveUpvalue(current, &name)) != -1) { 
        getOp = OP_GET_UPVALUE;
        setOp = OP_SET_UPVALUE;
    }
    else {
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    // check that it is to set or get variable
    if (canAssign && match(TOKEN_EQUAL)) {
        expression();
        emitBytes(setOp, arg);
    } else {
        emitBytes(getOp, arg);
    }
}

// get or set varibable
// encapsuled <func> namedVariable 
static void 
variable(bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

static void 
unary(bool canAssign) {
    TokenType operatorType = parser.previous.type;

    // compile the operand.
    parsePrecedence(PREC_UNARY);

    // emit the operator instruction
    switch (operatorType)
    {
        case TOKEN_BANG: emitByte(OP_NOT); break;
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default:
            return;
    }
}

// function table
ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, call,   PREC_CALL},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {unary,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_GREATER]       = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_GREATER_EQUAL] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS]          = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS_EQUAL]    = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_IDENTIFIER]    = {variable,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {string,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     and_,   PREC_AND},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {literal,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {literal,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     or_,   PREC_OR},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {literal,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

// parse expression in accordance with precedence
static void 
parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expect expression.");
    }

    bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(canAssign);

    while ( (precedence <= getRule(parser.current.type)->precedence)){
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(canAssign);
    }
    if (canAssign && match(TOKEN_EQUAL)) {
        error("Invalid assignment target.");
    }    
}

// add identifier to string table and constant table
static uint8_t 
identifierConstant(Token* name) {
  return makeConstant(OBJ_VAL(copyString(name->start,
                                         name->length)));
}

// match two identifier
static bool 
identifiersEqual(Token* a, Token* b) {
    if (a->length != b->length) return false;
    return memcmp(a->start, b->start, a->length) == 0;
}

// get index of local variable in local variable stack
static int 
resolveLocal(Compiler* compiler, Token* name) {
    for (int i = compiler->localCount - 1; i >= 0; i--) {
        Local* local = &compiler->locals[i];
        if (identifiersEqual(name, &local->name)) {

            // don`t read variable hasn`t initialized.
            if (local->depth == -1) {
                error("Can't read local variable in its own initializer.");
            }
            return i;
        }
    }

    return -1;
}

// adding the upvalue to upvalues array, then
// returning index of it in upvalues array.
static int 
addUpvalue(Compiler* compiler, uint8_t index,
                      bool isLocal) {
    int upvalueCount = compiler->function->upvalueCount;

    // don`t add same upvalue
    for (int i = 0; i < upvalueCount; i++) {
        Upvalue* upvalue = &compiler->upvalues[i];
        if (upvalue->index == index && upvalue->isLocal == isLocal) {
            return i;
        }
    }

    if (upvalueCount == UINT8_COUNT) {
        error("Too many closure variables in function.");
        return 0;
    }

    compiler->upvalues[upvalueCount].isLocal = isLocal;
    compiler->upvalues[upvalueCount].index = index;
    return compiler->function->upvalueCount++;
}

// get index of local variable if the compiler isn`t outermost.
// add that to upvalues and return index of that in upvalues array.
static int 
resolveUpvalue(Compiler* compiler, Token* name) {
    if (compiler->enclosing == NULL) return -1;

    int local = resolveLocal(compiler->enclosing, name);
    if (local != -1) {
        return addUpvalue(compiler, (uint8_t)local, true);
    }

    // search targeted upvalue recursively
    int upvalue = resolveUpvalue(compiler->enclosing, name);
    if (upvalue != -1) {
        return addUpvalue(compiler, (uint8_t)upvalue, false);
    }

    return -1;
}

// add local variable to local variable stack 
static void 
addLocal(Token name) {
    if (current->localCount == UINT8_COUNT) {
        error("Too many local variables in function.");
        return;
    }
    Local* local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1;
    local->isCaptured = false;
}

// invoke declareVariable to parse local variable.
// OtherwiseS add global variable to constant table directly.
static uint8_t 
parseVariable(const char* errorMessage) {
    consume(TOKEN_IDENTIFIER, errorMessage);
    declareVariable();
    if (current->scopeDepth > 0) return 0;
    return identifierConstant(&parser.previous);
}

// mark variable as initialized
static void 
markInitialized() {
    if (current->scopeDepth == 0) return;
    current->locals[current->localCount - 1].depth =
        current->scopeDepth;
}

// define global variable
// emit OP_DEFINE_GLOBAL
static void 
defineVariable(uint8_t global) {
    if (current->scopeDepth > 0) {
        markInitialized();
        return;
    }
    emitBytes(OP_DEFINE_GLOBAL, global);
}

// parse and operator
static void 
and_(bool canAssign) {
    int endJump = emitJump(OP_JUMP_IF_FALSE);

    emitByte(OP_POP);
    parsePrecedence(PREC_AND);

    patchJump(endJump);
}

// parse local variable through invoking addLocal.
// validate the name of variable.
static void 
declareVariable() {
    if (current->scopeDepth == 0) return;

    Token* name = &parser.previous;
    for (int i = current->localCount - 1; i >= 0; i--) {

        // don't allow same name in one scope
        Local* local = &current->locals[i];
        if (local->depth != -1 && local->depth < current->scopeDepth) {
            break; 
        }

        if (identifiersEqual(name, &local->name)) {
            error("Already a variable with this name in this scope.");
        }
    }
    addLocal(*name);
}

// helper function to parse arguments
static uint8_t 
argumentList() {
    uint8_t argCount = 0;
    if (!check(TOKEN_RIGHT_PAREN)) {
        do {
            expression();
        if (argCount == 255) {
            error("Can't have more than 255 arguments.");
        }
        argCount++;
        } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    return argCount;
}

// get parse function
static ParseRule* 
getRule(TokenType type) {
    return &rules[type];
}

// compeiler of clox
ObjFunction*
compile(const char* source) {
    initScanner(source);
    Compiler compiler;
    initCompiler(&compiler, TYPE_SCRIPT);
    parser.hadError = false;
    parser.panicMode = false;

    advance();

    while (!match(TOKEN_EOF)) {
        declaration();
    }

    ObjFunction* function = endCompiler();
    return parser.hadError ? NULL : function;
}