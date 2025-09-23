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
    def __init__(self, tokenType:int, lexeme:str):
        self.tokenType = tokenType
        self.lexeme = lexeme


class Compiler:
    def __init__(self):
        self.sourceCode:str = ''
        self.tokens:list = []
        self.previousToken:Token = Token(-1, '')
        self.currentToken:Token = Token(-1, '')
        self.tokenPointer = 0
        self.repl()


    def repl(self):
        while True:
            print('pratt>', end='')
            self.sourceCode:str = input()
            if self.sourceCode == 'q':
                break
            
            compile()


    def compile(self):
        self.tokens:list = self.tokenizer(self.sourceCode)
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

    def parse_precedence(prec:int):
        
    


def tokenizer(sourceCode:str)->list:
    # Lazy so just split on WS
    return sourceCode.split()

if __name__ == '__main__':
    compiler = Compiler()