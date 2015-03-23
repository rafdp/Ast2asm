#include "Build.h"


int main()
{
    EXPN = new exception_data (20, "ExceptionErrors.txt");
    try
    {
        Node_t<NodeContent_t> root;
        TreeLoader_t trl ("ast.txt"s, &root);
        trl.LoadTree ();
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
