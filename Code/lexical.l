%{
   #include "syntax.tab.h"
   #include "TypeInfo.h"
   #include "SymbolTable.h"
   #include "common.h"
   extern bool struct_defining;

   int A_error; 

   int yycolumn;
   #define YY_USER_ACTION \
     yylloc.first_line = yylloc.last_line = yylineno; \
	 yylloc.first_column = yycolumn; \
	 yylloc.last_column = yycolumn + yyleng - 1; \
	 yycolumn += yyleng; 
%}

/* regular definitions */
ws     [ \t\r]+
letter [A-Za-z]
digit  [0-9]
digits {digit}+
INT    {digits}
FLOAT  {digits}(\.{digits})+([Ee][+-]?{digits})?
id     (_|{letter})({letter}|_|{digit})*
%option yylineno

%%
{ws}    {/* no action */}
\n      { yycolumn = 1; } 
if      { return IF; }
else    { return ELSE; }
while   { return WHILE; }
return  { return RETURN; }
struct  { struct_defining = true; return STRUCT; }
int     { yylval.type_int = INTEGER; return TYPE; }
float   { yylval.type_int = DECIMAL; return TYPE; }
{id}    { yylval.type_str = yytext; return ID; } 
{INT}   { yylval.type_int = atoi(yytext); return INT; }
{FLOAT} { yylval.type_float = (atof(yytext)); return FLOAT; } 
"."       { return DOT; }
";"       { return SEMI; }
","       { return COMMA; }
"="       { return ASSIGNOP; }
">"       { yylval.type_int = '>'; return RELOP; }
"<"       { yylval.type_int = '<'; return RELOP; }
">="      { yylval.type_int = '.'; return RELOP; }
"<="      { yylval.type_int = ','; return RELOP; }
"=="      { yylval.type_int = '='; return RELOP; }
"!="      { yylval.type_int = '!'; return RELOP; }
"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return STAR; }
"/"       { return DIV; }
"&&"      { return AND; }
"||"      { return OR; }
"!"       { return NOT; }
"("       { return LP; }
")"       { return RP; }
"["       { return LB; }
"]"       { return RB; }
"{"       { enterField(); return LC;}
"}"       { return RC; }
"//"      { char c = input();
            while(c != '\n') c = input();
          }
"/*"      { char c0 = input();
            char c1 = input();
            while(c0 != '*' && c1 != '/') {
              c0 = c1;
              c1 = input();
            }
          }
.         {
           A_error = 1;
       	   printf("Error type A at line %d: unknown characters %s\n", yylineno, yytext);
          }

%%




