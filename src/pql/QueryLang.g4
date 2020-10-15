grammar QueryLang;

AND          : 'and';
OR           :  'or';
EQ           :  '=';
CONTAINS     : '~';
GREATER_THAN : '>';
WS           : [ \t\r\n]+ -> skip ;
INT          : '-'? [0-9]+ ;
DOUBLE       : '-'? [0-9]+'.'[0-9]+ ;
STRING       : '"' .*? '"' ;
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
    | GREATER_THAN
    ;

value
    : INT
    | DOUBLE
    | STRING
    ;
