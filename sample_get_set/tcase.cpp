#include "contractcomm.hpp"
#include "string.hpp"
#include "types.h"
#include "tcase.hpp"

#define PARAM_MAX_LEN (2048)

#define ERROR_PACK_FAIL (-1)
#define ERROR_UNPACK_FAIL (-2)
#define ERROR_SAVE_DB_FAIL (-3)
#define ERROR_METHOD_INVALID (-4)

static bool unpack_struct(MsgPackCtx *ctx, UserInfo *info)
{
    uint32_t size = 0;
    UNPACK_ARRAY(3)


    UNPACK_STR(info, userName, (USER_NAME_LEN + 1))
    myprints(info->userName);

    UNPACK_STR(info, userRole, (USER_ROLE_LEN + 1))
    
    
    UNPACK_U64(info, rcvHelloNum)
	
    string strval = strval + string(info->userName);
    strval = strval + ", info->userRole: " +  info->userRole;
    strval = strval + ", info->rcvHelloNum: ";
    PRINT_STRING_VALUE(strval) 
    PRINT_I_VALUE(info->rcvHelloNum)

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

/*only for verify*/
static bool unpack_struct(MsgPackCtx *ctx, UserDetail *info)
{
    uint32_t size = 0;
    UNPACK_ARRAY(2)


    UNPACK_STR(info, userRole, (USER_ROLE_LEN + 1))
    myprints(info->userRole);

    
    UNPACK_U64(info, rcvHelloNum)
	
    return 1;
}

template <class T>
static bool parseDBParam(char *param, uint64_t paramLen, T  & para)
{
    
    MsgPackCtx ctx;
    msgpack_init(&ctx, (char *)param, paramLen);

    return unpack_struct(&ctx, &para);

}


bool g_unpackSuc = 1;
template <class T> uint32_t  parseParam(T  & para)
{
    char param[PARAM_MAX_LEN] = {0};
    uint32_t paramLen = 0;

    
    paramLen = getParam(param, PARAM_MAX_LEN);
	
    MsgPackCtx ctx;
    msgpack_init(&ctx, (char *)param, paramLen);


    return unpack_struct(&ctx, &para);

	
    /*following g_unpackSuc can not be judged even it is 1 or true, why?*/
    bool unpackSuc = unpack_struct(&ctx, &para);
    myprints("unpack start .....:");
    PRINT_I_VALUE(unpackSuc)
    if (! g_unpackSuc )
    {
        char pstr[] = "unpack failed --????? !!!!!!!!!!!";
        myprints(pstr);
	PRINT_I_VALUE(g_unpackSuc)
        return ERROR_UNPACK_FAIL;
    }
}

template <class T, class K> uint32_t saveData(T& data, char *tablename, K& key)
{
    char param[PARAM_MAX_LEN] = {0};
    uint32_t paramLen = PARAM_MAX_LEN;
    MsgPackCtx ctx;

    msgpack_init(&ctx, (char *)param, paramLen);
    bool packSuc = pack_struct(&ctx, &data);
    PRINT_I_VALUE(packSuc);

    if (!packSuc)
    {
        char pstr[] = "pack failed";
        myprints(pstr);

        return ERROR_PACK_FAIL;
    }

    uint32_t saveLen = setBinValue(tablename, strlen(tablename), key, strlen(key), ctx.buf, ctx.pos);
    if (0 == saveLen)
    {
        char pstr[] = "save db failed";
        myprints(pstr);

        return ERROR_SAVE_DB_FAIL;
    }
   
    return OK;
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
	return 0;
    }
    
    myprints("mycontract_name is:");
    myprints(mycontract_name);
	
    if ( isMethod("reguser", method) )
    {
        UserInfo userinfo = {{0}};
        UserDetail userDetail = {{0}};
	UserDetail data2 = {{0}};	
        char tablename[] = "userdetail";

        if ( !parseParam<UserInfo>(userinfo) )  return ERROR_UNPACK_FAIL;
	
	myprints(userinfo.userName);
	myprints(userinfo.userRole);
	PRINT_I_VALUE(userinfo.rcvHelloNum);

        strcpy(userDetail.userRole, userinfo.userRole);
	userDetail.rcvHelloNum = userinfo.rcvHelloNum;

        //if ( !getData()) return 0;
	
        if (!saveData(userDetail, tablename, userinfo.userName)) return ERROR_SAVE_DB_FAIL;


	//verify data
	 	char param2[PARAM_MAX_LEN] = {0};
	        uint32_t ret = getBinValue(mycontract_name, strlen(mycontract_name), tablename, strlen(tablename), userinfo.userName, strlen(userinfo.userName), param2, PARAM_MAX_LEN);

		PRINT_I_VALUE(ret);

		if (!ret)
		{
		    myprints("getBinValue FAILED!");
		    return 0;
		}

		//use to unpack here!!!!
		if (!parseDBParam<UserDetail>(param2, PARAM_MAX_LEN, data2)) return ERROR_UNPACK_FAIL;

		//if ( !parseParam<UserDetail>(data2) )  return ERROR_UNPACK_FAIL;	
		

	//verify end

        return 0;  //return ok;

    }
    else if (isMethod("sayhello", method))	
    {
	SayHello sayhello = {{0}};
        UserDetail userDetail = {{0}};
        char tablename[] = "userdetail";
	UserDetail data2 = {{0}};	
	
	
        if ( !parseParam<SayHello>(sayhello) )  return ERROR_UNPACK_FAIL;
	
	char param2[PARAM_MAX_LEN] = {0};
	uint32_t ret = getBinValue(mycontract_name, strlen(mycontract_name), tablename, strlen(tablename), sayhello.userName, strlen(sayhello.userName), param2, PARAM_MAX_LEN);

	//use to unpack here!!!!
	if (!parseDBParam<UserDetail>(param2, PARAM_MAX_LEN, userDetail)) return ERROR_UNPACK_FAIL;
		
	if (!ret)
	{
	    myprints("getBinValue FAILED!");
	    return 0;
	}
	myprints("getBinValue : rcvHelloNum BEFORE :");
	PRINT_I_VALUE(userDetail.rcvHelloNum)
	userDetail.rcvHelloNum++;
	
	myprints("getBinValue : rcvHelloNum AFTER :");
	PRINT_I_VALUE(userDetail.rcvHelloNum)
	
	
        if (!saveData(userDetail, tablename, sayhello.userName)) return ERROR_SAVE_DB_FAIL;



	//verify data
	 	char param3[PARAM_MAX_LEN] = {0};
	        ret = getBinValue(mycontract_name, strlen(mycontract_name), tablename, strlen(tablename), sayhello.userName, strlen(sayhello.userName), param3, PARAM_MAX_LEN);

		if (!ret)
		{
		    myprints("getBinValue FAILED!");
		    return 0;
		}

		//use to unpack here!!!!
		if (!parseDBParam<UserDetail>(param3, PARAM_MAX_LEN, data2)) return ERROR_UNPACK_FAIL;
		myprints("getBinValue : NOW VERIFY:=> rcvHelloNum IS :");
		PRINT_I_VALUE(data2.rcvHelloNum)


	//verify end
    }	
    else
    {
        char pstr[] = "invalid method";
        prints(pstr, strlen(pstr));
        return ERROR_METHOD_INVALID;
    }
}
