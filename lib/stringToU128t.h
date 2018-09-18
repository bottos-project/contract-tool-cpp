#include "types.h"
#include "uint128_t.h"
// 0123456789abcdefghijklmnopqrstuvwxyz-. len:38
    static uint8_t charToIdx( char c ) 
    {
        if( c >= '0' && c <= '9' ) return (c - '0');
        if( c >= 'a' && c <= 'z' ) return (c - 'a') + 10;

        if (c == '-') return 36;
        if (c == '.') return 37;

        return 0;
    }
    
    uint128_t stringToEncodeName(const char *str)
    {
        uint64_t upper = 0, lower = 0;

        uint32_t len = 0;

        while( str[len] ) ++len;

        uint64_t c = 0;

        for( uint32_t i = 0; i < 21 ; ++i ) 
        {
            
            printi(100+i);
            
            c = 0;
            
            if( i < len ) c = charToIdx( str[i] );

            c &= 0x3f;

            printi(c);

            if( i < 10 )
            {
                c <<= (64-6*(i+1));
                printi(c);
                upper |= c;
            }
            else if (i == 10)
            {
                uint64_t c_left4bit = c >> 2;
                uint64_t c_right2bit = c & 3;

                printi(c_left4bit);
                printi(c_right2bit);

                upper |= c_left4bit;
                lower |= c_right2bit<<62;
            }
            else{
                c <<= (62-6*(i-10));
                printi(c);

                lower |= c;
            }

            printi(upper);
            printi(lower);
            
        }

        uint128_t value(upper, lower);
        
        return value;
    }

    #define ENCODE_NAME(X) stringToEncodeName(#X)