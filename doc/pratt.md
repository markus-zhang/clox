```
main.c -> combineMultipleLine(): 2+7/1

Name of chunk: code
Offset -> 0000 Line    0 - Pos    4 OP_CONSTANT      Index    0 -> '2'
Offset -> 0002 Line    0 - Pos    4 OP_CONSTANT      Index    1 -> '7'
Offset -> 0004 Line    0 - Pos    4 OP_CONSTANT      Index    2 -> '1'
Offset -> 0006 Line    0 - Pos    4 OP_DIV
Offset -> 0007 Line    0 - Pos    4 OP_ADD
```

The idea of Pratt Parser is:
- You have prefix operators such as `!`, `-` and `(` -> last one is for grouping
- You also have infix operators such as `+`, `-`, `*` and `/`
- You know the precedence of these operators
- So the idea is:

```Python
def Compile():
    # We need two tokens to make decisions
    previous_token = current_token
    current_token = GetNextToken()

    ParsePrecedence(PREC_ASSIGNMENT)    # lowest precedence other than PREC_NONE which is for EOF

def ParsePrecedence(prec):
    # This is where to advance again, because you want to compare precedence in other functions 
    # We also save the newly acquired current_token so it is not lost/skipped
    previous_token = current_token
    current_token = GetNextToken()

    prefix_fp = GetPrefixFunc(previous_token)
    prefix_fp()

    # Then we keep going to the right until we hit a HIGHER precedence
    # For example, think 3+4+5/2, we keep going until we hit /
    # The thing is, at the end of the day, we want to have / at the top of the stack
    # so we want to "drag" the emit of operators so that they get pushed AFTER the operands get pushed

    # At this point, prec is passed from the previous op, which is already not visible
    # previous_token should point to the right operands of previous op
    # current_token should point to the next op
    while (prec <= GetRecedence(current_token))
    {
        previous_token = current_token
        current_token = GetNextToken()

        infix_fp = GetInfixFunc(previous_token)
        infix_fp()
    }

def prefix_fp():
    # Right now we only have number() or unary() or grouping()
    if NUMBER:
        PushConstant(previous_token)
    elif UNARY:
        # For unary, should always recursively call ParsePrecedence()
        # Save the unary operator first
        op = previous_token.type
        ParsePrecedence(PREC_UNARY)
        # Once right side is pushed onto stack, push the operator
        PushOp(op)
    elif GROUPING:
        # We have expressions in () so recursively call ParsePrecedence and start from the lower precedence again
        ParsePrecedence(PREC_ASSIGNMENT)
        # Make sure it has a right parenthesis trailing
        Consume(')')

def infix_fp():
    # Save the operator first because we need to emit it later
    op = previous_token.type
    # Make sure that we recursively call ParsePrecedence() for the RHS with the correct precedence
    if ADD:
        ParsePrecedence(PREC_ADD)
        PushOP(op)
    elif SUB:
        ParsePrecedence(PREC_SUB)
        PushOP(op)
    elif MUL:
        ParsePrecedence(PREC_MUL)
        PushOP(op)
    elif DIV:
        ParsePrecedence(PREC_DIV)
        PushOP(op)
        
```

A few key issues:
- In prefix and infix functions, if it's an operator, then you should always call parsePrecedence() recursively, because the RHS may have more stuffs, and THEN emit the operators -> remember that in a stack (RPN), operator is at the top, like (+ 7 2)
- Since we are always saving two tokens, previous and current, make sure that you update them properly with advance(), and refernece them properly. Sometimes you need previous and sometimes you need current

Parser uses `advance()` to ask for the next Token, setting precedence to PREC_ASSIGNMENT (This is called from `expression()` into `parsePrecedence()`)
- `advance()` calls `scanToken()`
- `previous` = garbage and `current` = 2

    `parsePrecedence()` 
    - calls `advance()` to grab the next Token, so previous is now 2 and current is +
    - calls `prefixRule()`, which is `number()` function
    - `number()` pushes `OP_CONSTANT`, and the value 2 onto `compilingChunk->code`
    
    ```
    Stack   -> 2
            -> OP_CONSTANT
    ```

    - `parsePrecedence()` then look at `current`, which is +, which has higher precedence than `PREC_ASSIGNMENT`
    - calls `advance()` to grab the next Token, so previous is now + and current is 7
    - Checks `infixRule`, which is `binary` for `TOKEN_PLUS`
    - run `infixRule()`, which is `binary()` -> Recall that both prefixRule() and infixRule() are functional pointers

        `binary()` takes the op, which is +. It fetches its precedence and recursively called `parsePrecedence(PREC_TERM)`

            `parsePrecedence()`
            - calls `advance()`, so previous is 7 and current is /
            - calls `prefixRule()`, which is `number()`
            - `number()` pushes `OP_CONSTANT`, and the value 7 onto `compilingChunk->code`

            ```
            Stack   -> 7
                    -> OP_CONSTANT
                    -> 2
                    -> OP_CONSTANT
            ```

            - `parsePrecedence()` then look at `current`, which is /, which has higher precedence than `PREC_TERM`
            - call `advance()` to grab the next Token, so previous is / and current is 1
            - call `infixRule()` which is `binary()` again

                `binary()` takes the op, which is /. It fetches its precedence and calls `parsePrecedence(PREC_FACTOR)`

                    `parsePrecedence()`
                    - calls `advance()`, so previous is 1 and current is EOF
                    - calls `prefixRule()`, which is `number()`
                    - `number()` pushes OP_CONSTANT`, and value 1 onto `compilingChunk->code`

                    ```
                    Stack   -> 1
                            -> OP_CONSTANT
                            -> 7
                            -> OP_CONSTANT
                            -> 2
                            -> OP_CONSTANT
                    ```
                    
                    - `parsePrecedence()` then look at `current`, which is EOF, which has LOWER precedence than `PREC_FACTOR`
                    return to `binary()`

                - emits `OP_SLASH` as op is / in `binary()`

                ```
                Stack   -> OP_SLASH
                        -> 1
                        -> OP_CONSTANT
                        -> 7
                        -> OP_CONSTANT
                        -> 2
                        -> OP_CONSTANT
                ```
                return to `parsePrecedence()`

            - `parsePrecedence()` then look at `current`, which is EOF, which has LOWER precedence than `PREC_TERM`
            return to `binary()`

        - emits `OP_PLUS` as op is + in `binary()`

        ```
        Stack   -> OP_PLUS
                -> OP_SLASH
                -> 1
                -> OP_CONSTANT
                -> 7
                -> OP_CONSTANT
                -> 2
                -> OP_CONSTANT
        ```
        return to `parsePrecedence()`

    - `parsePrecedence()` then look at `current`, which is EOF, which has LOWER precedence than `PREC_ASSIGNMENT`
    return to `expression()`

return to `compiler()`, which then consumes EOF (success, as the current token is EOF), then calls `endCompiler()`

