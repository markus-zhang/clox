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
    - 

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

