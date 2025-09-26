"""
    A research script that demonstrates Pratt's parser:

    1. Very simple language
    - all tokens separated by whitespaces, even strings (no space allowed in strings, lol), so that I can simply call split()
    - Only supports float numbers
    - Operators:
        - Unary (only OP_NEG)
        - Binary(OP_ADD, OP_SUB, OP_MUL, OP_DIV)
        - Grouping

    2. REPL only, not scripts import allowed
    - And only allow a single line of input :)
"""

# Precedence
PREC_EOF            = -1
PREC_NONE           = 0
PREC_ASSIGNMENT     = 1
PREC_TERM           = 2 # +-
PREC_FACTOR         = 3 # */
PREC_UNARY          = 4 # -
PREC_PRIMARY        = 5

TOKEN_ADD           = 0
TOKEN_SUB           = 1
TOKEN_MUL           = 2
TOKEN_DIV           = 3
TOKEN_NUM           = 4
TOKEN_STR           = 5
TOKEN_EOF           = 6

class Token:
    def __init__(self, tokenType:int, lexeme:str, prec:int):
        self.tokenType = tokenType
        self.lexeme = lexeme
        # Only for operators
        self.prec = prec


class Compiler:
    def __init__(self):
        self.sourceCode:str = ''
        self.tokens:list = []
        self.previousToken:Token = Token(-1, '', PREC_NONE)
        self.currentToken:Token = Token(-1, '', PREC_NONE)
        self.tokenPointer = 0
        # emulate the stack
        self.stack = []
        # self.repl()
        self.compile()
        print(self.stack)


    def repl(self):
        while True:
            print('pratt>', end='')
            self.sourceCode:str = input()
            if self.sourceCode == 'q':
                break
            
            compile()


    def compile(self):
        # self.tokens:list = self.tokenizer(self.sourceCode)
        # Let's hardcode something first
        self.tokens:list[Token] = [
            Token(TOKEN_NUM, '1', PREC_NONE), 
            Token(TOKEN_ADD, '+', PREC_TERM), 
            Token(TOKEN_NUM, '3', PREC_NONE),
            Token(TOKEN_NUM, '/', PREC_FACTOR),
            Token(TOKEN_NUM, '2', PREC_NONE),
            Token(TOKEN_EOF, '', PREC_EOF)
        ]
        self.advance()
        self.expression()
        self.consume(TOKEN_EOF, "Expect end of expression")


    def advance(self):
        self.previousToken:Token = self.currentToken
        self.currentToken:Token = self.tokens[self.tokenPointer]
        self.tokenPointer += 1

    def consume(self, tokenType:int, message:str):
        if self.currentToken.tokenType == tokenType:
            self.advance()
        else:
            print(message)

    def expression(self):
        self.parse_precedence(PREC_ASSIGNMENT)

    def parse_precedence(self, prec:int):
        self.advance()
        # Case 1: Unary minus
        print(self.previousToken)
        if self.previousToken.tokenType == TOKEN_SUB:
            self.prefix(TOKEN_SUB)
        
        # TODO: Figure out a way to match tokenType to precedence, maybe simply add the prec into Token class?
        # NOTE: Actually, why do we need two tokens? Why can't we simply track the current token? 
        while prec <= self.currentToken.prec:
            self.advance()
            self.infix(self.previousToken.prec)
            

    def prefix(self, tokenType:int):
        # Parse the next token first (most likely a number or a variable or a function call)
        self.parse_precedence(PREC_UNARY)
        # We always want to append the operator last (top of stack)
        if tokenType == TOKEN_SUB:
            self.stack.append('-')

    def infix(self, tokenType:int):
        # The only infix rule is binary, but different op has different prec so we need to put everything into if-elif-else
        # Unlike in prefix() we can call parse_precendence() immediately
        if tokenType == TOKEN_ADD:
            self.parse_precedence(PREC_TERM + 1)
            self.stack.append('+')
        elif tokenType == TOKEN_SUB:
            self.parse_precedence(PREC_TERM + 1)
            self.stack.append('-')

    
    def tokenizer(self)->list:
        # Lazy so just split on WS
        split = self.sourceCode.split()
        prec = PREC_NONE
        for s in split:
            if s in ('+', '-'):
                pass

if __name__ == '__main__':
    compiler = Compiler()