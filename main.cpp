#include <iostream>
#include <fstream>
#include <string>

#include "lex.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[])
{
    ifstream source_file(argv[1]);
    string source_line;
    lexical_analyzer lex;

    if (!source_file.good())
    {
        cout << "Invalid input file" << endl;
        return -1;
    }
    
    while (lex.is_valid && getline(source_file, source_line))
    {
        lex.add_tokens(source_line);
    }

    lex.complete();

    if (!lex.is_valid) return 0;

    cout << endl << "Tokens:" << endl << endl;

    lex.print_tokens();

    parser par;

    auto tree = par.parse(lex.first_token);

    if (tree == nullptr)
    {
        cout << endl << "Parse Failed" << endl;
        return 0;
    }

    cout << endl << "Parse Tree:" << endl << endl;

    tree->print();

    cout << endl;
}