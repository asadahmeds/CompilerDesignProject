#pragma once

#include <string>
#include <vector>

#include "lex.h"

struct node
{
	const char* node_type = nullptr;
	const token* node_token;
	vector<node*> children;

	node(const token* node_token) : node_token(node_token) { }

	node* add_child(token* node_token);

	void print(int indent = 0);

	~node();
};

class parser
{
	node* curr_node = nullptr;
	token* curr_token = nullptr;

	bool prod(void (parser::*prod_func)(), bool optional = false);

	bool match_token_type(token_type type, bool optional = false);
	bool match_keyword(const char* keyword, bool optional = false);
	bool match_token(const char* token_chars, bool optional = false);
	bool match_end(bool optional = false);

	void root();
	
	void external_declaration();
	void variable_declaration();
	void function_definition();
	void type_name();
	void compound_statement();
	void statement();
	void expression_statement();
	void selection_statement();
	void iteration_statement();
	void jump_statement();
	void labeled_statement();
	void constant_expression();
	void expression();
	void assignment_expression();
	void assignment_operator();
	void assignment();
	void logical_or_expression();
	void logical_and_expression();
	void inclusive_or_expression();
	void exclusive_or_expression();
	void and_expression();
	void equality_expression();
	void relational_expression();
	void shift_expression();
	void additive_expression();
	void multiplicative_expression();
	void cast_expression();
	void unary_expression();
	void postfix_expression();
	void primary_expression();
	void argument_list();
	void constant();

public:

	node* parse(token* first_token);
};