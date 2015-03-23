
struct TreeLoader_t : NZA_t
{
private:

    DISABLE_CLASS_COPY (TreeLoader_t)
    uint16_t buildNumber_;
    uint8_t  personalCode_;
    Node_t<NodeContent_t>* node_;
    File_t file_;

    void ok ();

    public:

    TreeLoader_t (std::string filename, Node_t<NodeContent_t>* node);

    void LoadTree ();
    void LoadNode (Node_t<NodeContent_t>* current);
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
    node_         (node),
    file_         (filename, "r")
{
END (CTOR)




void TreeLoader_t::LoadTree ()
{
    BEGIN

    fscanf (*file_, "%hu %hhu\n", &buildNumber_, &personalCode_);


    uint32_t skip = 0;
    if (fscanf (*file_, "FUNCS %u\n", &skip) != 1)
        _EXC_N (FILE_FUNCS, "Failed to read \"FUNCS\" line")
    for (uint32_t i = 0; i < skip; i++) fscanf (*file_, "%*[^\n]\n");

    if (fscanf (*file_, "VARS %u\n", &skip) != 1)
        _EXC_N (FILE_VARS, "Failed to read \"VARS\" line")
    for (uint32_t i = 0; i < skip; i++) fscanf (*file_, "%*[^\n]\n");

    LoadNode (node_);


    END (LOAD_TREE)
}

void TreeLoader_t::LoadNode (Node_t<NodeContent_t>* current)
{
    BEGIN

    uint16_t cmd = 0;
    double val = 0;
    if (fscanf (*file_, "[ %hu %lg ", &cmd, &val) != 2)
        _EXC_N (READ_NODE, "Failed to read node")

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
