#include "contractcomm.hpp"
#include "testRegUser.hpp"


#define PARAM_MAX_LEN (2048)

#define ERROR_PACK_FAIL (1)
#define ERROR_UNPACK_FAIL (2)
#define ERROR_SAVE_DB_FAIL (3)    


static bool unpack_struct(MsgPackCtx *ctx, UserInfo *info)
{ 
    uint32_t size = 0;
    UNPACK_ARRAY(2)
    
    UNPACK_STR(info, userName, (USER_NAME_LEN+1))
    UNPACK_STR(info, userInfo, (USER_INFO_LEN+1))
    
    return true;
}

static bool pack_struct(MsgPackCtx *ctx, UserBaseInfo *info)
{   
    PACK_ARRAY16(1)
    PACK_STR16(info, userInfo )

    return true;
}

int reguser() 
{
    UserInfo userinfo = {{0}};
    
    if ( !parseParam<UserInfo>(userinfo) )  return ERROR_UNPACK_FAIL;

    myprints(userinfo.userName);
    myprints(userinfo.userInfo);

    UserBaseInfo userBaseInfo = {{0}};
    
    strcpy(userBaseInfo.userInfo, userinfo.userInfo);

    char tablename[] = "userreginfo"; 

    if (!saveData(userBaseInfo, tablename, userinfo.userName)) return ERROR_SAVE_DB_FAIL;
            
    return 0;      

}

