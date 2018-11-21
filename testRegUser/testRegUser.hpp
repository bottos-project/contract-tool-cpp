#define USER_NAME_LEN (20)
#define USER_INFO_LEN (20)



extern "C" {
    int reguser();
}

//@abi action reguser
struct UserInfo {
    char userName[USER_NAME_LEN];
    char userInfo[USER_INFO_LEN];
};


//@abi table userinfo:[index_type:string, key_names:userName, key_types:string]
struct UserBaseInfo {
    char userInfo[USER_INFO_LEN];
};
