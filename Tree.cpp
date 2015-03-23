

struct NodeContent_t
{
    uint16_t flag;
    double data;

    NodeContent_t (uint8_t flag_, double data_ = 0) :
        flag (flag_),
        data (data_)
    {}

    NodeContent_t () :
        flag (0),
        data (0)
    {}

    void Print (File_t& f)
    {
        /*switch (flag)
        {
        case NODE_NUMBER:
            fprintf (*f, "num");
            break;
        case NODE_VARIABLE:
            fprintf (*f, "var");
            break;

        case NODE_ROOT:
            fprintf (*f, "root");
            return;

        case NODE_MAIN:
            fprintf (*f, "main");
            return;

        case NODE_OPERATOR:
            fprintf (*f, "op");
            break;

        case NODE_LOGIC:
            fprintf (*f, "logic");
            break;

        case NODE_STD_FUNCTION:
            fprintf (*f, "std_func");
            break;

        case NODE_USER_FUNCTION:
            fprintf (*f, "usr_func");
            break;

        case NODE_VAR_INIT:
            fprintf (*f, "var_init");
            return;

        case NODE_SERVICE:
            fprintf (*f, "serv");
            break;
        default:
            fprintf (*f, "unk");
            break;
        }
        fprintf (*f, " %g", data - ((flag == NODE_USER_FUNCTION) ? 1 : 0));*/
        fprintf (*f, "%d %g", flag, data - ((flag == NODE_USER_FUNCTION) ? 1 : 0));
    }
/*
    double GetDouble ()
    {
        switch (flag)
        {
        case FLAG_NUMERIC:
            //printf ("|num %I64u|", data);
            return static_cast<double> (data);
            break;
        case FLAG_NUMERICD:
            //printf ("|num %I64u|", data);
            return *(reinterpret_cast<double*> (&data));
            break;
        default:
            return 0;
            break;
        }
    }

    bool ResolvePriorities (const NodeContent_t& elem)
    {
        if (flag == FLAG_FUNCTION)
        {
            if (flag == FLAG_OPERATOR) return true;
            else return true;
        }
        else
        if ((elem.flag == FLAG_NUMERIC && *(reinterpret_cast<const int64_t*> (&(elem.data))) < 0) ||
            (elem.flag == FLAG_NUMERICD && *(reinterpret_cast<const double*> (&(elem.data))) < 0)) return true;
        else
        if (elem.flag == FLAG_NUMERIC ||
            elem.flag == FLAG_NUMERICD ||
            elem.flag == FLAG_ARGUMENT ||
            elem.flag == FLAG_FUNCTION) return false;
        else
        if (flag == FLAG_OPERATOR)
        {
            if (data == OP_PLUS || data == OP_MINUS) return false;
            if (data == OP_POW) return true;
            if ((data == OP_MUL || data == OP_DIV) && (elem.data == OP_PLUS || elem.data == OP_MINUS)) return true;
        }
        return false;
    }
*/
};


template <class Element_t>
class Node_t : NZA_t
{

    DISABLE_CLASS_COPY (Node_t)

    std::vector<Node_t*> children_;
    Node_t* parent_;
    Element_t elem_;

    void ok ()
    {
        DEFAULT_OK_BLOCK
    }
public:

    Node_t (const Element_t& elem = Element_t ())
    try :
        children_   (),
        parent_     (),
        elem_       (elem)
    {
    END (CTOR)

    Node_t (Node_t* parent,
            const Element_t& elem = ZERO (Element_t))
    try :
        children_   (),
        parent_     (parent),
        elem_       (elem)
    {
    END (CTOR)


    Node_t* PushChild (Element_t elem = Element_t())
    {
        BEGIN

        Node_t<Element_t>* ptr = new Node_t (this, elem);

        children_.push_back (ptr);

        return ptr;

        END (PUSH_CHILD)
    }


    void SetChild (uint32_t n, Node_t* child)
    {
        BEGIN
        if (n > children_.size ()) return;
        if (children_[n] != nullptr)
        {
            delete children_[n];
            children_[n] = nullptr;
        }
        children_[n] = child;
        if (child) children_[n]->parent_ = this;
        END (SET_CHILD)
    }

    void ClearLastChild ()
    {
        BEGIN
        if (children_.size () == 0)
            _EXC_N (NO_LAST_CHILD, "No last child")
        delete *children_.rbegin();
        children_.erase (children_.end() - 1);
        END (CLEAR_LAST_CHILD)
    }


    bool Leaf ()
    {
        BEGIN
        return (children_.empty ());
        END (LEAF)
    }


    Node_t* GetChild (uint32_t n)
    {
        BEGIN
        if (n > children_.size ())
            _EXC_N (OUT_OF_RANGE, "Trying to access out of range element")
        return children_[n];
        END (GET_CHILD)
    }

    uint32_t GetNChildren ()
    {
        BEGIN
        return children_.size ();
        END (GET_N_CHILDREN)
    }



    Node_t* GetLastChild ()
    {
        BEGIN
        if (children_.size () == 0)
            _EXC_N (NO_LAST_CHILD, "No last child")
        return *children_.rbegin ();
        END (GET_LAST_CHILD)
    }

    Node_t& operator [] (uint32_t n)
    {
        BEGIN
        return *GetChild (n);
        END (OP_SQUARE_BRACKETS)
    }

    Node_t* GetParent ()
    {
        BEGIN
        return parent_;
        END (GET_PARENT)
    }



    bool Root ()
    {
        BEGIN
        return parent_ == nullptr;
        END (ROOT)
    }


    ~Node_t ()
    {
        BEGIN
        Clear ();
        if (parent_)
        {
            auto found = std::find (parent_->children_.begin(),
                                    parent_->children_.end (),
                                    this);
            if (found != parent_->children_.end ())
                *found = nullptr;
        }
        return;
        END (DTOR)
    }


    void SetElem (const Element_t& elem)
    {
        BEGIN
        elem_ = elem;
        END (SET_ELEM)
    }

    Node_t* InsertAndSlide (uint32_t pos, const Element_t& elem)
    {
        BEGIN
        if (pos > children_.size ())
            _EXC_N (OUT_OF_RANGE, "Trying to access out of range element")

        Node_t<Element_t>* bak = children_[pos];
        children_[pos] = nullptr;
        SetChild (pos, new Node_t (this, elem));
        children_[pos]->PushChild ();
        children_[pos]->SetChild (0, bak);
        return children_[pos];
        END (INSERT_AND_SLIDE)
    }


    Element_t& GetElem ()
    {
        BEGIN
        return elem_;
        END (GET_ELEM)
    }

    void Clear ()
    {
        BEGIN
        for (auto& i : children_)
        {
            if (i)
            {
                delete i;
                i = nullptr;
            }
        }
        children_.clear ();
        END (CLEAR)
    }


    void DumpPrefix (File_t& f, bool first = true)
    {
        BEGIN
        fprintf (*f, "[ ");
        elem_.Print (f);
        for (auto& i : children_) {fprintf (*f, " "); i->DumpPrefix (f, false); }
        fprintf (*f, " NULL ]");
        //if (!first) fprintf (*f, " ");
        END (DUMP)
    }
/*
    void DumpInfix ()
    {
        if (leftChild_)
        {
            if (elem_.ResolvePriorities (leftChild_->GetElem()))
            {
                printf ("(");
                leftChild_->DumpInfix ();
                printf (")");
            }
            else leftChild_->DumpInfix ();
        }
        elem_.Print();

        if (rightChild_)
        {
            if (elem_.ResolvePriorities (rightChild_->GetElem()))
            {
                printf ("(");
                rightChild_->DumpInfix ();
                printf (")");
            }
            else rightChild_->DumpInfix ();
        }
    }

    void DumpPostfix ()
    {
        printf ("(");
        if (leftChild_)  leftChild_->DumpPostfix ();
        if (rightChild_) {rightChild_->DumpPostfix (); printf (" ");}
        if (leftChild_ && !rightChild_) printf (" ");
        elem_.Print();
        printf (")");
    }
*/

    Node_t* UnlinkChild (uint32_t n)
    BEGIN
        Node_t* ret = children_[n];
        children_[n] = nullptr;
        return ret;
    END (UNLINK_CHILD)


};

void CopyTree (Node_t<NodeContent_t>* source, Node_t<NodeContent_t>* destination);
void CopyTree (Node_t<NodeContent_t>* source, Node_t<NodeContent_t>* destination)
{
    destination->SetElem (source->GetElem ());
    uint32_t size = source->GetNChildren ();
    for (uint32_t i = 0; i < size; i++)
    {
        CopyTree (source->GetChild (i), destination->PushChild ());
    }
}
