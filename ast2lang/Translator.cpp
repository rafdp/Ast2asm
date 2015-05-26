

class Ast2LangTranslator_t : NZA_t
{
    DISABLE_CLASS_COPY (Ast2LangTranslator_t)

    File_t target_;

    Node_t<NodeContent_t> root_;
    TreeLoader_t loader_;
    uint32_t loop_;
    uint32_t tab_;

    void ok ();
public:

    Ast2LangTranslator_t (std::string source, std::string target);
    ~Ast2LangTranslator_t () {}

    void Translate ();
    void TranslateTree (Node_t<NodeContent_t>* current, bool root);
    void TranslateArithmeticTree (Node_t<NodeContent_t>* current, bool shift = false, bool inv = false);
    void PrintFormatted (bool shift, std::string str)
    {
        if (shift)
            fprintf (*target_, "%*c%s", tab_ + 1, ' ', str.c_str ());
        else
            fprintf (*target_, "%s", str.c_str ());
    }
};

void Ast2LangTranslator_t::ok ()
{
    DEFAULT_OK_BLOCK
}

Ast2LangTranslator_t::Ast2LangTranslator_t (std::string source, std::string target)
try :
    target_     (target, "w"),
    root_       (),
    loader_     (source, &root_),
    loop_       (),
    tab_        ()
{
    loader_.LoadTree ();
END (CTOR)

void Ast2LangTranslator_t::Translate ()
{
    BEGIN
    fprintf (*target_, "HAI\n\n");

    uint32_t size = root_[0].GetNChildren ();
    for (uint32_t i = 0; i < size; i++)
    {
        fprintf (*target_, "I HAS A %s ITZ %.6g\n",
                 loader_.var (root_[0][i].GetElem().data).c_str (),
                 root_[0][i][0].GetElem().data);
    }
    fprintf (*target_, "\n");

    size = root_.GetNChildren () - 1;

    for (uint32_t i = 1; i < size; i++)
    {
        fprintf (*target_, "HOW IZ I %s\n\n",
                 loader_.func (root_[i].GetElem().data).c_str ());
        tab_ += 2;
        TranslateTree (root_.GetChild (i), true);
        tab_ -= 2;
        fprintf (*target_, "\nIF YOU SAY SO\n\n");
    }

    fprintf (*target_, "OPENNG\n\n");
    tab_ += 2;

    TranslateTree (root_.GetLastChild (), true);
    tab_ -= 2;
    fprintf (*target_, "\nKTHXBYE\n");
    END (TRANSLATE)
}

void Ast2LangTranslator_t::TranslateTree (Node_t<NodeContent_t>* current, bool root)
{
    #define $(str) *target_, "%*c" str, tab_ + 1, ' '
    //printf ("%d %g\n", current->GetElem ().flag, current->GetElem ().data);
    BEGIN
    if (root)
    {
        uint32_t size = current->GetNChildren ();
        for (uint32_t i = 0; i < size; i++)
            TranslateTree (current->GetChild (i), false);
        return;
    }
    if (current->GetElem ().flag == NODE_STD_FUNCTION)
    {
        #define CHECK(dt) if (fabs (current->GetElem ().data - STD_FUNC_##dt) <= 0.01)
        CHECK (INPUT)
        {
            fprintf ($ ("GIMMEH "));
            TranslateArithmeticTree (current->GetChild (0));
            fprintf (*target_, "\n");
            return;
        }
        else
        CHECK (OUTPUT)
        {
            fprintf ($ ("VISIBLE "));
            TranslateArithmeticTree (current->GetChild (0));
            fprintf (*target_, "\n");
            return;
        }
        else
        CHECK (GETCH)
        {
            fprintf ($ ("GETCH\n"));
            return;
        }
        else
        CHECK (CALL_USER)
        {
            fprintf ($ ("I IZ %s\n"), loader_.func (current->GetChild (0)->GetElem ().data).c_str ());
            return;
        }
        else
            _EXC_N (UNKNOWN_CMD, "Found unknown cmd (%g)" _ current->GetElem ().data)
        #undef CHECK
    }
    else
    if (current->GetElem ().flag == NODE_OPERATOR)
    {
        TranslateArithmeticTree (current);
        fprintf (*target_, "\n");
    }
    else
    if (current->GetElem ().flag == NODE_LOGIC)
    {
        #define CHECK(dt) if (fabs (current->GetElem ().data - LOGIC_##dt) <= 0.01)
        CHECK (IF)
        {
            TranslateArithmeticTree (current->GetChild (0)->GetChild (0), true);
            fprintf (*target_, "\n");
            fprintf ($ ("O RLY?\n"));
            tab_ += 2;
            fprintf ($ ("YA RLY\n\n"));
            tab_ += 2;
            TranslateTree (current->GetChild (1), true);
            if (current->GetNChildren () == 3)
            {
                fprintf (*target_, "\n");
                tab_ -= 2;
                fprintf ($ ("NO WAI\n\n"));
                tab_ += 2;
                TranslateTree (current->GetChild (2), true);
                fprintf (*target_, "\n");
            }
            tab_ -= 4;
            fprintf ($ ("OIC\n\n"));
        }
        else
        CHECK (WHILE)
        {
            loop_++;
            uint32_t loopBak = loop_ - 1;
            fprintf ($ ("IM IN YR loop%d "), loopBak);
            if (current->GetChild (0)->GetChild (0)->GetElem () == NodeContent_t (NODE_OPERATOR, OP_BOOL_NOT))
            {
                fprintf (*target_, "TIL ");
                TranslateArithmeticTree (current->GetChild (0)->GetChild (0)->GetChild (0));
            }
            else
            {
                fprintf (*target_, "WILE ");
                TranslateArithmeticTree (current->GetChild (0)->GetChild (0));
            }
            tab_ += 4;
            fprintf (*target_, "\n\n");
            TranslateTree (current->GetChild (1), true);
            fprintf (*target_, "\n");
            tab_ -= 4;
            fprintf ($ ("IM OUTTA YR loop%d\n\n"), loopBak);
        }
        else
            _EXC_N (UNKNOWN_LOGIC, "Found unknown logic cmd (%g)" _ current->GetElem ().data)
        #undef CHECK
    }
    END (TRANSLATE_TREE)
    #undef $
}

void Ast2LangTranslator_t::TranslateArithmeticTree (Node_t<NodeContent_t>* current, bool shift, bool inv)
{
    //printf ("_%d %g\n", current->GetElem ().flag, current->GetElem ().data);
    #define $(str) *target_, "%*c" str, tab_ + 1
    BEGIN
    if (current->GetElem ().flag == NODE_OPERATOR)
    {
        #define CHECK(dt) if (fabs (current->GetElem ().data - OP_##dt) <= 0.01)
        CHECK (EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (0), true, inv);
            fprintf (*target_, " R ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (PLUS)
        {
            PrintFormatted (shift, "SUM OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (MINUS)
        {
            PrintFormatted (shift, "DIFF OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (MULTIPLY)
        {
            PrintFormatted (shift, "PRODUKT OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (DIVIDE)
        {
            PrintFormatted (shift, "QUOSHUNT OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (POWER)
        {
            PrintFormatted (shift, "PWR OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_EQUAL)
        {
            PrintFormatted (shift, "BOTH SAEM ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_NOT_EQUAL)
        {
            PrintFormatted (shift, "DIFFRINT ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_LOWER)
        {
            PrintFormatted (shift, "BOTH OF DIFFRINT ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);

            fprintf (*target_, " AN BOTH SAEM ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN SMALLR OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_GREATER)
        {
            PrintFormatted (shift, "BOTH OF DIFFRINT ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);

            fprintf (*target_, " AN BOTH SAEM ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN BIGGR OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_NOT_LOWER)
        {
            PrintFormatted (shift, " AN BOTH SAEM ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN BIGGR OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_NOT_GREATER)
        {
            PrintFormatted (shift, " AN BOTH SAEM ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN SMALLR OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_NOT)
        {
            PrintFormatted (shift, "NOT ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
        }
        else
        CHECK (BOOL_OR)
        {
            PrintFormatted (shift, "ANY OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (BOOL_AND)
        {
            PrintFormatted (shift, "BOTH OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (PLUS_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (0), true, inv);
            fprintf (*target_, " R SUM OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (MINUS_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (0), true, inv);
            fprintf (*target_, " R DIFF OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (MULTIPLY_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (0), true, inv);
            fprintf (*target_, " R PRODUKT OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (DIVIDE_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (0), true, inv);
            fprintf (*target_, " R QUOSHUNT OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
            _EXC_N (UNKNOWN_OP, "Found unknown op (%g)" _ current->GetElem ().data)
        #undef CHECK

    }
    else
    if (current->GetElem ().flag == NODE_NUMBER)
    {
        fprintf (*target_, "%.6g", current->GetElem ().data);
    }
    else
    if (current->GetElem ().flag == NODE_VARIABLE)
    {
        PrintFormatted (shift, loader_.var (round (current->GetElem ().data)));
    }
    else
    if (current->GetElem ().flag == NODE_STD_FUNCTION)
    {
        #define CHECK(dt) if (fabs (current->GetElem ().data - STD_FUNC_##dt) <= 0.01)
        CHECK (SIN)
        {
            PrintFormatted (shift, "SIN OF ");
            TranslateArithmeticTree (current->GetChild (0), inv);
        }
        else
        CHECK (COS)
        {
            PrintFormatted (shift, "COS OF ");
            TranslateArithmeticTree (current->GetChild (0), inv);
        }
        else
        CHECK (SQRT)
        {
            PrintFormatted (shift, "SQRT OF ");
            TranslateArithmeticTree (current->GetChild (0), inv);
        }
        else
        CHECK (DIFFERENTIATE)
        {
            PrintFormatted (shift, "DER OF ");
            TranslateArithmeticTree (current->GetChild (0), inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (MAX)
        {
            PrintFormatted (shift, "BIGGR OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
        CHECK (MIN)
        {
            PrintFormatted (shift, "SMALLR OF ");
            TranslateArithmeticTree (current->GetChild (0), false, inv);
            fprintf (*target_, " AN ");
            TranslateArithmeticTree (current->GetChild (1), false, inv);
        }
        else
            _EXC_N (UNKNOWN_CMD, "Found unknown cmd (%g)" _ current->GetElem ().data)
        #undef CHECK

    }
    END (TRANSLATE_ARITHMETIC_TREE)

    #undef $
}


