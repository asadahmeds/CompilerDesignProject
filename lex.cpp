#include <iostream>
#include <string>
#include <cctype>
#include <iomanip>

#include "lex.h"

using namespace std;

void lexical_analyzer::add_tokens(string& source_line)
{
    const string three_symbol_tokens = "<<=$>>=$..."; // $ is used as separator
    const string two_symbol_tokens = "!=$%=$*=$^=$==$&&$&=$/=$||$|=$++$+=$<<$<=$--$-=$->$>=$>>";

    const string keywords = ".auto.double.int.struct.break.else.long.switch.case.enum.register."
        "typedef.char.extern.return.union.const.float.short.unsigned.continue."
        "for.signed.void.default.goto.sizeof.volatile.do.if.static.while."; // dot used as separator

    cur_line++;
    cur_pos = 0;

    while (is_valid && cur_pos < source_line.length())
    {
        if (in_multiline_comment)
        {
            auto end_pos = source_line.find("*/", cur_pos);
            if (end_pos == string::npos) return;
            cur_pos = end_pos + 2;
            in_multiline_comment = false;
            continue;
        }

        auto lookahead1 = source_line[cur_pos];

        if (isspace(lookahead1))
        {
            cur_pos++;
            continue;
        }

        auto lookahead2 = cur_pos + 1 < source_line.length() ?
            source_line[cur_pos + 1] : '@'; // @ indicates no 2nd char

        auto lookahead3 = cur_pos + 2 < source_line.length() ?
            source_line[cur_pos + 2] : '@'; // @ indicates no 3rd char

        string lookahead12{ lookahead1, lookahead2 };
        string lookahead123{ lookahead1, lookahead2, lookahead3 };

        if (!is_valid_char(lookahead1)) return;

        if (lookahead12 == "//") return; // ignore rest of the line

        if (lookahead12 == "/*") // start of multiline comment
        {
            in_multiline_comment = true;
            cur_pos += 2;
            continue;
        }

        token* p_token;

        if (lookahead1 == '\'') // char constant
        {
            if (lookahead3 != '\'')
            {
                cout << "Invalid character constant" << lookahead123 << " at line number " << cur_line << endl;

                is_valid = false;
                return;
            }

            p_token = new token(token_type::character_constant, lookahead123);
            cur_pos += 3;
        }
        else if (lookahead1 == '\"') // string
        {
            auto end_pos = source_line.find('\"', cur_pos + 1);

            if (end_pos == string::npos)
            {
                cout << "Unterminated string at line number " << cur_line << endl;
                is_valid = false;
                return;
            }

            p_token = new token(token_type::string_constant, source_line.substr(cur_pos, end_pos - cur_pos + 1));
            cur_pos = end_pos + 1;
        }
        else if (isalpha(lookahead1) || lookahead1 == '_') // identifier
        {
            auto end_pos = cur_pos;
            while (isalnum(source_line[end_pos]) || source_line[end_pos] == '_') end_pos++;

            auto identifier_str = source_line.substr(cur_pos, end_pos - cur_pos);

            auto identifier_type = token_type::identifier;
            if (keywords.find("." + identifier_str + ".") != string::npos) identifier_type = token_type::keyword;

            p_token = new token(identifier_type, identifier_str);
            cur_pos = end_pos;
        }
        else if (isdigit(lookahead1) || lookahead1 == '.' && isdigit(lookahead2)) // numeric constant
        {
            auto end_pos = cur_pos;
            auto decimal_count = 0;

            while (isdigit(source_line[end_pos]) || source_line[end_pos] == '.')
            {
                if (source_line[end_pos] == '.') decimal_count++;
                end_pos++;
            }

            auto const_str = source_line.substr(cur_pos, end_pos - cur_pos);

            if (decimal_count > 1)
            {
                cout << "Invalid numeric constant " << const_str << " at line number " << cur_line << endl;
                is_valid = false;
                return;
            }

            p_token = new token(token_type::numeric_constant, const_str);
            cur_pos = end_pos;
        }
        else if (three_symbol_tokens.find(lookahead123) != string::npos) // symbol token of length 3
        {
            p_token = new token(token_type::other, lookahead123);
            cur_pos += 3;
        }
        else if (two_symbol_tokens.find(lookahead12) != string::npos) // symbol token of length 2
        {
            p_token = new token(token_type::other, lookahead12);
            cur_pos += 2;
        }
        else  // single symbol
        {
            p_token = new token(token_type::other, string{ lookahead1 });
            cur_pos++;
        }

        append_token(p_token);
    }
}

void lexical_analyzer::complete()
{
    if (in_multiline_comment)
    {
        cout << "Unexpected end of file in comment" << endl;
        is_valid = false;
    }
}

bool lexical_analyzer::is_valid_char(char ch)
{
    const char* invalid_token_start_chars = "$`@#\\";

    if (ch < 0 || ch >= 127)
    {
        cout << "Invalid character " << int(ch) << " at line " << cur_line << "; column " << cur_pos << endl;
        is_valid = false;
        return false;
    }

    if (strchr(invalid_token_start_chars, ch))
    {
        cout << "Unexpected character " << ch << " at line " << cur_line << "; column " << cur_pos << endl;
        is_valid = false;
        return false;
    }

    return true;
}

void lexical_analyzer::append_token(token* new_token)
{
    if (first_token == nullptr)
    {
        first_token = p_last = new_token;
    }
    else
    {
        p_last->p_next = new_token;
        p_last = new_token;
    }
}

lexical_analyzer::~lexical_analyzer()
{
    while (first_token != nullptr)
    {
        auto curr = first_token;
        first_token = first_token->p_next;
        delete curr;
    }

    p_last = nullptr;
}

void lexical_analyzer::print_tokens()
{
    auto curr = first_token;

    while (curr != nullptr)
    {
        curr->print();
        curr = curr->p_next;
    }
}

void token::print()
{
    const int width = 17;

    cout << left << setw(width) << token_type_name(type);
    cout << ": " << chars << endl;
}

const char* token::token_type_name(token_type type)
{
    switch (type)
    {
    case token_type::identifier:
        return "identifier";
    case token_type::keyword:
        return "keyword";
    case token_type::numeric_constant:
        return "numeric_constant";
    case token_type::character_constant:
        return "character_constant";
    case token_type::string_constant:
        return "string_constant";
    default:
        return "other";
    }
}