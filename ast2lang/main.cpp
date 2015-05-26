#include "Build.h"


int main (int argc, char** argv)
{
    std::string input, output;
    if (argc != 3)
    {
        std::string cmd (argv[0]);
        size_t n = cmd.find_last_of ('\\');
        if (n != cmd.npos) cmd.erase (cmd.begin(), cmd.begin() + n + 1);
        printf ("%s: Invalid number of command line arguments\nExpected input.file output.file\n", cmd.c_str ());
        return 0;
    }
    else
    {
        input = argv[1];
        output = argv[2];
    }


    EXPN = new exception_data (20, "ExceptionErrors.txt");
    try
    {
        Ast2LangTranslator_t a2l (input, output);
        a2l.Translate ();
        return 0;
    }
    catch (ExceptionHandler& ex)
    {
        printf ("Exception occurred\nCheck \"ExceptionErrors.txt\"");
        ex.WriteLog (EXPN);
    }
    catch (std::exception err)
    {
        printf ("Exception occurred: %s\n", err.what ());
    }
    catch (...)
    {
        printf ("Exception occurred\n");
    }
}
