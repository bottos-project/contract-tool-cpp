#include "contractcomm.hpp"
#include "string.hpp"
#include "testRegUser.hpp"


#define PARAM_MAX_LEN (2048)

#define ERROR_PACK_FAIL (-1)
#define ERROR_UNPACK_FAIL (-2)
#define ERROR_SAVE_DB_FAIL (-3)
#define ERROR_METHOD_INVALID (-4)
    


static bool unpack_struct(MsgPackCtx *ctx, UserInfo *info)
{ 
    uint32_t size = 0;
    UNPACK_ARRAY(2)
    
    UNPACK_STR(info, userName, (USER_NAME_LEN+1))
    UNPACK_STR(info, userInfo, (USER_INFO_LEN+1))
    
    return 1;
}

static bool pack_struct(MsgPackCtx *ctx, UserBaseInfo *info)
{   
    PACK_ARRAY16(1)
    PACK_STR16(info, userInfo )

    return 1;
}

void init()
{
}

int start(char* method) 
{
    if (0 == strcmp("reguser", method))
    {
        char param[PARAM_MAX_LEN] = {0};
        uint32_t paramLen = 0;
        UserInfo userinfo = {{0}};       
        
        paramLen = getParam(param, PARAM_MAX_LEN);
        
        MsgPackCtx ctx;
        msgpack_init(&ctx, (char*)param, paramLen);
        bool unpackSuc = unpack_struct(&ctx, &userinfo);
        if (!unpackSuc)
        {
            char pstr[] = "unpack userinfo failed";
            myprints(pstr);

            return ERROR_UNPACK_FAIL;
        }

        UserBaseInfo userBaseInfo = {{0}};
        
        strcpy(userBaseInfo.userInfo, userinfo.userInfo);

        msgpack_init(&ctx, (char*)param, paramLen);

        bool packSuc = pack_struct(&ctx, &userBaseInfo);
        if (!packSuc)
        {
            char pstr[] = "pack userbaseinfo failed";
            myprints(pstr);

            return ERROR_PACK_FAIL;
        }
    
        char objname[] = "userreginfo";       
        
        uint32_t saveLen = setBinValue(objname, strlen(objname), userinfo.userName, strlen(userinfo.userName), ctx.buf, ctx.pos);
        if (0 == saveLen)
        {
            char pstr[] = "save db failed";
            myprints(pstr);

            return ERROR_SAVE_DB_FAIL;
        }  
                
        return 0;      
 
    }    
    else 
    {
        char pstr[] = "invalid method";
        prints(pstr, strlen(pstr));
        return ERROR_METHOD_INVALID;
    }
}


