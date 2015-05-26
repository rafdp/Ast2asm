
struct TreeLoader_t : NZA_t
{
private:

    DISABLE_CLASS_COPY (TreeLoader_t)
    uint16_t buildNumber_;
    uint8_t  personalCode_;
    std::vector<std::string> funcs_;
    std::vector<std::string> vars_;
    Node_t<NodeContent_t>* node_;
    File_t file_;

    void ok ();

    public:

    TreeLoader_t (std::string filename, Node_t<NodeContent_t>* node);
    ~TreeLoader_t () {}

    void LoadTree ();
    void LoadNode (Node_t<NodeContent_t>* current);

    std::string func (uint32_t n)
    {
        BEGIN
        if (n >= funcs_.size ())
            _EXC_N (OUT_OF_RANGE, "Trying to access out of range element (%d of %d)" _ n _ funcs_.size ())
        else return funcs_ [n];
        END (GET_FUNC)
    }

    std::string var (uint32_t n)
    {
        BEGIN
        if (n >= vars_.size ())
            _EXC_N (OUT_OF_RANGE, "Trying to access out of range element (%d of %d)" _ n _ vars_.size ())
        else return vars_ [n];
        END (GET_VAR)
    }
};

void TreeLoader_t::ok ()
{
    DEFAULT_OK_BLOCK
    if (node_ == nullptr)
        _EXC_N (NULL_PTR, "Null node ptr")
}

TreeLoader_t::TreeLoader_t (std::string filename, Node_t<NodeContent_t>* node)
try :
    buildNumber_  (),
    personalCode_ (),
    funcs_        (),
    vars_         (),
    node_         (node),
    file_         (filename, "r")
{
END (CTOR)




void TreeLoader_t::LoadTree ()
{
    BEGIN

    fscanf (*file_, "%hu %hhu\n", &buildNumber_, &personalCode_);


    uint32_t skip = 0;

    char buffer[MAX_PATH] = "";

    if (fscanf (*file_, "FUNCS %u\n", &skip) != 1)
        _EXC_N (FILE_FUNCS, "Failed to read \"FUNCS\" line")
    for (uint32_t i = 0; i < skip; i++)
    {
        fscanf (*file_, "%[^\n]%*c\n", buffer);
        funcs_.push_back (buffer);
    }

    if (fscanf (*file_, "VARS %u\n", &skip) != 1)
        _EXC_N (FILE_VARS, "Failed to read \"VARS\" line")
    for (uint32_t i = 0; i < skip; i++)
    {
        fscanf (*file_, "%[^\n]%*c\n", buffer);
        vars_.push_back (buffer);
    }

    LoadNode (node_);


    END (LOAD_TREE)
}

void TreeLoader_t::LoadNode (Node_t<NodeContent_t>* current)
{
    BEGIN
    static int n = 0;
    n++;

    uint16_t cmd = 0;
    double val = 0;
    if (fscanf (*file_, "[ %hu %lg ", &cmd, &val) != 2)
        _EXC_N (READ_NODE, "Failed to read node %d" _ n)
    current->SetElem (NodeContent_t (cmd, val));
    int32_t next = fgetc (*file_);
    if (next == '[')
    {
        while (next == '[')
        {
            fseek (*file_,-1, SEEK_CUR);
            current->PushChild ();
            LoadNode (current->GetLastChild ());
            next = fgetc (*file_);
            if (next != ' ')
                _EXC_N (EXCPECTED_SPACE, "Excpected \' \' between child nodes")
            next = fgetc (*file_);
        }
    }
    if (next == 'N')
    {
        fscanf (*file_, "ULL ]");
        return;
    }
    else
        _EXC_N (EXCPECTED_SPACE, "Excpected \"NULL\" between child nodes")

    END (LOAD_NODE)
}
