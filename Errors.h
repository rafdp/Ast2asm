#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

#define E(num, name) ERROR_##name = num

enum ERRORS
{
    E (357, BAD_ALLOC),
    E (358, UNKNOWN),
    E (359, NULL_THIS),
    E (360, NULL_PTR),
    E (361, OPEN_FILE),
    E (362, DESTROYED),
    E (363, REGISTER_STRING),
    E (364, DELETE_STRING),
    E (365, FILE_MODE),
    E (366, READ_LINE),
    E (367, CTOR),
    E (368, DTOR),
    E (369, NULL_CODE_PTR),
    E (370, LOAD_FILE),
    E (371, INVALID_ERROR_MODE),
    E (372, ADD_ERROR),
    E (373, EMPTY_STL),
    E (374, FILE_END),
    E (375, WRITE_ERROR),
    E (376, DUMP_ERRORS),
    E (377, COMPILE),
    E (378, NULL_CUMLATIVE_ERRORS_PTR),
    E (379, CLEAR),
    E (380, PROCESS),
    E (381, PARSE_LINE),
    E (382, GET_STRING),
    E (383, PARSE_TOKENS),
    E (384, PARSE_CONSTRUCTS),
    E (385, UNTYPED_ON_CONSTRUCTS),
    E (386, PUSH_CHILD),
    E (387, SET_CHILD),
    E (388, GET_CHILD),
    E (389, LEAF),
    E (390, GET_PARENT),
    E (391, ROOT),
    E (392, SET_ELEM),
    E (393, GET_ELEM),
    E (394, RECURSIVE_ANALYZER),
    E (395, OUT_OF_RANGE),
    E (396, OP_SQUARE_BRACKETS),
    E (397, GET_LAST_CHILD),
    E (398, NO_LAST_CHILD),
    E (399, GET_N_CHILDREN),
    E (400, INSERT_AND_SLIDE),
    E (401, ARITHMETIC_PARSER),
    E (402, UNLINK_CHILD),
    E (403, CLEAR_LAST_CHILD),
    E (404, DUMP),
    E (405, SAVE),
    E (406, LOAD_TREE),
    E (407, FILE_FUNCS),
    E (407, FILE_VARS),
    E (408, LOAD_NODE),
    E (409, READ_NODE),
    E (410, EXCPECTED_SPACE),
    E (411, TRANSLATE),
    E (412, TRANSLATE_TREE),
    E (413, TRANSLATE_ARITHMETIC_TREE),
    E (414, UNKNOWN_CMD),
    E (415, UNKNOWN_OP),
    E (416, UNKNOWN_LOGIC)

};

#undef E

#define F(num, name) FLAG_##name = num

enum NODE_CONTENT : uint8_t
{
    F (1, NUMERIC),
    F (2, OPERATOR),
    F (3, FUNCTION),
    F (4, ARGUMENT),
    F (5, NUMERICD)
};

#undef F

#define TOKEN_BLOCK \
    T ( 1, UNTYPED), \
    T ( 2, HAI), \
    T ( 3, KTHXBYE), \
    T ( 4, ITZ), \
    T ( 5, WILE), \
    T ( 6, HAS), \
    T ( 7, R), \
    T ( 8, OF), \
    T ( 9, AN), \
    T (10, SUM), \
    T (11, DIFF), \
    T (12, PRODUKT), \
    T (13, QUOSHUNT), \
    T (14, BIGGR), \
    T (15, SMALLR), \
    T (16, WON), \
    T (17, EITHER), \
    T (18, A), \
    T (19, I), \
    T (20, NOT), \
    T (21, MKAY), \
    T (22, BOTH), \
    T (23, SAEM), \
    T (24, DIFFRINT), \
    T (25, VISIBLE), \
    T (26, GIMMEH), \
    T (27, O), \
    T (28, RLY_QM), \
    T (29, YA), \
    T (30, RLY), \
    T (31, NO), \
    T (32, WAI), \
    T (33, OIC), \
    T (34, IM), \
    T (35, IN), \
    T (36, YR), \
    T (37, UPPIN), \
    T (38, NERFIN), \
    T (39, TIL), \
    T (40, OUTTA), \
    T (41, HOW), \
    T (42, IZ), \
    T (43, IF), \
    T (44, YOU), \
    T (45, SAY), \
    T (46, SO), \
    T (47, OPENNG), \
    T (48, GETCH), \
    T (49, COS), \
    T (50, SIN), \
    T (51, DER)


#define T(num, name) TOKEN_##name = num
enum TOKEN_TYPES : uint32_t
{

    T (998,  FUNC),
    T (999,  LOOP),
    T (1000, NUM),
    T (1001, VAR),
    T (1002, IF_YOU),
    T (1003, BOTH_SAEM),
    T (1004, I_HAS),
    T (1005, HOW_IZ),
    T (1006, O_RLY_QM),
    T (1007, YA_RLY),
    T (1008, NO_WAI),
    T (1009, IM_IN),
    T (1010, IM_OUTTA),
    T (1011, UPPIN_YR),
    T (1012, NERFIN_YR),
    T (1013, SUM_OF),
    T (1014, DIFF_OF),
    T (1015, PRODUKT_OF),
    T (1016, QUOSHUNT_OF),
    T (1017, BIGGR_OF),
    T (1018, SMALLR_OF),
    T (1019, WON_OF),
    T (1020, EITHER_OF),
    T (1021, BOTH_OF),
    T (1022, IF_YOU_SAY),
    T (1023, I_HAS_A),
    T (1024, HOW_IZ_I),
    T (1025, IM_IN_YR),
    T (1026, IM_OUTTA_YR),
    T (1027, IF_YOU_SAY_SO),
    T (1028, I_IZ),
    TOKEN_BLOCK
};

#undef T

#define E(num, name) EM_##name = num
enum ERROR_MODES : uint32_t
{
    E (1, WARNING),
    E (2, ERROR)
};

#undef E

const uint32_t DEFAULT_STR_BUFFER_SIZE = 512;

enum TREE_STUFF
{
    NODE_ROOT          = 0,
    NODE_USER_FUNCTION = 1,
    NODE_NUMBER        = 2,
    NODE_VARIABLE      = 3,
    NODE_OPERATOR      = 4,
    NODE_LOGIC         = 5,
    NODE_MAIN          = 6,
    NODE_STD_FUNCTION  = 7,
    NODE_VAR_INIT      = 8,
    NODE_SERVICE       = 9,

    STD_FUNC_INPUT         = 1,
    STD_FUNC_OUTPUT        = 2,
    STD_FUNC_SIN           = 3,
    STD_FUNC_COS           = 4,
    STD_FUNC_GETCH         = 5,
    STD_FUNC_CALL_USER     = 6,
    STD_FUNC_DIFFERENTIATE = 7,
    STD_FUNC_SQRT          = 8,
    STD_FUNC_MAX           = 9,
    STD_FUNC_MIN           = 10,

    OP_EQUAL            = 1,
    OP_PLUS             = 2,
    OP_MINUS            = 3,
    OP_MULTIPLY         = 4,
    OP_DIVIDE           = 5,
    OP_POWER            = 6,
    OP_BOOL_EQUAL       = 7,
    OP_BOOL_NOT_EQUAL   = 8,
    OP_BOOL_LOWER       = 9,
    OP_BOOL_GREATER     = 10,
    OP_BOOL_NOT_LOWER   = 11,
    OP_BOOL_NOT_GREATER = 12,
    OP_BOOL_NOT         = 13,
    OP_BOOL_OR          = 14,
    OP_BOOL_AND         = 15,
    OP_PLUS_EQUAL       = 16,
    OP_MINUS_EQUAL      = 17,
    OP_MULTIPLY_EQUAL   = 18,
    OP_DIVIDE_EQUAL     = 19,

    LOGIC_IF            = 1,
    LOGIC_WHILE         = 2,
    LOGIC_ELSE          = 3,
    LOGIC_CONDITION     = 4,
    LOGIC_CONDITION_MET = 5,

    PERSONAL_CODE = 12,
    BUILD_NUMBER = 1

};



#endif // ERRORS_H_INCLUDED
