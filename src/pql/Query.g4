grammar Query;

AND          : 'and';
OR           :  'or';

// comparison operators
EQ           :  '=';
CONTAINS     : '~';
GT           : '>';
GTE          : '>=';
LT           : '<';
LTE          : '<=';

WS           : [ \t\r\n]+ -> skip ;
INT          : '-'? [0-9]+ ;
FLOAT        : '-'? [0-9]+'.'[0-9]+ ;
STRING       : '"' .*? '"' ;

SIZE_SUFFIX  : 'kb'|'mb'|'gb';

ID           : [a-zA-Z_] [a-zA-Z_0-9.]*;

filter
    : expression
    ;

expression
    : expression AND expression #AndExpression
    | expression OR  expression #OrExpression
    | predicate                 #PredicateExpression
    ;

reference: ID;

predicate
    : reference oper value #OperatorPredicate
    ;

oper
    : EQ
    | CONTAINS
    | GT
    | GTE
    | LT
    | LTE
    ;

value
    : INT SIZE_SUFFIX?
    | FLOAT
    | STRING
    ;
