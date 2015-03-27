

class Ast2AsmTranslator_t : NZA_t
{
    DISABLE_CLASS_COPY (Ast2AsmTranslator_t)

    File_t target_;

    Node_t<NodeContent_t> root_;
    TreeLoader_t loader_;
    uint32_t nLabels_;
    uint32_t nEndLabels_;
    bool needEnd_;

    void ok ();
    uint32_t label () {nLabels_++; return nLabels_-1;}
    uint32_t labels () {return nLabels_;}
    uint32_t endlabel () {nEndLabels_++; return nEndLabels_-1;}
    uint32_t EndLabels () {return nEndLabels_;}

    void AddEndLabel ()
    {
        if (needEnd_) return;
        needEnd_ = true;
        fprintf (*target_, "plabel EndLabel%d\n", endlabel ());
    }

    void ResolveEndLabel ()
    {
        if (!needEnd_) return;
        needEnd_ = false;
        fprintf (*target_, "EndLabel%d:\n", EndLabels () - 1);
    }
public:

    Ast2AsmTranslator_t (std::string source, std::string target);
    ~Ast2AsmTranslator_t () {}

    void Translate ();
    void TranslateTree (Node_t<NodeContent_t>* current, bool root);
    void TranslateArithmeticTree (Node_t<NodeContent_t>* current, bool inv = false);
};

void Ast2AsmTranslator_t::ok ()
{
    DEFAULT_OK_BLOCK
}

Ast2AsmTranslator_t::Ast2AsmTranslator_t (std::string source, std::string target)
try :
    target_     (target, "w"),
    root_       (),
    loader_     (source, &root_),
    nLabels_    (),
    nEndLabels_ (),
    needEnd_    (false)
{
    loader_.LoadTree ();
END (CTOR)

void Ast2AsmTranslator_t::Translate ()
{
    BEGIN
    fprintf (*target_, "import \"msvcrt.dll\", \"printf _printf\"\n");
    fprintf (*target_, "import \"msvcrt.dll\", \"scanf _scanf\"\n");
    fprintf (*target_, "var pointer, type_ptr\n\n");
    uint32_t size = root_[0].GetNChildren ();
    for (uint32_t i = 0; i < size; i++)
    {
        fprintf (*target_, "var variable%d, type_qword\n", i);
        fprintf (*target_, "mov variable%d, %lld\n\n", i, root_[0][i][0].GetElem ().data);
    }

    size = root_.GetNChildren () - 1;

    for (uint32_t i = 1; i < size; i++)
    {
        fprintf (*target_, "func function%d\n", i - 1);
        TranslateTree (root_.GetChild (i), true);

        fprintf (*target_, "ret\n\n");
    }
    fprintf (*target_, "\n\n");
    TranslateTree (root_.GetLastChild (), true);
    END (TRANSLATE)
}

void Ast2AsmTranslator_t::TranslateTree (Node_t<NodeContent_t>* current, bool root)
{   //!ResolveEndLabel ();
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
            fprintf (*target_, "lea pointer, variable%g\n",
                     round (current->GetChild (0)->GetElem ().data));

            fprintf (*target_, "init_stack_dump_point\n");
            fprintf (*target_, "push pointer\n");
            fprintf (*target_, "push \"%%lg\"\n");
            fprintf (*target_, "jit_call_void _scanf\n");
            return;
        }
        else
        CHECK (OUTPUT)
        {
            TranslateArithmeticTree (current->GetChild (0));
            fprintf (*target_, "printd PRINT_NUMBER\n");
            fprintf (*target_, "pop\n");
            return;
        }
        else
        CHECK (GETCH)
        {
            fprintf (*target_, "getch\n");
            return;
        }
        else
        CHECK (CALL_USER)
        {
            fprintf (*target_, "call function%g\n",
                     round (current->GetChild (0)->GetElem ().data));
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
    }
    else
    if (current->GetElem ().flag == NODE_LOGIC)
    {
        #define CHECK(dt) if (fabs (current->GetElem ().data - LOGIC_##dt) <= 0.01)
        CHECK (IF)
        {
            bool needElse = (current->GetNChildren () == 3);
            TranslateArithmeticTree (current->GetChild (0)->GetChild (0));
            TranslateTree (current->GetChild (1), true);
            uint32_t label_ = 0;
            if (needElse)
            {
                label_ = label ();
                fprintf (*target_, "plabel Label%d\n", label_);
                fprintf (*target_, "jmp Label%d\n", label_);
            }
            ResolveEndLabel ();
            if (needElse)
            {
                TranslateTree (current->GetChild (2), true);
                fprintf (*target_, "Label%d:\n", label_);
            }
        }
        else
        CHECK (WHILE)
        {
            uint32_t label_ = label ();
            fprintf (*target_, "Label%d:\n", label_);
            TranslateArithmeticTree (current->GetChild (0)->GetChild (0), false);
            TranslateTree (current->GetChild (1), true);
            fprintf (*target_, "jmp Label%d\n", label_);
            ResolveEndLabel ();
        }
        else
            _EXC_N (UNKNOWN_LOGIC, "Found unknown logic cmd (%g)" _ current->GetElem ().data)
        #undef CHECK
    }
    END (TRANSLATE_TREE)
}

void Ast2AsmTranslator_t::TranslateArithmeticTree (Node_t<NodeContent_t>* current, bool inv)
{
    //printf ("_%d %g\n", current->GetElem ().flag, current->GetElem ().data);
    BEGIN
    if (current->GetElem ().flag == NODE_OPERATOR)
    {
        #define CHECK(dt) if (fabs (current->GetElem ().data - OP_##dt) <= 0.01)
        CHECK (EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop variable%g\n",
                     round (current->GetChild (0)->GetElem ().data));
        }
        else
        CHECK (PLUS)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "addsd\n");
        }
        else
        CHECK (MINUS)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "subsd\n");
        }
        else
        CHECK (MULTIPLY)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "mulsd\n");
        }
        else
        CHECK (DIVIDE)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "divsd\n");
        }
        else
        CHECK (POWER)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pwrsd\n");
        }
        else
        CHECK (BOOL_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop bqx\n");
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "cmprd aqx, bqx\n");
            AddEndLabel ();
            fprintf (*target_, "%s EndLabel%d\n",
                     inv ? "je" : "jne",
                     EndLabels () - 1);
        }
        else
        CHECK (BOOL_NOT_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop bqx\n");
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "cmprd aqx, bqx\n");
            AddEndLabel ();
            fprintf (*target_, "%s EndLabel%d\n",
                     inv ? "jne" : "je",
                     EndLabels () - 1);
        }
        else
        CHECK (BOOL_LOWER)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop bqx\n");
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "cmprd aqx, bqx\n");
            AddEndLabel ();
            fprintf (*target_, "%s EndLabel%d\n",
                     inv ? "jb" : "jae",
                     EndLabels () - 1);
        }
        else
        CHECK (BOOL_GREATER)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop bqx\n");
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "cmprd aqx, bqx\n");
            AddEndLabel ();
            fprintf (*target_, "%s EndLabel%d\n",
                     inv ? "ja" : "jbe",
                     EndLabels () - 1);
        }
        else
        CHECK (BOOL_NOT_LOWER)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop bqx\n");
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "cmprd aqx, bqx\n");
            AddEndLabel ();
            fprintf (*target_, "%s EndLabel%d\n",
                     inv ? "jae" : "jb",
                     EndLabels () - 1);
        }
        else
        CHECK (BOOL_NOT_GREATER)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop bqx\n");
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "cmprd aqx, bqx\n");
            AddEndLabel ();
            fprintf (*target_, "%s EndLabel%d\n",
                     inv ? "jbe" : "ja",
                     EndLabels () - 1);
        }
        else
        CHECK (BOOL_NOT)
        {
            TranslateArithmeticTree (current->GetChild (0), !inv);
        }
        else
        CHECK (BOOL_OR)
        {
            uint32_t size = current->GetNChildren ();
            for (uint32_t i = 0; i < size; i++)
                TranslateArithmeticTree (current->GetChild (i), !inv);
            fprintf (*target_, "plabel EndLabel%d\n", EndLabels ());
            fprintf (*target_, "jmp EndLabel%d\n", EndLabels ());
            ResolveEndLabel ();
            needEnd_ = true;
        }
        else
        CHECK (BOOL_AND)
        {
            uint32_t size = current->GetNChildren ();
            for (uint32_t i = 0; i < size; i++)
                TranslateArithmeticTree (current->GetChild (i), inv);
        }
        else
        CHECK (PLUS_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "addd variable%g, aqx\n",
                     round (current->GetChild (0)->GetElem ().data));
        }
        else
        CHECK (MINUS_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "subd variable%g, aqx\n",
                     round (current->GetChild (0)->GetElem ().data));
        }
        else
        CHECK (MULTIPLY_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "muld variable%g, aqx\n",
                     round (current->GetChild (0)->GetElem ().data));
        }
        else
        CHECK (DIVIDE_EQUAL)
        {
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "pop aqx\n");
            fprintf (*target_, "divd variable%g, aqx\n",
                     round (current->GetChild (0)->GetElem ().data));
        }
        else
            _EXC_N (UNKNOWN_OP, "Found unknown op (%g)" _ current->GetElem ().data)
        #undef CHECK

    }
    else
    if (current->GetElem ().flag == NODE_NUMBER)
    {
        fprintf (*target_, "push %lld\n", current->GetElem ().data);
    }
    else
    if (current->GetElem ().flag == NODE_VARIABLE)
    {
        fprintf (*target_, "push variable%g\n", round (current->GetElem ().data));
    }
    else
    if (current->GetElem ().flag == NODE_STD_FUNCTION)
    {
        #define CHECK(dt) if (fabs (current->GetElem ().data - STD_FUNC_##dt) <= 0.01)
        CHECK (SIN)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            fprintf (*target_, "pop aqx\nsin aqx\npush aqx\n");
        }
        else
        CHECK (COS)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            fprintf (*target_, "pop aqx\ncos aqx\npush aqx\n");
        }
        else
        CHECK (SQRT)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            fprintf (*target_, "pop aqx\nsqrtd aqx\npush aqx\n");
        }
        else
        CHECK (MAX)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "maxd\n");
        }
        else
        CHECK (MIN)
        {
            TranslateArithmeticTree (current->GetChild (0), inv);
            TranslateArithmeticTree (current->GetChild (1), inv);
            fprintf (*target_, "mind\n");
        }
        else
            _EXC_N (UNKNOWN_CMD, "Found unknown cmd (%g)" _ current->GetElem ().data)
        #undef CHECK

    }
    END (TRANSLATE_ARITHMETIC_TREE)
}


