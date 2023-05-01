#include <iostream>
#include <iomanip>

#include "parser.h"

bool parser::match_token_type(token_type type, bool optional)
{
	if (curr_token == nullptr || curr_token->type != type)
	{
		if (optional) return false;
		throw "NoMatch";
	}

	curr_node->add_child(curr_token);

	curr_token = curr_token->p_next;

	return true;
}

bool parser::match_keyword(const char *keyword, bool optional)
{
	if (curr_token == nullptr || curr_token->type != token_type::keyword || curr_token->chars != keyword)
	{
		if (optional) return false;
		throw "NoMatch";
	}

	curr_node->add_child(curr_token);

	curr_token = curr_token->p_next;

	return true;
}

bool parser::match_token(const char* token_chars, bool optional)
{
	if (curr_token == nullptr || curr_token->type != token_type::other || curr_token->chars != token_chars)
	{
		if (optional) return false;
		throw "NoMatch";
	}

	curr_node->add_child(curr_token);

	curr_token = curr_token->p_next;

	return true;
}

bool parser::match_end(bool optional)
{
	if (curr_token != nullptr)
	{
		if (optional) return false;
		throw "NoMatch";
	}

	return true;
}

node* parser::parse(token* first_token)
{
	curr_token = first_token;

	auto root_node = new node(curr_token);
	curr_node = root_node;

	auto success = true;

	try
	{
		root();
	}
	catch (...)
	{
		success = false;
	}

	if (curr_token != nullptr) success = false;

	curr_token = nullptr;
	curr_node = nullptr;

	if (!success)
	{
		delete root_node;
		root_node = nullptr;
	}

	return root_node;
}

node* node::add_child(token* node_token)
{
	node* child = new node(node_token);
	children.push_back(child);
	return child;
}

node::~node()
{
	auto size = children.size();

	while (size > 0)
	{
		auto child = children[--size];
		children.pop_back();
		delete child;
	}
}

bool parser::prod(void(parser::*prod_func)(), bool optional)
{
	auto prev_token = curr_token;
	auto prev_node = curr_node;
	auto prev_child_count = prev_node->children.size();

	curr_node = curr_node->add_child(curr_token);

	try
	{
		(this->*prod_func)();
	}
	catch (...)
	{
		auto curr_count = prev_node->children.size();

		while (curr_count > prev_child_count)
		{
			auto child = prev_node->children[curr_count - 1];

			prev_node->children.pop_back();
			curr_count--;

			delete child;
		}

		curr_node = prev_node;
		curr_token = prev_token;

		if (optional) return false;

		throw;
	}

	curr_node = prev_node;

	return true;
}

void node::print(int indent)
{
	const int indentSize = 4;

	if (children.size() == 1 && children[0]->node_type != nullptr)
	{
		children[0]->print(indent);
		return;
	}

	int width = indent * indentSize;

	cout << setw(width) << "";

	if (node_type != nullptr)
	{
		cout << node_type;
	}
	else
	{
		if (node_token->type != token_type::other)
		{
			cout << token::token_type_name(node_token->type) << ":";
		}

		cout << node_token->chars;
	}

	cout << endl;

	indent++;

	for (auto& child : children)
	{
		child->print(indent);
	}
}