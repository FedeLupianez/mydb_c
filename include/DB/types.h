typedef enum {
    INT,
    BYTE,
    FLOAT,
    STRING,
    CHAR,
    VOID
} Type;

Type get_type(char* name);
char* get_type_name(Type type);
