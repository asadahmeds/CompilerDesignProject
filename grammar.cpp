
#include "parser.h"

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

void parser::variable_declaration()
{
	curr_node->node_type = "variable_declaration";

	prod(&parser::type_name);
	match_token_type(token_type::identifier);

	while (match_token(",", true))
	{
		match_token_type(token_type::identifier);
	}

	match_token(";");
}

void parser::type_name()
{
	curr_node->node_type = "type_name";

	if (match_keyword("int", true)) return;
	if (match_keyword("long", true)) return;
	if (match_keyword("float", true)) return;
	if (match_keyword("double", true)) return;
	match_keyword("char");
}

void parser::function_definition()
{
	curr_node->node_type = "function_definition";

	if (!match_keyword("void", true))
	{
		prod(&parser::type_name);
	}

	match_token_type(token_type::identifier);

	match_token("(");

	if (prod(&parser::type_name, true))
	{
		match_token_type(token_type::identifier);

		while (match_token(",", true))
		{
			prod(&parser::type_name);
			match_token_type(token_type::identifier);
		}
	}

	match_token(")");

	prod(&parser::compound_statement);
}

void parser::compound_statement()
{
	curr_node->node_type = "compound_statement";

	match_token("{");

	while (prod(&parser::variable_declaration, true));
	while (prod(&parser::statement, true));

	match_token("}");
}

void parser::statement()
{
	curr_node->node_type = "statement";

	if (prod(&parser::labeled_statement, true)) return;
	if (prod(&parser::expression_statement, true)) return;
	if (prod(&parser::compound_statement, true)) return;
	if (prod(&parser::selection_statement, true)) return;
	if (prod(&parser::iteration_statement, true)) return;
	prod(&parser::jump_statement);
}

void parser::expression_statement()
{
	curr_node->node_type = "expression_statement";

	prod(&parser::expression, true);

	match_token(";");
}

void parser::selection_statement()
{
	curr_node->node_type = "selection_statement";

	auto if_stmt = match_keyword("if", true);
	if (!if_stmt) match_keyword("switch");

	match_token("(");
	prod(&parser::expression);
	match_token(")");
	prod(&parser::statement);

	if (if_stmt)
	{
		if (match_keyword("else", true))
			prod(&parser::statement);
	}
}

void parser::iteration_statement()
{
	curr_node->node_type = "iteration_statement";

	if (match_keyword("while", true))
	{
		match_token("(");
		prod(&parser::expression);
		match_token(")");
		prod(&parser::statement);
	}
	else if (match_keyword("do", true))
	{
		prod(&parser::statement);
		match_keyword("while");
		match_token("(");
		prod(&parser::expression);
		match_token(")");
		match_token(";");
	}
	else
	{
		match_keyword("for");
		match_token("(");
		prod(&parser::expression, true);
		match_token(";");
		prod(&parser::expression, true);
		match_token(";");
		prod(&parser::expression, true);
		match_token(")");
		prod(&parser::statement);
	}
}

void parser::jump_statement()
{
	curr_node->node_type = "jump_statement";

	if (match_keyword("goto", true))
	{
		match_token_type(token_type::identifier);
	}
	else if (match_keyword("return", true))
	{
		prod(&parser::expression, true);
	}
	else if (match_keyword("continue", true))
	{
		// no more tokens
	}
	else
	{
		match_keyword("break");
	}

	match_token(";");
}

void parser::labeled_statement()
{
	curr_node->node_type = "labeled_statement";

	if (match_keyword("case", true))
	{
		prod(&parser::constant_expression);
		match_token(":");
		prod(&parser::statement);
	}
	else if (match_keyword("default", true))
	{
		prod(&parser::statement);
	}
	else
	{
		match_token_type(token_type::identifier);
		match_token(":");
		prod(&parser::statement);
	}
}

void parser::constant_expression()
{
	curr_node->node_type = "constant_expression";
	prod(&parser::logical_or_expression);
}

void parser::expression()
{
	curr_node->node_type = "expression";

	prod(&parser::assignment_expression);

	while (match_token(",", true))
	{
		prod(&parser::assignment_expression);
	}
}

void parser::assignment_expression()
{
	curr_node->node_type = "assignment_expression";

	if (prod(&parser::assignment, true)) return;

	prod(&parser::logical_or_expression);
}

void parser::assignment()
{
	match_token_type(token_type::identifier);
	prod(&parser::assignment_operator);
	prod(&parser::logical_or_expression);
}

void parser::assignment_operator()
{
	curr_node->node_type = "assignment_operator";

	if (match_token("=", true)) return;
	if (match_token("*=", true)) return;
	if (match_token("/=", true)) return;
	if (match_token("%=", true)) return;
	if (match_token("+=", true)) return;
	if (match_token("-=", true)) return;
	if (match_token("<<=", true)) return;
	if (match_token(">>=", true)) return;
	if (match_token("&=", true)) return;
	if (match_token("^=", true)) return;
	match_token("|=");
}

void parser::logical_or_expression()
{
	curr_node->node_type = "logical_or_expression";

	prod(&parser::logical_and_expression);

	while (match_token("||", true))
	{
		prod(&parser::logical_and_expression);
	}
}

void parser::logical_and_expression()
{
	curr_node->node_type = "logical_and_expression";

	prod(&parser::inclusive_or_expression);

	while (match_token("&&", true))
	{
		prod(&parser::inclusive_or_expression);
	}
}

void parser::inclusive_or_expression()
{
	curr_node->node_type = "inclusive_or_expression";

	prod(&parser::exclusive_or_expression);

	while (match_token("|", true))
	{
		prod(&parser::exclusive_or_expression);
	}
}

void parser::exclusive_or_expression()
{
	curr_node->node_type = "exclusive_or_expression";

	prod(&parser::and_expression);

	while (match_token("^", true))
	{
		prod(&parser::and_expression);
	}
}

void parser::and_expression()
{
	curr_node->node_type = "and_expression";

	prod(&parser::equality_expression);

	while (match_token("&", true))
	{
		prod(&parser::equality_expression);
	}
}

void parser::equality_expression()
{
	curr_node->node_type = "equality_expression";

	prod(&parser::relational_expression);

	while (match_token("==", true) || match_token("!=", true))
	{
		prod(&parser::relational_expression);
	}
}

void parser::relational_expression()
{
	curr_node->node_type = "relational_expression";

	prod(&parser::shift_expression);

	while (match_token("<", true) ||
		match_token(">", true) ||
		match_token("<=", true) ||
		match_token(">=", true) )
	{
		prod(&parser::shift_expression);
	}
}

void parser::shift_expression()
{
	curr_node->node_type = "shift_expression";

	prod(&parser::additive_expression);

	while (match_token("<<", true) || match_token(">>", true))
	{
		prod(&parser::additive_expression);
	}
}

void parser::additive_expression()
{
	curr_node->node_type = "additive_expression";

	prod(&parser::multiplicative_expression);

	while (match_token("+", true) || match_token("-", true))
	{
		prod(&parser::multiplicative_expression);
	}
}

void parser::multiplicative_expression()
{
	curr_node->node_type = "multiplicative_expression";

	prod(&parser::cast_expression);

	while (match_token("*", true) || match_token("/", true) || match_token("%", true))
	{
		prod(&parser::cast_expression);
	}
}

void parser::cast_expression()
{
	curr_node->node_type = "cast_expression";

	if (match_token("(", true))
	{
		prod(&parser::type_name);
		match_token("(");
		prod(&parser::unary_expression);
	}
	else
	{
		prod(&parser::unary_expression);
	}
}

void parser::unary_expression()
{
	curr_node->node_type = "unary_expression";

	if (match_token("++", true) ||
		match_token("--", true))
	{
		prod(&parser::unary_expression);
	}
	else if (match_token("+", true) ||
		match_token("-", true) ||
		match_token("~", true) ||
		match_token("!", true))
	{
		prod(&parser::cast_expression);
	}
	else if (match_keyword("sizeof", true))
	{
		match_keyword("sizeof");

		if (match_token("(", true))
		{
			prod(&parser::type_name);
			match_token(")");
		}
		else
		{
			prod(&parser::unary_expression);
		}
	}
	else
	{
		prod(&parser::postfix_expression);
	}
}

void parser::postfix_expression()
{
	curr_node->node_type = "postfix_expression";

	prod(&parser::primary_expression);

	while (true)
	{
		if (match_token("++", true) || match_token("--", true))
		{
			continue;
		}

		if (match_token("(", true))
		{
			prod(&parser::argument_list);
			match_token(")");
			continue;
		}

		break;
	}
}

void parser::argument_list()
{
	curr_node->node_type = "argument_list";

	prod(&parser::assignment_expression);

	while (match_token(",", true))
	{
		prod(&parser::assignment_expression);
	}
}

void parser::primary_expression()
{
	curr_node->node_type = "primary_expression";

	if (match_token_type(token_type::identifier, true)) return;
	if (match_token_type(token_type::string_constant, true)) return;

	if (match_token("(", true))
	{
		prod(&parser::expression);
		match_token(")");
	}

	prod(&parser::constant);
}

void parser::constant()
{
	curr_node->node_type = "constant";

	if (match_token_type(token_type::numeric_constant, true)) return;

	match_token_type(token_type::character_constant);
}