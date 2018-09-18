#ifndef CONTRACT_COMMON_H_
#define CONTRACT_COMMON_H_

#include "types.hpp"
#include "string.hpp"
#include "db.h"
#include "msgpack.hpp"
#include "const.h"
#include "uint256_t.h"

extern "C" {
    void prints(char *str, uint32_t len);
    void printi(uint64_t u);
    uint32_t getParam(char *param, uint32_t buf_len);
    void init();
    int start(char* method);

    uint32_t getSender(char *str , uint32_t len);
    uint32_t getCtxName(char *str , uint32_t len);

    static inline void myprints(char *s) 
    {
        prints(s, strlen(s));
    }
    
    static inline void myprintstring(string s)
    {
        prints(s.get_data(), s.get_strlen());
	
    }

    uint32_t callTrx(char *contract , uint32_t contractLen, char *method, uint32_t methodLen, char *buf , uint32_t bufLen );
    bool isAccountExist(char *name,  uint32_t nameLen);
}



#define EMSCRIPTEN_KEEPALIVE

#define STR_ARRAY_LEN(STR_LEN) (STR_LEN + 1)

#define PRINT_I(VAR_NAME, ELEM_NAME)  do { char elemstr[] = #ELEM_NAME ; myprints(elemstr); printi(VAR_NAME->ELEM_NAME); }while(0);
#define PRINT_STR(VAR_NAME, ELEM_NAME)  do { char elemstr[] = #ELEM_NAME ; myprints(elemstr); myprints(VAR_NAME->ELEM_NAME); }while(0);
#define PRINT_STRING(VAR_NAME, ELEM_NAME)  do { char elemstr[] = #ELEM_NAME ; myprints(elemstr); myprintstring(VAR_NAME->ELEM_NAME); }while(0);
#define PRINT_STRUCT(VAR_NAME, ELEM_NAME) do { print_struct(&VAR_NAME->ELEM_NAME); }while(0);

#define PRINT_I_VALUE(value)  do { printi(value); }while(0);
#define PRINT_STRING_VALUE(string_val) do { myprintstring(string_val); }while(0);


#define UNPACK_STR(VAR_NAME, ELEM_NAME, ELEM_LEN) do { if (!unpack_str(ctx, VAR_NAME->ELEM_NAME, ELEM_LEN, &size)) return 0; }while(0);
#define UNPACK_STRING(VAR_NAME, ELEM_NAME, ELEM_LEN) do { VAR_NAME->ELEM_NAME = string(ELEM_LEN);  if (!unpack_string(ctx, VAR_NAME->ELEM_NAME.get_data(), ELEM_LEN, &size)) return 0; }while(0);

#define UNPACK_U32(VAR_NAME, ELEM_NAME) do { if (!unpack_u32(ctx, &VAR_NAME->ELEM_NAME)) return 0; }while(0); 
#define UNPACK_U64(VAR_NAME, ELEM_NAME) do { if (!unpack_u64(ctx, &VAR_NAME->ELEM_NAME)) return 0; }while(0);
#define UNPACK_ARRAY(SIZE_NUM)  do { if (!unpack_array(ctx, &size)) return 0; if (SIZE_NUM != size) return 0; }while(0);
#define UNPACK_STRUCT(VAR_NAME, ELEM_NAME) do { if (!unpack_struct(ctx, &VAR_NAME->ELEM_NAME)) return 0; }while(0);

#define PACK_ARRAY16(SIZE_NUM) do { if (!pack_array16(ctx, SIZE_NUM)) return 0; }while(0);
#define PACK_U32(VAR_NAME, ELEM_NAME) do { if (!pack_u32(ctx, VAR_NAME->ELEM_NAME)) return 0; }while(0);
#define PACK_U64(VAR_NAME, ELEM_NAME) do { if (!pack_u64(ctx, VAR_NAME->ELEM_NAME)) return 0; }while(0);
#define PACK_STR16(VAR_NAME, ELEM_NAME) do { if (!pack_str16(ctx, VAR_NAME->ELEM_NAME, strlen(VAR_NAME->ELEM_NAME))) return 0; }while(0);
#define PACK_STRING(VAR_NAME, ELEM_NAME) do { if (!pack_str16(ctx, VAR_NAME->ELEM_NAME.get_data(), VAR_NAME->ELEM_NAME.get_strlen())) return 0; }while(0);

#define PACK_U256(VAR_NAME, ELEM_NAME) do { \
    uint8_t bytesValue[U256_BYTE_LEN]; \
    VAR_NAME->ELEM_NAME.toBytes(bytesValue); \
    if (!pack_bin16(ctx, bytesValue, U256_BYTE_LEN)) return 0; \
}while(0)


#define UNPACK_U256(VAR_NAME, ELEM_NAME) do { \
    uint8_t bytesValue[32]; \
    uint32_t u256MaxLen = 32; \
    if (!unpack_bin(ctx, bytesValue, &u256MaxLen)) return 0; \
    VAR_NAME->ELEM_NAME.setBybytes(bytesValue); \
}while(0)


#define CHECK_ACCOUNT_VALID(ACCOUNT) do { if (!isAccountExist(info->ACCOUNT)) return false;} while(0);

#endif