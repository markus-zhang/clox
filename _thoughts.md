let's say we have = 8+3*(1-5)*10-8

After parsing = we see that the RHS is an expression so we call expression(precedence = PREC_ASSIGNMENT)

    First we reach 8, push 8 onto stack;

    stack -> 8

    Then we reach +, op = '+', push '+' onto stack, and since PREC_ADD > PREC_ASSIGNMENT, call expression(precedence = PREC_ADD);

    stack -> +
             8

        First we reach 3, push 3 onto stack;

        stack -> 3
                 +
                 8

        Then we reach *, since PREC_MUL > PREC_ADD, call expression(precedence = PREC_MUL);

        stack -> *
                 3
                 +
                 8
        
            First we reach '(', since PREC_CALL > PREC_MUL, call expression(precedence = PREC_CALL);

                First we reach 1, push 1 onto stack;

                stack -> 1
                         *
                         3
                         +
                         8
                
                Then we reach -, since PREC_SUB < PREC_CALL, 



8+3*(1-5)*10-8

(- (+ 8 (* 3 ())) 8)