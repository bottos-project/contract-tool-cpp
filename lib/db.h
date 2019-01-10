
#pragma once

extern "C" {

    uint32_t setBinValue(char* object, uint32_t objLen, char* key, uint32_t keyLen, char *value, uint32_t valLen);

    uint32_t getBinValue(char *contract, uint32_t contractLen, char* object, uint32_t objLen, char* key, uint32_t keyLen, char *valueBuf, uint32_t valueBufLen);
}

