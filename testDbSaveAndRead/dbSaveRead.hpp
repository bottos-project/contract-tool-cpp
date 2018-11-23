
extern "C" {
    int helloworld();
    int start();    
}


//@abi action start
struct TestStruct {
    uint32_t valueA;
    uint32_t valueB;
};

//@abi table testTableName:[index_type:string, key_names:keyName, key_types:string]
struct SaveStruct {
    uint32_t valueA;
    uint32_t valueB;
};

