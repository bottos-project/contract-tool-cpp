#include "contractcomm.hpp"
#include "string.hpp"
#include "types.h"
#include "tcase.hpp"

#define PARAM_MAX_LEN (2048)

#define ERROR_PACK_FAIL (-1)
#define ERROR_UNPACK_FAIL (-2)
#define ERROR_SAVE_DB_FAIL (-3)
#define ERROR_METHOD_INVALID (-4)
#define ERROR_GET_CONTRACT_NAME_FAIL (-5)
#define ERROR_GET_DB_FAIL (-5)

static bool unpack_struct(MsgPackCtx *ctx, UserInfo *info)
{
    uint32_t size = 0;
    UNPACK_ARRAY(3)

    UNPACK_STR(info, userName, (USER_NAME_LEN + 1))

    UNPACK_STR(info, userRole, (USER_ROLE_LEN + 1))
    
    UNPACK_U64(info, rcvHelloNum)
	
    return 1;
}

static bool unpack_struct(MsgPackCtx *ctx, SayHello *sayhello)
{
    uint32_t size = 0;
    UNPACK_ARRAY(1)

    UNPACK_STR(sayhello, userName, (USER_NAME_LEN + 1))

    return 1;
}

static bool pack_struct(MsgPackCtx *ctx, UserDetail *info)
{
    PACK_ARRAY16(2)
    
    PACK_STR16(info, userRole)
    
    PACK_U64(info, rcvHelloNum)
	
    return 1;
}

static bool unpack_struct(MsgPackCtx *ctx, UserDetail *info)
{
    uint32_t size = 0;
    UNPACK_ARRAY(2)

    UNPACK_STR(info, userRole, (USER_ROLE_LEN + 1))
    
    UNPACK_U64(info, rcvHelloNum)
	
    return 1;
}

template <class T> static bool parseDBParam(char *param, uint64_t paramLen, T  & para)
{
    
    MsgPackCtx ctx;
    msgpack_init(&ctx, (char *)param, paramLen);

    return unpack_struct(&ctx, &para);

}

template <class T> uint32_t parseParam(T  & para)
{
    char param[PARAM_MAX_LEN] = {0};
    uint32_t paramLen = 0;

    
    paramLen = getParam(param, PARAM_MAX_LEN);
	
    MsgPackCtx ctx;
    msgpack_init(&ctx, (char *)param, paramLen);


    return unpack_struct(&ctx, &para);
}

template <class T, class K> uint32_t saveData(T& data, char *tablename, K& key)
{
    char param[PARAM_MAX_LEN] = {0};
    uint32_t paramLen = PARAM_MAX_LEN;
    MsgPackCtx ctx;

    msgpack_init(&ctx, (char *)param, paramLen);
    bool packSuc = pack_struct(&ctx, &data);

    if (!packSuc)
    {
        char pstr[] = "pack failed";
        myprints(pstr);

        return 0;
    }

    uint32_t saveLen = setBinValue(tablename, strlen(tablename), key, strlen(key), ctx.buf, ctx.pos);
    if (0 == saveLen)
    {
        char pstr[] = "save db failed";
        myprints(pstr);

        return 0;
    }
   
    return 1;
}

template <class T, class K> uint32_t getData(char *contract_name, uint64_t len_contract_name, char *table_name, uint64_t len_table_name,     T & keyword,  uint64_t len_keyword, K & return_data)
{
	char param2[PARAM_MAX_LEN] = {0};
	uint32_t ret = getBinValue(contract_name, len_contract_name, table_name, len_table_name, keyword, len_keyword, param2, PARAM_MAX_LEN);

	if (!parseDBParam<UserDetail>(param2, PARAM_MAX_LEN, return_data)) return 0;
		
	if (!ret)
	{
	    myprints("getBinValue failed!");
	    return 0;
	}
	
	return 1;
}


bool isMethod(const char* method, const char* input)
{
    return ((0 == strcmp(input, method)) ? true : false);
}

int start(char *method)
{
    char mycontract_name[30] = "";
    getCtxName(mycontract_name, sizeof(mycontract_name));
    
    if (strlen(mycontract_name) <= 0)
    {
	myprints("ERROR: Get my contract name failed.");
	return ERROR_GET_CONTRACT_NAME_FAIL;
    }
    
    if ( isMethod("reguser", method) )
    {
        UserInfo userinfo = {{0}};
        UserDetail userDetail = {{0}};
	UserDetail data2 = {{0}};	
        char tablename[] = "userdetail";

        if ( !parseParam<UserInfo>(userinfo) )  return ERROR_UNPACK_FAIL;

        strcpy(userDetail.userRole, userinfo.userRole);
	userDetail.rcvHelloNum = userinfo.rcvHelloNum;
	
        if (!saveData(userDetail, tablename, userinfo.userName)) return ERROR_SAVE_DB_FAIL;

        return 0;

    }
    else if (isMethod("sayhello", method))	
    {
	SayHello sayhello = {{0}};
        UserDetail userDetail = {{0}};
        char tablename[] = "userdetail";
	UserDetail data2 = {{0}};	
	
        if ( !parseParam<SayHello>(sayhello) )  return ERROR_UNPACK_FAIL;
	
	if (!getData(mycontract_name, strlen(mycontract_name), tablename,  strlen(tablename), sayhello.userName, strlen(sayhello.userName), userDetail) ){
		myprints("getData failed!");
		return ERROR_GET_DB_FAIL;
	}
	
	userDetail.rcvHelloNum++;

        if (!saveData(userDetail, tablename, sayhello.userName)) return ERROR_SAVE_DB_FAIL;
	
	return 0;
    }	
    else
    {
        char pstr[] = "invalid method";
        prints(pstr, strlen(pstr));
        return ERROR_METHOD_INVALID;
    }
}
