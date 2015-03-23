
std::string& CreateStringOnFail (const char* text, ...)
{
    std::string* strPtr = new std::string;
    if ( !text )
        return *strPtr;

    char str [512]  = "";

    va_list arguments;
    va_start (arguments, text);

    vsnprintf (str, 510, text, arguments);

    va_end (arguments);

    *strPtr += str;

    return *strPtr;
}

#define _EXC_N(code, msg) { NAT_EXCEPTION (EXPN, CreateStringOnFail (msg).c_str (), ERROR_##code) }
#define _ ,

#define BEGIN \
try { ok ();

#define END(code)\
ok (); } \
DETAILED_CONSECUTIVE_CATCH ("Error occurred", ERROR_##code, EXPN) \
DETAILED_CATCH             ("Error occurred", ERROR_BAD_ALLOC, std::bad_alloc, EXPN) \
DETAILED_UNKNOWN_CATCH     ("Error occurred", ERROR_UNKNOWN, EXPN)

#define DEFAULT_OK_BLOCK \
NZA_t::ok (); \
if (this == nullptr) \
    _EXC_N (NULL_THIS, "Null this")\

class NZA_t // no zombies allowed
{
protected:
    int not_yet_destroyed;

public:
    NZA_t () :
        not_yet_destroyed (1)
    {}

    virtual ~NZA_t ()
    {
        not_yet_destroyed = 0;
    }

    virtual void ok ()
    {
        if (this == nullptr)
            _EXC_N (NULL_THIS, "Null this")
        if (not_yet_destroyed == 0)
            _EXC_N (DESTROYED, "Trying to access destroyed object")
    }

};


class File_t : NZA_t
{
    DISABLE_CLASS_COPY (File_t)
    std::string filename_;
    FILE* file_;
    std::string mode_;

    void ok ()
    {
        DEFAULT_OK_BLOCK
    }

public:
    File_t (std::string filename, const char* mode)
    try :
        filename_ (filename),
        file_     (fopen (filename_.c_str (), mode)),
        mode_     (mode)
    {
        if (!file_)
            _EXC_N (OPEN_FILE, "Failed to open file \"%s\"" _ filename_.c_str())

        if (!CheckMode ())
            _EXC_N (FILE_MODE, "Invalid file open mode \"%s\"" _ mode_.c_str())
//  }
    END (CTOR)

    ~File_t ()
    {
        fclose (file_);
        file_ = nullptr;
        filename_.clear ();
        mode_.clear ();
    }

    bool CheckMode ()
    {
        if (mode_.size() > 2 || mode_.size() < 1)
            return false;
        if (mode_[0] != 'a' && mode_[0] != 'w' && mode_[0] != 'r')
            return false;
        if (mode_.size() == 2 && mode_[1] != '+' && mode_[1] != 'b')
            return false;
        return true;
    }

    std::string ReadLine ()
    {
        BEGIN
        char str [DEFAULT_STR_BUFFER_SIZE] = "";
        fgets (str, DEFAULT_STR_BUFFER_SIZE - 1, file_);
        return str;
        END (READ_LINE)
    }

    bool End ()
    {
        BEGIN
        return feof (file_);
        END (FILE_END)
    }

    FILE* operator * ()
    {
        return file_;
    }
};

struct ErrorInfo_t : NZA_t
{
    std::string source_;
    std::string message_;
    uint32_t line_;
    uint32_t pos_;
    uint8_t  mode_;

    void ok ()
    {
        DEFAULT_OK_BLOCK
    }

    ErrorInfo_t (std::string src,
                 std::string msg,
                 uint32_t line,
                 uint32_t pos,
                 uint8_t mode)
    try :
        source_  (src),
        message_ (msg),
        line_    (line),
        pos_     (pos),
        mode_    (mode)
    {
        if (mode_ != EM_WARNING && mode_ != EM_ERROR)
            _EXC_N (INVALID_ERROR_MODE, "Invalid error mode %d" _ mode_)
//  }
    END (CTOR)

    ~ErrorInfo_t () {}

    void WriteError (File_t& file)
    {
        BEGIN
        if (mode_ == EM_WARNING) fprintf (*file, "Warning: ");
        if (mode_ == EM_ERROR) fprintf (*file, "Error: ");
        fprintf (*file, "%s on line %d\n", message_.c_str (), line_);
        fprintf (*file, "%*c\n", pos_ + 1, 'v');
        fprintf (*file, "%s\n--------------------------------------------------\n", source_.c_str ());
        END (WRITE_ERROR)
    }
};

struct CumulativeErrors_t : NZA_t
{
    std::vector<ErrorInfo_t> errors;
    bool ignorable;

    void ok ()
    {
        DEFAULT_OK_BLOCK
    }

    CumulativeErrors_t ()
    try :
        errors    (),
        ignorable (true)
    {
//  }
    END (CTOR)

    void AddError (std::string src,
                   std::string msg,
                   uint32_t line,
                   uint32_t pos,
                   uint8_t mode)
    {
        BEGIN
        printf ("Error\n");
        errors.push_back (ErrorInfo_t (src, msg, line, pos, mode));
        if (mode == EM_ERROR)
            ignorable = false;
        END (ADD_ERROR)
    }

    bool Empty ()
    {
        BEGIN
        return errors.empty ();
        END (EMPTY_STL)
    }

    void DumpErrors (std::string filename, uint32_t parse_level, std::string sourceFilename)
    {
        BEGIN
        File_t file (filename, "w");

        time_t t = time (nullptr);   // get time now
        struct tm* now = localtime( & t );
        fprintf (*file, "Build: %d.%d.%d %d:%d:%d\n",
                        now->tm_mday, now->tm_mon + 1, now->tm_year - 100,
                        now->tm_hour, now->tm_min,  now->tm_sec);

        fprintf (*file, "Parsing file \"%s\"\nLevel %d\n--------------------------------------------------\n", sourceFilename.c_str (), parse_level);

        for (auto& err : errors)
            err.WriteError (file);

        END (DUMP_ERRORS)
    }

    void Clear ()
    {
        BEGIN
        errors.clear ();
        END (CLEAR)
    }

    bool Ignore ()
    {
        return ignorable;
    }

    uint32_t Size ()
    {
        return errors.size ();
    }
};

