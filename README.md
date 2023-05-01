DESIGN
The design consists of two distinct set of components – for the two compiler phases, lexical analysis and syntax check.

Lexical Analyser
Six types of tokens are defined using an enum.
enum class token_type
{
    identifier,
    keyword,
    numeric_constant,
    character_constant,
    string_constant,
    other // operators, delimiters, etc.
};
These token types capture all the tokens defined in ANSI C language.

The identified tokens are stored in a linked list with one node per token. The linked list node is stored using the token structure. The data members in the structure are shown below.
struct token
{
    token_type type;
    string chars;  // characters forming the token
    token* p_next;
};

The lexical analysis is done by the lexical_analyzer class. Following are the significant public members in the class.
class lexical_analyzer
{
    bool is_valid;
    token* first_token;

    void add_tokens(string& source_line);
    void complete();
};

The add_tokens() function should be called once for each line in the source code. The complete() function should be called at the end, after calling add_tokens() for all lines.

The is_valid member will indicate if the lexical analysis was successful or not. It is updated after every call to add_tokens() and after the call to complete().

The first_token member will have the pointer to the head of the linked list of tokens after complete() is successful.

The add_tokens() function contains all the logic required for identifying tokens. It uses the approach of looking ahead up to three characters to identify tokens.

Error detection
The lexical analyser uses the approach of stopping at first error. Following are the errors detected.
-	Invalid characters outside character/string constants like $ @ # \ 
-	Invalid character constant
-	Invalid numeric constant
-	Unterminated string constant
-	End of file reached without closing multiline comment

Limitations
The lexical analyser does not support the following. All of them are related to constant tokens.
-	Octal and hexadecimal integer constants
-	Floating-point constant exponential notation
-	Suffixes in numeric constants
-	Escape sequences in character/string constants 
-	Wide character constants

Syntax Analyser
The syntax analyser produces the parse tree taking the list of tokens produced by lexical analyser as input. Nodes in the parse tree are stored using the token structure. The data members in the structure are shown below.
struct node
{
	const char* node_type;
	const token* node_token;
	vector<node*> children;
};
The node_type member holds the type of the node in case non-terminal symbols in the grammar. Tokens are the terminal symbols in the grammar. The node_token member holds the pointer to the first token of the node.

The syntax analysis is done by the parser class. The only public member in the class is parse() function:
node* parse(token* first_token)

The function takes the list of tokens as argument and returns the root of the parse tree. In case of error in parsing, it returns null.

Parsing functions – matching non-terminals
The parsing logic is implemented using member functions in the parser class. Each non-terminal in the grammar is represented by one parsing member function. Each parsing function handles the logic for all productions of one non-terminal.

Following are the parsing functions for the top-level non-terminal, root, and the next level non terminal, external_declaration.

void parser::root()
{
	curr_node->node_type = "root";

	prod(&parser::external_declaration);

	while (!match_end(true))
		prod(&parser::external_declaration);
}

void parser::external_declaration()
{
	curr_node->node_type = "external_declaration";

	auto matched = prod(&parser::variable_declaration, true);

	if (!matched) prod(&parser::function_definition);
}

The above parsing functions represent the following productions in the grammar.
root:
external_declaration
root external_declaration

external_declaration:
variable_declaration
function_definition

The parsing function refer to other non-terminals by invoking the parsing function corresponding to that non-terminal. This is done using the prod() function.
bool prod(void (parser::*prod_func)(), bool optional = false)

The prod() function takes the parsing function pointer as argument and invokes it. It takes care of the recovery mechanism when the parsing function does not find a match. The optional argument indicates whether an exception is thrown if a match is not found by the parsing function. If optional is false, an exception is thrown. The exception is handled at the top level by parse() function. If optional is true and a match is not found, prod() returns false instead of throwing an exception.

Matching terminals
The tokens are terminals in the grammar. Terminals are matched using the following functions.
Function	Purpose
bool match_token_type(token_type type, bool optional = false);
	Matches token based on token type.
bool match_keyword(const char* keyword, bool optional = false);
	Matches keyword token.
bool match_token(const char* token_chars, bool optional = false);
	Matches an other token based on the token characters.
bool match_end(bool optional = false);	Matches end of file.

Error detection
The syntax analyser uses the approach of stopping at first error. No explicit error message is returned there may be multiple possible reasons why a mismatch may occur.

Limitations
The grammar supported is a subset of the ANSI C grammar.
 
2	LANGUAGE GRAMMAR
The grammar supported is a subset of ANSI C grammar. The grammar is based on the grammar in “The C Programming Language” by Kernighan & Ritchie (second edition). The naming conventions used in the grammar closely match the grammar section A3 of the book. Following are the non-terminals defined in the grammar.
root	logical_or_expression
external_declaration	logical_and_expression
variable_declaration	inclusive_or_expression
function_definition	exclusive_or_expression
type_name	and_expression
compound_statement	equality_expression
statement	relational_expression
expression_statement	shift_expression
selection_statement	additive_expression
iteration_statement	multiplicative_expression
jump_statement	cast_expression
labeled_statement	unary_expression
constant_expression	postfix_expression
expression	primary_expression
assignment_expression	argument_list
assignment_operator	constant
assignment	

Following are some of the significant features not supported by the simplified C language grammar used.
-	Structures and Unions
-	Pointers
-	Arrays


 
3	CODE STRUCTURE
The structure of the code is shown below.
 

lex.h – Header for lexical analysis
parser.h – Header for syntax analysis
lex.cpp – Logic for lexical analysis
parser.cpp – Common logic for syntax analysis
grammar.cpp – Parse functions for non-terminals in the grammar
main.cpp – Main driver code

C++ header files used: iostream fstream iomanip string cctype vector

The source code is available in Github at .
 
4	RUNNING THE PARSER
The parser (includes lexical analyser) once compiled into an executable has no external dependencies. It takes a single argument as input – path of the input source file. The parser can be run using the below command line.

Parser.exe input_source.c

The output from the parser is written to the console. Lexical analysis output is printer as a list of tokens and the parser output is printed in textual tree format.
5	TEST INPUT AND OUTPUT
Input source file content

int a;

long b, c; // globals

/* function to calculate
sum of two numbers */
int calc_sum(float x, float y)
{
	int sum;

	sum = x + y;

	return sum /* x+y */;
}

int is_digit(char ch)
{
	return ch >= 0 && ch <= 9;
}

int main()
{
	int a, b;

	a = calc_sum(5, 6.5);
	b = is_digit('x');

	return 0;
}

  
RESULT

The Mini C Language Parser project has demonstrated the core lexical analyser and syntax analyser design techniques. The method can be extended for enhanced language grammar support. Though it is more difficult to build and maintain a parser using this full custom code approach, it gives complete control over the parsing process when compared to generating lexical analyser and parser code from grammar using tools like lex and yacc. This approach can be used for specialized languages with parsing needs that do not fit into restrictions imposed by code generation tools.
