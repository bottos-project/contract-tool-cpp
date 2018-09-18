#ifndef __UINT256_T__
#define __UINT256_T__


#include "uint128_t.h"
#define UINT64_CRITICAL_VALUE  "18446744073709551615"
#define UINT128_CRITICAL_VALUE "340282366920938463463374607431768211455"
#define CHAR_BIT               8
#define BYTE_LEN               32
#define SIZE256                ((sizeof(uint128_t) + sizeof(uint128_t)) * CHAR_BIT)

#define U256_BYTE_LEN (32)

extern "C" {
    
    void printi(uint64_t u);
}
char  number256[SIZE256 + 1] = {0};

class uint256_t;

class uint256_t {
    private:

        uint128_t UPPER, LOWER;
		static const unsigned int size256 = (sizeof(uint128_t) + sizeof(uint128_t)) * CHAR_BIT;

    public:
		uint256_t():UPPER(uint128_t(0)),LOWER(uint128_t(0)){}
		uint256_t(const uint256_t & rhs):UPPER(rhs.UPPER),LOWER(rhs.LOWER){}
		uint256_t(int             value):LOWER(uint128_t(value)), UPPER(0) { if(value < 0) UPPER = uint128_t(-1); }
		uint256_t(unsigned int    value):LOWER(uint128_t(value)), UPPER(0) {}
		uint256_t(float           value):LOWER(uint128_t(value)), UPPER(0) {}
		uint256_t(double          value):LOWER(uint128_t(value)), UPPER(0) {}
		uint256_t(uint256_t & rhs):UPPER(rhs.UPPER), LOWER(rhs.LOWER)
		{
			if (this != &rhs){
				rhs.UPPER = uint128_t(0);
				rhs.LOWER = uint128_t(0);
			}
		}
		uint256_t(uint128_t lower){
			this->UPPER = uint128_t(0);
			this->LOWER = lower;
		}
		uint256_t(uint128_t upper , uint128_t lower) {
			UPPER = upper;
			LOWER = lower;
		}
		uint256_t(uint64_t upper_upper ,uint64_t upper_lower, uint64_t lower_upper , uint64_t lower_lower) {
			UPPER = uint128_t(upper_upper , upper_lower);
			LOWER = uint128_t(lower_upper , lower_lower);
		}
		uint256_t(const char * number)
		{
			UPPER = uint128_t(0);
			LOWER = uint128_t(0);
			if (number) {
				int  radix  = 10;
				bool minus  = false;

				const char *ptr = number;
				if(*ptr == '-') {
					++ptr;
					minus = true;
				}
				// check if there is hexadecimal
				if(*ptr != '\0') {

					if(*ptr == '0') {
						radix = 8;
						++ptr;
						if(*ptr != '\0') {
							if(*ptr == 'x') {
								radix = 16;
								++ptr;
							}
						}
					}

					while(*ptr != '\0') {
						unsigned int n;
						const char   ch = *ptr;

						if(ch >= 'A' && ch <= 'Z') {
							if(((ch - 'A') + 10) < radix) {
								n = (ch - 'A') + 10;
							} else {
								break;
							}
						} else if(ch >= 'a' && ch <= 'z') {
							if(((ch - 'a') + 10) < radix) {
								n = (ch - 'a') + 10;
							} else {
								break;
							}
						} else if(ch >= '0' && ch <= '9') {
							if((ch - '0') < radix) {
								n = (ch - '0');
							} else {
								break;
							}
						} else {
							break;
						}

						(*this) *= radix;
						(*this) += n;

						++ptr;
					}
				}
			
				if(minus) {
					*this = -*this;
				}
			}
		}
		char *to_str(unsigned int radix = 10) const {

			if(*this == 0) {
				return nullptr;
			}
			if(radix < 2 || radix > 37) {
				return nullptr;
			}
			int i = size256 - 1;
			number256[sizeof(number256) - 1] = '\0';

			uint256_t cur(*this);
			while (cur != 0 && i) {
				uint256_t remainder(0);
				divide(cur, uint256_t(radix), cur, remainder);
				number256[--i] = "0123456789abcdefghijklmnopqrstuvwxyz"[remainder.to_int()];
			}

			return (char *)(number256 + i);
		}
		
		void set(uint64_t upper_upper ,uint64_t upper_lower, uint64_t lower_upper , uint64_t lower_lower) {
			UPPER.set(upper_upper, upper_lower);
			LOWER.set(lower_upper, lower_lower);
		}

		void getdata(uint64_t value[4])
		{
			value[0] = UPPER.upper();
			value[1] = UPPER.lower();
			value[2] = LOWER.upper();
			value[3] = LOWER.lower();
		}

		void toBytes(uint8_t value[U256_BYTE_LEN])
		{
			uint64_t upper_upper, upper_lower, lower_upper, lower_lower;
			upper_upper = UPPER.upper();
			upper_lower = UPPER.lower();
			lower_upper = LOWER.upper();
			lower_lower = LOWER.lower();
			uint32_t loop;
			for (loop = 0;loop<8;loop++)
			{
				value[   loop]  = (upper_upper>>(56 - 8*loop))&0xFF;
				value[8 +loop]  = (upper_lower>>(56 - 8*loop))&0xFF;
				value[16+loop]  = (lower_upper>>(56 - 8*loop))&0xFF;
				value[24+loop]  = (lower_lower>>(56 - 8*loop))&0xFF;        
			}
		}
				
		void setBybytes (uint8_t value[U256_BYTE_LEN]) {
			uint64_t upper_upper = 0, upper_lower = 0, lower_upper = 0, lower_lower = 0;

			uint32_t loop;
			for (loop = 0;loop<8;loop++)
			{
				upper_upper +=    ((uint64_t)(value[    loop]))<< (56 - 8*loop);
				upper_lower +=    ((uint64_t)(value[8 + loop]))<< (56 - 8*loop);
				lower_upper +=    ((uint64_t)(value[16 + loop]))<< (56 - 8*loop);
				lower_lower +=    ((uint64_t)(value[24 + loop]))<< (56 - 8*loop);
			}			

			UPPER.set(upper_upper, upper_lower);
			LOWER.set(lower_upper, lower_lower);
		}
		
        template <typename T>
        uint256_t(const T & rhs):UPPER(uint128_t(0)), LOWER(rhs){}
        template <typename S, typename T>
        uint256_t(const S & upper_rhs, const T & lower_rhs):UPPER(upper_rhs), LOWER(lower_rhs){}
        template <typename R, typename S, typename T, typename U>
        uint256_t(const R & upper_lhs, const S & lower_lhs, const T & upper_rhs, const U & lower_rhs):UPPER(upper_lhs, lower_lhs), LOWER(upper_rhs, lower_rhs){}

		uint256_t & operator=(const uint256_t & rhs){
			UPPER = rhs.UPPER;
			LOWER = rhs.LOWER;
			return *this;
		}
		uint256_t & operator=(uint256_t & rhs){
			if (this != &rhs){
				UPPER = rhs.UPPER;
				LOWER = rhs.LOWER;
				rhs.UPPER = uint128_t(0);
				rhs.LOWER = uint128_t(0);
			}
			return *this;
		}
		int to_int() const {
			return (int)LOWER.lower();
		}
		template <typename T>
		uint256_t & operator=(const T & rhs){
			UPPER = uint128_t(0);
			LOWER = rhs;
			return *this;
		}

		operator bool() const{
			return (bool) (UPPER | LOWER);
		}
		operator uint8_t() const{
			return (uint8_t) LOWER;
		}
		operator uint16_t() const{
			return (uint16_t) LOWER;
		}
		operator uint32_t() const{
			return (uint32_t) LOWER;
		}
        operator uint64_t() const{
			return (uint64_t) LOWER;
		}
        operator uint128_t() const{
			return LOWER;
		}
        // Bitwise Operators
        uint256_t operator&(const uint128_t & rhs) const{
			return uint256_t(uint128_t(0), LOWER & rhs);
		}
        uint256_t operator&(const uint256_t & rhs) const {
			return uint256_t(UPPER & rhs.UPPER, LOWER & rhs.LOWER);
		}
        template <typename T>
        uint256_t operator&(const T & rhs) const{
            return uint256_t(uint128_t(0), LOWER & (uint128_t) rhs);
        }
        uint256_t & operator&=(const uint128_t & rhs){
			UPPER  = uint128_t(0);
			LOWER &= rhs;
			return *this;
		}
        uint256_t & operator&=(const uint256_t & rhs){
			UPPER &= rhs.UPPER;
			LOWER &= rhs.LOWER;
			return *this;
		}
        template <typename T>
        uint256_t & operator&=(const T & rhs){
            UPPER = uint128_t(0);
            LOWER &= rhs;
            return *this;
        }
        uint256_t operator|(const uint128_t & rhs) const{
			return uint256_t(UPPER , LOWER | rhs);
		}
        uint256_t operator|(const uint256_t & rhs) const{
			return uint256_t(UPPER | rhs.UPPER, LOWER | rhs.LOWER);
		}
        template <typename T>
        uint256_t operator|(const T & rhs) const{
            return uint256_t(UPPER, LOWER | uint128_t(rhs));
        }
        uint256_t & operator|=(const uint128_t & rhs){
			LOWER |= rhs;
			return *this;
		}
        uint256_t & operator|=(const uint256_t & rhs){
			UPPER |= rhs.UPPER;
			LOWER |= rhs.LOWER;
			return *this;
		}
        template <typename T>
        uint256_t & operator|=(const T & rhs){
            LOWER |= (uint128_t) rhs;
            return *this;
        }
        uint256_t operator^(const uint128_t & rhs) const{
			return uint256_t(UPPER, LOWER ^ rhs);
		}
		uint256_t operator^(const uint256_t & rhs) const{
			return uint256_t(UPPER ^ rhs.UPPER, LOWER ^ rhs.LOWER);
		}
        template <typename T>
        uint256_t operator^(const T & rhs) const{
            return uint256_t(UPPER, LOWER ^ (uint128_t) rhs);
        }
        uint256_t & operator^=(const uint128_t & rhs){
			LOWER ^= rhs;
			return *this;
		}

		uint256_t & operator^=(const uint256_t & rhs){
			UPPER ^= rhs.UPPER;
			LOWER ^= rhs.LOWER;
			return *this;
		}
        template <typename T>
        uint256_t & operator^=(const T & rhs){
            LOWER ^= (uint128_t) rhs;
            return *this;
        }
        uint256_t operator~() const{
			return uint256_t(~UPPER, ~LOWER);
		}
        // Bit Shift Operators
        uint256_t operator<<(const uint128_t & rhs) const{
			return *this << uint256_t(rhs);
		}
		uint256_t operator<<(const uint256_t & rhs) const{
			const uint128_t shift = rhs.LOWER;
			if (((bool) rhs.UPPER) || (shift >= uint128_t(256))){
				return uint256_t(0);
			}
			else if (shift == uint128_t(128)){
				return uint256_t(LOWER, uint128_t(0));
			}
			else if (shift == uint128_t(0)){
				return *this;
			}
			else if (shift < uint128_t(128)){
				return uint256_t((UPPER << shift) + (LOWER >> (uint128_t(128) - shift)), LOWER << shift);
			}
			else if ((uint128_t(256) > shift) && (shift > uint128_t(128))){
				return uint256_t(LOWER << (shift - uint128_t(128)), uint128_t(0));
			}
			else{
				return uint256_t(0);
			}
		}
        template <typename T>
        uint256_t operator<<(const T & rhs) const{
            return *this << uint256_t(rhs);
        }
        uint256_t & operator<<=(const uint128_t & shift){
			return *this <<= uint256_t(shift);
		}

		uint256_t & operator<<=(const uint256_t & shift){
			*this = *this << shift;
			return *this;
		}
        template <typename T>
        uint256_t & operator<<=(const T & rhs){
            *this = *this << uint256_t(rhs);
            return *this;
        }
        uint256_t operator>>(const uint128_t & rhs) const{
			return *this >> uint256_t(rhs);
		}

		uint256_t operator>>(const uint256_t & rhs) const{
			const uint128_t shift = rhs.LOWER;
			if (((bool) rhs.UPPER) | (shift >= uint128_t(256))){
				return uint256_t(0);
			}
			else if (shift == uint128_t(128)){
				return uint256_t(UPPER);
			}
			else if (shift == uint128_t(0)){
				return *this;
			}
			else if (shift < uint128_t(128)){
				return uint256_t(UPPER >> shift, (UPPER << (uint128_t(128) - shift)) + (LOWER >> shift));
			}
			else if ((uint128_t(256) > shift) && (shift > uint128_t(128))){
				return uint256_t(UPPER >> (shift - uint128_t(128)));
			}
			else{
				return uint256_t(0);
			}
		}
        template <typename T>
        uint256_t operator>>(const T & rhs) const{
            return *this >> uint256_t(rhs);
        }

        uint256_t & operator>>=(const uint128_t & shift){
			return *this >>= uint256_t(shift);
		}

		uint256_t & operator>>=(const uint256_t & shift){
			*this = *this >> shift;
			return *this;
		}
        template <typename T>
        uint256_t & operator>>=(const T & rhs){
            *this = *this >> uint256_t(rhs);
            return *this;
        }

        // Logical Operators
        bool operator!() const{
			return ! (bool) *this;
		}

		bool operator&&(const uint128_t & rhs) const{
			return (*this && uint256_t(rhs));
		}

		bool operator&&(const uint256_t & rhs) const{
			return ((bool) *this && (bool) rhs);
		}
        template <typename T>
        bool operator&&(const T & rhs) const{
            return ((bool) *this && rhs);
        }

        bool operator||(const uint128_t & rhs) const{
			return (*this || uint256_t(rhs));
		}

		bool operator||(const uint256_t & rhs) const{
			return ((bool) *this || (bool) rhs);
		}
        template <typename T>
        bool operator||(const T & rhs) const{
            return ((bool) *this || rhs);
        }

        // Comparison Operators
        bool operator==(const uint128_t & rhs) const{
			return (*this == uint256_t(rhs));
		}
		bool operator==(const uint256_t & rhs) const{
			return ((UPPER == rhs.UPPER) && (LOWER == rhs.LOWER));
		}
		bool operator==(const int & rhs) const{
			if (UPPER == 0) {
				return (LOWER == rhs);
			} else {
				return false;
			}
		}
        template <typename T>
        bool operator==(const T & rhs) const{
            return (!UPPER && (LOWER == uint128_t(rhs)));
        }
        bool operator!=(const uint128_t & rhs) const{
			return (*this != uint256_t(rhs));
		}
		bool operator!=(const uint256_t & rhs) const{
			return ((UPPER != rhs.UPPER) | (LOWER != rhs.LOWER));
		}
        template <typename T>
        bool operator!=(const T & rhs) const{
            return ((bool) UPPER | (LOWER != uint128_t(rhs)));
        }
        bool operator>(const uint128_t & rhs) const{
			return (*this > uint256_t(rhs));
		}

		bool operator>(const uint256_t & rhs) const{
			if (UPPER == rhs.UPPER){
				return (LOWER > rhs.LOWER);
			}
			if (UPPER > rhs.UPPER){
				return true;
			}
			return false;
		}
        template <typename T>
        bool operator>(const T & rhs) const{
            return ((bool) UPPER | (LOWER > uint128_t(rhs)));
        }
        bool operator<(const uint128_t & rhs) const{
			return (*this < uint256_t(rhs));
		}

		bool operator<(const uint256_t & rhs) const{
			if (UPPER == rhs.UPPER){
				return (LOWER < rhs.LOWER);
			}
			if (UPPER < rhs.UPPER){
				return true;
			}
			return false;
		}
        template <typename T>
        bool operator<(const T & rhs) const{
            return (!UPPER)?(LOWER < uint128_t(rhs)):false;
        }
        bool operator>=(const uint128_t & rhs) const{
			return (*this >= uint256_t(rhs));
		}

		bool operator>=(const uint256_t & rhs) const{
			return ((*this > rhs) | (*this == rhs));
		}
        template <typename T>
        bool operator>=(const T & rhs) const{
            return ((*this > rhs) | (*this == rhs));
        }
        bool operator<=(const uint128_t & rhs) const{
			return (*this <= uint256_t(rhs));
		}

		bool operator<=(const uint256_t & rhs) const{
			return ((*this < rhs) | (*this == rhs));
		}
        template <typename T>
        bool operator<=(const T & rhs) const{
            return ((*this < rhs) | (*this == rhs));
        }
        // Arithmetic Operators
        uint256_t operator+(const uint128_t & rhs) const{
			return *this + uint256_t(rhs);
		}

		uint256_t operator+(const uint256_t & rhs) const{
			return uint256_t(UPPER + rhs.UPPER + (((LOWER + rhs.LOWER) < LOWER)?uint128_t(1):uint128_t(0)), LOWER + rhs.LOWER);
		}
        template <typename T>
        uint256_t operator+(const T & rhs) const{
            return uint256_t(UPPER + ((LOWER + (uint128_t) rhs) < LOWER), LOWER + (uint128_t) rhs);
        }
        uint256_t & operator+=(const uint128_t & rhs){
			return *this += uint256_t(rhs);
		}

		uint256_t & operator+=(const uint256_t & rhs){
			UPPER = rhs.UPPER + UPPER + ((LOWER + rhs.LOWER) < LOWER);
			LOWER = LOWER + rhs.LOWER;
			return *this;
		}
        template <typename T>
        uint256_t & operator+=(const T & rhs){
            UPPER = UPPER + ((LOWER + rhs) < LOWER);
            LOWER = LOWER + rhs;
            return *this;
        }
        uint256_t operator-(const uint128_t & rhs) const{
			return *this - uint256_t(rhs);
		}

		uint256_t operator-(const uint256_t & rhs) const{
			return uint256_t(UPPER - rhs.UPPER - ((LOWER - rhs.LOWER) > LOWER), LOWER - rhs.LOWER);
		}
        template <typename T>
        uint256_t operator-(const T & rhs) const{
            return uint256_t(UPPER - ((LOWER - rhs) > LOWER), LOWER - rhs);
        }
        uint256_t & operator-=(const uint128_t & rhs){
			return *this -= uint256_t(rhs);
		}

		uint256_t & operator-=(const uint256_t & rhs){
			*this = *this - rhs;
			return *this;
		}
        template <typename T>
        uint256_t & operator-=(const T & rhs){
            *this = *this - rhs;
            return *this;
        }
        uint256_t operator*(const uint128_t & rhs) const{
			return *this * uint256_t(rhs);
		}

		uint256_t operator*(const uint256_t & rhs) const{
			// split values into 4 64-bit parts
			uint128_t top[4]    = {UPPER.upper(), UPPER.lower(), LOWER.upper(), LOWER.lower()};
			uint128_t bottom[4] = {rhs.upper().upper(), rhs.upper().lower(), rhs.lower().upper(), rhs.lower().lower()};
			uint128_t products[4][4];

			// multiply each component of the values
			for(int y = 3; y > -1; y--){
				for(int x = 3; x > -1; x--){
					products[3 - y][x] = top[x] * bottom[y];
				}
			}

			// first row
			uint128_t fourth64 = uint128_t(products[0][3].lower());
			uint128_t third64  = uint128_t(products[0][2].lower()) + uint128_t(products[0][3].upper());
			uint128_t second64 = uint128_t(products[0][1].lower()) + uint128_t(products[0][2].upper());
			uint128_t first64  = uint128_t(products[0][0].lower()) + uint128_t(products[0][1].upper());

			// second row
			third64  += uint128_t(products[1][3].lower());
			second64 += uint128_t(products[1][2].lower()) + uint128_t(products[1][3].upper());
			first64  += uint128_t(products[1][1].lower()) + uint128_t(products[1][2].upper());

			// third row
			second64 += uint128_t(products[2][3].lower());
			first64  += uint128_t(products[2][2].lower()) + uint128_t(products[2][3].upper());

			// fourth row
			first64  += uint128_t(products[3][3].lower());

			// combines the values, taking care of carry over
			return uint256_t(first64 << uint128_t(64), uint128_t(0)) +
				uint256_t(third64.upper(), third64 << uint128_t(64)) +
				uint256_t(second64, uint128_t(0)) +
				uint256_t(fourth64);
		}
        template <typename T>
        uint256_t operator*(const T & rhs) const{
            return *this * uint256_t(rhs);
        }
        uint256_t & operator*=(const uint128_t & rhs){
			return *this *= uint256_t(rhs);
		}

		uint256_t & operator*=(const uint256_t & rhs){
			*this = *this * rhs;
			return *this;
		}
        template <typename T>
        uint256_t & operator*=(const T & rhs){
            *this = *this * uint256_t(rhs);
            return *this;
        }

    public:
        uint256_t operator/(const uint128_t & rhs) const{
			return *this / uint256_t(rhs);
		}
		uint256_t operator/(const uint256_t & rhs) const{
			//return divmod(*this, rhs).first;
			uint256_t quotient(0) , remainder(0);
			divide(*this, rhs, quotient, remainder);
			return quotient;
		}
        template <typename T>
        uint256_t operator/(const T & rhs) const{
            return *this / uint256_t(rhs);
        }
        uint256_t & operator/=(const uint128_t & rhs){
			return *this /= uint256_t(rhs);
		}

		uint256_t & operator/=(const uint256_t & rhs){
			*this = *this / rhs;
			return *this;
		}
        template <typename T>
        uint256_t & operator/=(const T & rhs){
            *this = *this / uint256_t(rhs);
            return *this;
        }
        uint256_t operator%(const uint128_t & rhs) const{
			return *this % uint256_t(rhs);
		}

		uint256_t operator%(const uint256_t & rhs) const{
			return *this - (rhs * (*this / rhs));
		}
        template <typename T>
        uint256_t operator%(const T & rhs) const{
            return *this % uint256_t(rhs);
        }
        uint256_t & operator%=(const uint128_t & rhs){
			return *this %= uint256_t(rhs);
		}

		uint256_t & operator%=(const uint256_t & rhs){
			*this = *this % rhs;
			return *this;
		}
        template <typename T>
        uint256_t & operator%=(const T & rhs){
            *this = *this % uint256_t(rhs);
            return *this;
        }
        // Increment Operators
        uint256_t & operator++(){
			*this += uint256_t(1);
			return *this;
		}
		uint256_t operator++(int){
			uint256_t temp(*this);
			++*this;
			return temp;
		}
        // Decrement Operators
        uint256_t & operator--(){
			*this -= uint256_t(1);
			return *this;
		}
		uint256_t operator--(int){
			uint256_t temp(*this);
			--*this;
			return temp;
		}
        // Nothing done since promotion doesn't work here
        uint256_t operator+() const{
			return *this;
		}
        // two's complement
        uint256_t operator-() const{
			return ~*this + uint256_t(1);
		}
        // Get private values
        const uint128_t & upper() const {
			return UPPER;
		}
        const uint128_t & lower() const {
			return LOWER;
		}
        // Get bitsize of value
        uint16_t bits() const{
			uint16_t out = 0;
			if (UPPER){
				out = 128;
				uint128_t up = UPPER;
				while (up){
					up >>= uint128_t(1);
					out++;
				}
			}
			else{
				uint128_t low = LOWER;
				while (low){
					low >>= uint128_t(1);
					out++;
				}
			}
			return out;
		}
};

// Bitwise Operators
uint256_t operator&(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator&(const T & lhs, const uint256_t & rhs){
    return rhs & lhs;
}

uint128_t & operator&=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator&=(T & lhs, const uint256_t & rhs){
    return lhs = (T)(rhs & lhs);
}

uint256_t operator|(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator|(const T & lhs, const uint256_t & rhs){
    return rhs | lhs;
}


uint128_t & operator|=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator|=(T & lhs, const uint256_t & rhs){
    return lhs = (T)(rhs | lhs);
}

uint256_t operator^(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator^(const T & lhs, const uint256_t & rhs){
    return rhs ^ lhs;
}

uint128_t & operator^=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator^=(T & lhs, const uint256_t & rhs){
    return lhs = (T) (rhs ^ lhs);
}

// Bitshift operators
uint256_t operator<<(const bool      & lhs, const uint256_t & rhs);
uint256_t operator<<(const uint8_t   & lhs, const uint256_t & rhs);
uint256_t operator<<(const uint16_t  & lhs, const uint256_t & rhs);
uint256_t operator<<(const uint32_t  & lhs, const uint256_t & rhs);
uint256_t operator<<(const uint64_t  & lhs, const uint256_t & rhs);
uint256_t operator<<(const uint128_t & lhs, const uint256_t & rhs);
uint256_t operator<<(const int8_t    & lhs, const uint256_t & rhs);
uint256_t operator<<(const int16_t   & lhs, const uint256_t & rhs);
uint256_t operator<<(const int32_t   & lhs, const uint256_t & rhs);
uint256_t operator<<(const int64_t   & lhs, const uint256_t & rhs);

uint128_t & operator<<=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator<<=(T & lhs, const uint256_t & rhs){
    lhs = (T) (uint256_t(lhs) << rhs);
    return lhs;
}

uint256_t operator>>(const bool      & lhs, const uint256_t & rhs);
uint256_t operator>>(const uint8_t   & lhs, const uint256_t & rhs);
uint256_t operator>>(const uint16_t  & lhs, const uint256_t & rhs);
uint256_t operator>>(const uint32_t  & lhs, const uint256_t & rhs);
uint256_t operator>>(const uint64_t  & lhs, const uint256_t & rhs);
uint256_t operator>>(const uint128_t & lhs, const uint256_t & rhs);
uint256_t operator>>(const int8_t    & lhs, const uint256_t & rhs);
uint256_t operator>>(const int16_t   & lhs, const uint256_t & rhs);
uint256_t operator>>(const int32_t   & lhs, const uint256_t & rhs);
uint256_t operator>>(const int64_t   & lhs, const uint256_t & rhs);

uint128_t & operator>>=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator>>=(T & lhs, const uint256_t & rhs){
    return lhs = (T) (uint256_t(lhs) >> rhs);
}

// Comparison Operators
bool operator==(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
bool operator==(const T & lhs, const uint256_t & rhs){
    return (!rhs.upper() && ((uint64_t) lhs == rhs.lower()));
}

bool operator!=(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
bool operator!=(const T & lhs, const uint256_t & rhs){
    return (rhs.upper() | ((uint64_t) lhs != rhs.lower()));
}

bool operator>(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
bool operator>(const T & lhs, const uint256_t & rhs){
    return rhs.upper()?false:((uint128_t) lhs > rhs.lower());
}

bool operator<(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
bool operator<(const T & lhs, const uint256_t & rhs){
    return rhs.upper()?true:((uint128_t) lhs < rhs.lower());
}

bool operator>=(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
bool operator>=(const T & lhs, const uint256_t & rhs){
    return rhs.upper()?false:((uint128_t) lhs >= rhs.lower());
}

bool operator<=(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
bool operator<=(const T & lhs, const uint256_t & rhs){
    return rhs.upper()?true:((uint128_t) lhs <= rhs.lower());
}

// Arithmetic Operators
uint256_t operator+(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator+(const T & lhs, const uint256_t & rhs){
    return rhs + lhs;
}

uint128_t & operator+=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator+=(T & lhs, const uint256_t & rhs){
    lhs = (T) (rhs + lhs);
    return lhs;
}

uint256_t operator-(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator-(const T & lhs, const uint256_t & rhs){
    return -(rhs - lhs);
}

uint128_t & operator-=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator-=(T & lhs, const uint256_t & rhs){
    return lhs = (T) (-(rhs - lhs));
}

uint256_t operator*(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator*(const T & lhs, const uint256_t & rhs){
    return rhs * lhs;
}

uint128_t & operator*=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator*=(T & lhs, const uint256_t & rhs){
    return lhs = (T) (rhs * lhs);
}

uint256_t operator/(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator/(const T & lhs, const uint256_t & rhs){
    return uint256_t(lhs) / rhs;
}

uint128_t & operator/=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator/=(T & lhs, const uint256_t & rhs){
    return lhs = (T) (uint256_t(lhs) / rhs);
}

uint256_t operator%(const uint128_t & lhs, const uint256_t & rhs);

template <typename T>
uint256_t operator%(const T & lhs, const uint256_t & rhs){
    return uint256_t(lhs) % rhs;
}

uint128_t & operator%=(uint128_t & lhs, const uint256_t & rhs);

template <typename T>
T & operator%=(T & lhs, const uint256_t & rhs){
    return lhs = (T) (uint256_t(lhs) % rhs);
}

#endif
