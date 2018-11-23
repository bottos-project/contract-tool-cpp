#include "contractcomm.hpp"
#include "dbSaveRead.hpp"
#define ERROR_PACK_FAIL (1)
#define ERROR_UNPACK_FAIL (2)
#define ERROR_SAVE_DB_FAIL (3)    
#define ERROR_GET_CONTRACT_NAME_FAIL (4)
#define ERROR_GET_DB_FAIL (5)



static bool unpack_struct(MsgPackCtx *ctx, TestStruct *info)
{    
    uint32_t size = 0;
    UNPACK_ARRAY(2)

    UNPACK_U32(info,valueA)
    UNPACK_U32(info,valueB)
    
    return true;
}

static bool pack_struct(MsgPackCtx *ctx, TestStruct *info)
{    
    PACK_ARRAY16(2)

    PACK_U32(info,valueA)
    PACK_U32(info,valueB) 

    return true;
}

int start() 
{
    TestStruct testStruct = {{0}};
    
    if ( !parseParam<TestStruct>(testStruct) )  return ERROR_UNPACK_FAIL;
    
    myprints("data from input param:");
    printi(testStruct.valueA);
    printi(testStruct.valueB);

    testStruct.valueA = 3;
    testStruct.valueB = 4;

    char tableName[] = "testTableName";
    char keyName[] = "testKeyName";

    if (!saveData(testStruct, tableName, keyName)) return ERROR_SAVE_DB_FAIL;


    if (!getData(nullptr, tableName, keyName, testStruct) ){
        myprints("getData failed!");
        return ERROR_GET_DB_FAIL;
    }

    printi(testStruct.valueA);
    printi(testStruct.valueB);

    return 0;
}