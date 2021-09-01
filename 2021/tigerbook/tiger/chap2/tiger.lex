type pos = int
type lexresult = Tokens.token

val lineNum = ErrorMsg.lineNum
val linePos = ErrorMsg.linePos
val err = ErrorMsg.error
val bug = ErrorMsg.impossible

val stringLiteral = ref ""

fun ToInt(s) = foldl (fn(a, r) => ord(a) - ord(#"0") + 10 * r) 0 (explode s)

datatype State = REGULAR | COMMENT_SEEN | STRING_SEEN
val state = ref REGULAR

fun eof() = let 
	val pos = hd(!linePos) 
	in 
		(case !state of
		  COMMENT_SEEN => err pos "missing comment terminator"
		| STRING_SEEN => err pos " missing string terminator"
		| _ => (); 
		Tokens.EOF(pos,pos))
	end

%% 
digits=[0-9]+;
identifier=[a-zA-Z][_a-zA-Z0-9]*;
ws=[ \t];
%s COMMENT STRING;
%%
<INITIAL>"while"	=> (Tokens.WHILE(yypos,yypos+size yytext));
<INITIAL>"for"		=> (Tokens.FOR(yypos,yypos+size yytext));
<INITIAL>"to"		=> (Tokens.TO(yypos,yypos+size yytext));
<INITIAL>"break"	=> (Tokens.BREAK(yypos,yypos+size yytext));
<INITIAL>"let"		=> (Tokens.LET(yypos,yypos+size yytext));
<INITIAL>"in"		=> (Tokens.IN(yypos,yypos+size yytext));
<INITIAL>"end"		=> (Tokens.END(yypos,yypos+size yytext));
<INITIAL>"function"	=> (Tokens.FUNCTION(yypos,yypos+size yytext));
<INITIAL>"var"		=> (Tokens.VAR(yypos,yypos+size yytext));
<INITIAL>"type"		=> (Tokens.TYPE(yypos,yypos+size yytext));
<INITIAL>"array"	=> (Tokens.ARRAY(yypos,yypos+size yytext));
<INITIAL>"if"		=> (Tokens.IF(yypos,yypos+size yytext));
<INITIAL>"then"		=> (Tokens.THEN(yypos,yypos+size yytext));
<INITIAL>"else"		=> (Tokens.ELSE(yypos,yypos+size yytext));
<INITIAL>"do"		=> (Tokens.DO(yypos,yypos+size yytext));
<INITIAL>"of"		=> (Tokens.OF(yypos,yypos+size yytext));
<INITIAL>"nil"		=> (Tokens.NIL(yypos,yypos+size yytext));

<INITIAL>","		=> (Tokens.COMMA(yypos,yypos+1));
<INITIAL>":"		=> (Tokens.COLON(yypos,yypos+1));
<INITIAL>";"		=> (Tokens.SEMICOLON(yypos,yypos+1));
<INITIAL>"("		=> (Tokens.LPAREN(yypos,yypos+1));
<INITIAL>")"		=> (Tokens.RPAREN(yypos,yypos+1));
<INITIAL>"["		=> (Tokens.LBRACK(yypos,yypos+1));
<INITIAL>"]"		=> (Tokens.RBRACK(yypos,yypos+1));
<INITIAL>"{"		=> (Tokens.LBRACE(yypos,yypos+1));
<INITIAL>"}"		=> (Tokens.RBRACE(yypos,yypos+1));
<INITIAL>"."		=> (Tokens.DOT(yypos,yypos+1));
<INITIAL>"+"		=> (Tokens.PLUS(yypos,yypos+1));
<INITIAL>"-"		=> (Tokens.MINUS(yypos,yypos+1));
<INITIAL>"*"		=> (Tokens.TIMES(yypos,yypos+1));
<INITIAL>"/"		=> (Tokens.DIVIDE(yypos,yypos+1));
<INITIAL>"="		=> (Tokens.EQ(yypos,yypos+1));
<INITIAL>"<>"		=> (Tokens.NEQ(yypos,yypos+2));
<INITIAL>"<"		=> (Tokens.LT(yypos,yypos+1));
<INITIAL>"<="		=> (Tokens.LE(yypos,yypos+2));
<INITIAL>">"		=> (Tokens.GT(yypos,yypos+1));
<INITIAL>">="		=> (Tokens.GE(yypos,yypos+2));
<INITIAL>"&"		=> (Tokens.AND(yypos,yypos+1));
<INITIAL>"|"		=> (Tokens.OR(yypos,yypos+1));
<INITIAL>":="		=> (Tokens.ASSIGN(yypos,yypos+2));
<INITIAL>{digits} 	=> (Tokens.INT(ToInt yytext, yypos, yypos+size yytext));
<INITIAL>{identifier} 	=> (Tokens.ID(yytext, yypos, yypos+size yytext));

<INITIAL>\"    		=>  (YYBEGIN STRING; 
			     stringLiteral:= ""; 
			     state := STRING_SEEN;
			     continue());
<STRING>\"     		=>  (YYBEGIN INITIAL; 
			     state := REGULAR;
			     Tokens.STRING(!stringLiteral, yypos, 0));
<STRING>\\\\     	=>  (stringLiteral := !stringLiteral ^ "\\"; 
			     continue());
<STRING>\\\"     	=>  (stringLiteral := !stringLiteral ^ "\""; 
			     continue());
<STRING>\\t     	=>  (stringLiteral := !stringLiteral ^ "\t"; 
			     continue());
<STRING>.     		=>  (stringLiteral := !stringLiteral ^ yytext; 
			     continue());
<INITIAL>"/*"    	=> (YYBEGIN COMMENT; 
			    continue());
<COMMENT>"*/"    	=> (YYBEGIN INITIAL; 
			    continue());
<COMMENT>.       	=> (continue());

<INITIAL>{ws} 		=> (continue());
<INITIAL>\n		=> (lineNum := !lineNum+1; 
			    linePos := yypos :: !linePos; 
			    continue());
<INITIAL>.       	=> (err yypos ("illegal character " ^ yytext); 
			    continue());
