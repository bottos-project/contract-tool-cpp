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

        if (!getData<char[USER_NAME_LEN], UserDetail>(mycontract_name, strlen(mycontract_name), tablename,  strlen(tablename), sayhello.userName, strlen(sayhello.userName), userDetail) ){
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
