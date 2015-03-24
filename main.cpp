#include "Build.h"


int main()
{
    EXPN = new exception_data (20, "ExceptionErrors.txt");
    try
    {
        Ast2AsmTranslator_t a2a ("ast.txt"s, "asm.txt"s);
        a2a.Translate ();
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
