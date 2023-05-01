#pragma once

#include <string>

using namespace std;

enum class token_type
{
    identifier,
    keyword,
    numeric_constant,
    character_constant,
    string_constant,
    other // operators, delimiters, etc.
};

struct token
{
    token_type type;
    string chars;  // characters forming the token
    token* p_next = nullptr;

    token(token_type type, string chars) : type(type), chars(chars) { }
    void print();

    static const char* token_type_name(token_type type);
};

class lexical_analyzer
{
    token* p_last = nullptr;
    bool in_multiline_comment = false;
    int cur_line = 0;
    size_t cur_pos = -1;

    bool is_valid_char(char ch);
    void append_token(token* new_token);

public:
    bool is_valid = true;
    token* first_token = nullptr;

    void add_tokens(string& source_line);
    void complete();

    void print_tokens();
    ~lexical_analyzer();
};