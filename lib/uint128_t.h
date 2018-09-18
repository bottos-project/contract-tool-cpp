#ifndef UINT128_H_
#define UINT128_H_

#define MAX_LEN_128 39

#define UINT64_CRITICAL_VALUE  "18446744073709551615"
#define UINT128_CRITICAL_VALUE "340282366920938463463374607431768211455"
#define CHAR_BIT               8
#define SIZE128                ((sizeof(uint64_t) + sizeof(uint64_t)) * CHAR_BIT)


#include "string.hpp"

char  number[SIZE128 + 1] = {0};
class uint128_t;

template <typename T>
static void divide(const T &numerator, const T &denominator, T &quotient, T &remainder) {

	static const int bits = sizeof(T) * CHAR_BIT;
	if(denominator == 0) {
		return;
	} else {
		T n      = numerator;
		T d      = denominator;
		T x      = 1;
		T answer = 0;

		while((n >= d) && (((d >> (bits - 1)) & 1) == 0)) {
			x <<= 1;
			d <<= 1;
		}

		while(x != 0) {
			if(n >= d) {
				n -= d;
				answer |= x;
			}

			x >>= 1;
			d >>= 1;
		}

		quotient  = answer;
		remainder = n;
	}
}

class uint128_t{
    private:
        
		uint64_t UPPER , LOWER;
		static const unsigned int size128 = (sizeof(uint64_t) + sizeof(uint64_t)) * CHAR_BIT;

	private:

		void divmod(const uint128_t & lhs, const uint128_t & rhs, uint128_t & first, uint128_t & second) const{
			if (rhs == uint128_t(0)){
				return;
			} else if (rhs == uint128_t(1)){
				first  = lhs;
				second = uint128_t(0);
			} else if (lhs == rhs){
				first  = uint128_t(1);
				second = uint128_t(0);
			} else if ((lhs == uint128_t(0)) || (lhs < rhs)){
				first  = uint128_t(0);
				second = lhs;
			}

			first  = uint128_t(0);
			second = uint128_t(0);
			for(uint8_t x = lhs.bits(); x > 0; x--){
				first  <<= uint128_t(1);
				second <<= uint128_t(1);

				if ((lhs >> (x - 1U)) & 1){
					second++;
				}

				if (second >= rhs){
					second -= rhs;
					first++;
				}
			}
		}

	public:

		uint128_t():UPPER(0), LOWER(0){}
		uint128_t(const uint128_t & rhs):UPPER(rhs.UPPER), LOWER(rhs.LOWER){}
		uint128_t(int             value):LOWER(uint64_t(value)), UPPER(0) { if(value < 0) UPPER = uint128_t(-1); }
		uint128_t(unsigned int    value):LOWER(uint64_t(value)), UPPER(0) {}
		uint128_t(float           value):LOWER(uint64_t(value)), UPPER(0) {}
		uint128_t(double          value):LOWER(uint64_t(value)), UPPER(0) {}
		uint128_t(uint128_t        &rhs):UPPER(rhs.UPPER), LOWER(rhs.LOWER)
		{
			if (this != &rhs){
				rhs.UPPER = 0;
				rhs.LOWER = 0;
			}
		}
		uint128_t(uint64_t upper , uint64_t lower) {
			UPPER = upper;
			LOWER = lower;
		}
		uint128_t(uint64_t lower) {
			UPPER = 0;
			LOWER = lower;
		}
		uint128_t(const char *number)
		{
			UPPER = 0;
			LOWER = 0;
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
		int i = size128 - 1;
		number[sizeof(number) - 1] = '\0';

		uint128_t cur(*this);
		while (cur != 0 && i) {
			uint128_t remainder(0);
			divide(cur, uint128_t(radix), cur, remainder);
			number[--i] = "0123456789abcdefghijklmnopqrstuvwxyz"[remainder.to_int()];
			
		}

		return (char *)(number+i);
	}

	void set(uint64_t upper, uint64_t lower) {
		UPPER = upper;
		LOWER = lower;
	}
	
	template <typename T>
	uint128_t(const T & rhs):UPPER(0),LOWER(rhs){}

	template <typename S, typename T>
	uint128_t(const S & upper_rhs, const T & lower_rhs):UPPER(upper_rhs),LOWER(lower_rhs){}
	
	template <typename T>
	uint128_t & operator=(const T & rhs){
		UPPER = rhs >> 64;
		LOWER = rhs &  0xFFFFFFFFFFFFFFFFULL;
		//revover way: ((UPPER << 64) | 0xFFFFFFFFFFFFFFFFULL)
		return *this;
	}
	
	template <typename T>
	uint128_t operator&(const T & rhs) const{
		return uint128_t(0, LOWER & (uint64_t) rhs);
    }
	
	template <typename T>
	uint128_t & operator&=(const T & rhs){
		UPPER = 0;
		LOWER &= rhs;
		return *this;
	}
	
	template <typename T>
	uint128_t operator|(const T & rhs) const{
		return uint128_t(UPPER, LOWER | (uint64_t) rhs);
	}
	
	template <typename T>
	uint128_t & operator|=(const T & rhs){
		LOWER |= (uint64_t) rhs;
		return *this;
	}
	
	template <typename T>
	uint128_t operator^(const T & rhs) const{
		return uint128_t(UPPER, LOWER ^ (uint64_t) rhs);
	}
	
	template <typename T>
	uint128_t & operator^=(const T & rhs){
		LOWER ^= (uint64_t) rhs;
		return *this;
	}
	
	template <typename T>
	uint128_t operator<<(const T & rhs) const{
		return *this << uint128_t(rhs);
	}
	
	template <typename T>
    uint128_t & operator<<=(const T & rhs){
		*this = *this << uint128_t(rhs);
		return *this;
	}
	
	template <typename T>
	uint128_t operator>>(const T & rhs) const{
		return *this >> uint128_t(rhs);
	}
	
	template <typename T>
	uint128_t & operator>>=(const T & rhs){
		*this = *this >> uint128_t(rhs);
		return *this;
	}
	
	template <typename T>
	bool operator&&(const T & rhs){
		return static_cast <bool> (*this && rhs);
	}
	
	template <typename T>
	bool operator||(const T & rhs){
		return static_cast <bool> (*this || rhs);
	}
	
	template <typename T>
	bool operator==(const T & rhs) const{
		return (!UPPER && (LOWER == (uint64_t) rhs));
	}
	
	template <typename T>
	bool operator!=(const T & rhs) const{
		return (UPPER | (LOWER != (uint64_t) rhs));
	}
	
	template <typename T>
	bool operator>(const T & rhs) const{
		return (UPPER || (LOWER > (uint64_t) rhs));
	}
	
	template <typename T>
	bool operator<(const T & rhs) const{
		return (!UPPER)?(LOWER < (uint64_t) rhs):false;
	}
	
	template <typename T>
	bool operator>=(const T & rhs) const{
		return ((*this > rhs) | (*this == rhs));
	}
	
	template <typename T>
	bool operator<=(const T & rhs) const{
		return ((*this < rhs) | (*this == rhs));
	}
	
	template <typename T>
	uint128_t operator+(const T & rhs) const{
		return uint128_t(UPPER + ((LOWER + (uint64_t) rhs) < LOWER), LOWER + (uint64_t) rhs);
	}
	
	template <typename T>
	uint128_t & operator+=(const T & rhs){
		UPPER = UPPER + ((LOWER + rhs) < LOWER);
		LOWER = LOWER + rhs;
		return *this;
	}
	
	template <typename T>
	uint128_t operator-(const T & rhs) const{
		return uint128_t((uint64_t) (UPPER - ((LOWER - rhs) > LOWER)), (uint64_t) (LOWER - rhs));
	}
	
	template <typename T>
	uint128_t & operator-=(const T & rhs){
		*this = *this - rhs;
		return *this;
	}
	
	template <typename T>
	uint128_t operator*(const T & rhs) const{
		return *this * uint128_t(rhs);
	}
	
	template <typename T>
	uint128_t & operator*=(const T & rhs){
		*this = *this * uint128_t(rhs);
		return *this;
	}
	
	template <typename T>
	uint128_t operator/(const T & rhs) const{
		return *this / uint128_t(rhs);
	}

	template <typename T>
	uint128_t & operator/=(const T & rhs){
		*this = *this / uint128_t(rhs);
		return *this;
	}
	
	template <typename T>
	uint128_t operator%(const T & rhs) const{
		return *this % uint128_t(rhs);
	}
	
	template <typename T>
	uint128_t & operator%=(const T & rhs){
		*this = *this % uint128_t(rhs);
		return *this;
	}

    uint128_t & operator=(const uint128_t & rhs){
		UPPER = rhs.UPPER;
		LOWER = rhs.LOWER;
		return *this;
	}
	// Typecast Operators
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
    // Bitwise Operators
    uint128_t operator&(const uint128_t & rhs) const{
		return uint128_t(UPPER & rhs.UPPER, LOWER & rhs.LOWER);
	}
    uint128_t & operator&=(const uint128_t & rhs){
		UPPER &= rhs.UPPER;
		LOWER &= rhs.LOWER;
		return *this;
	}
	uint128_t operator|(const uint128_t & rhs) const{
		return uint128_t(UPPER | rhs.UPPER, LOWER | rhs.LOWER);
	}
	uint128_t & operator|=(const uint128_t & rhs){
		UPPER |= rhs.UPPER;
		LOWER |= rhs.LOWER;
		return *this;
	}
	uint128_t operator^(const uint128_t & rhs) const{
		return uint128_t(UPPER ^ rhs.UPPER, LOWER ^ rhs.LOWER);
	}
	uint128_t & operator^=(const uint128_t & rhs){
		UPPER ^= rhs.UPPER;
		LOWER ^= rhs.LOWER;
		return *this;
	}
	uint128_t operator~() const{
		return uint128_t(~UPPER, ~LOWER);
	}

	// Bit Shift Operators
	uint128_t operator<<(const uint128_t & rhs) const{
		const uint64_t shift = rhs.LOWER;
		if (((bool) rhs.UPPER) || (shift >= 128)){
			return uint128_t(0);
		}
		else if (shift == 64){
			return uint128_t(LOWER, 0);
		}
		else if (shift == 0){
			return *this;
		}
		else if (shift < 64){
			return uint128_t((UPPER << shift) + (LOWER >> (64 - shift)), LOWER << shift);
		}
		else if ((128 > shift) && (shift > 64)){
			return uint128_t(LOWER << (shift - 64), 0);
		}
		else{
			return uint128_t(0);
		}
	}
	uint128_t & operator<<=(const uint128_t & rhs){
		*this = *this << rhs;
		return *this;
	}
	uint128_t operator>>(const uint128_t & rhs) const{
		const uint64_t shift = rhs.LOWER;
		if (((bool) rhs.UPPER) || (shift >= 128)){
			return uint128_t(0);
		}
		else if (shift == 64){
			return uint128_t(0, UPPER);
		}
		else if (shift == 0){
			return *this;
		}
		else if (shift < 64){
			return uint128_t(UPPER >> shift, (UPPER << (64 - shift)) + (LOWER >> shift));
		}
		else if ((128 > shift) && (shift > 64)){
			return uint128_t(0, (UPPER >> (shift - 64)));
		}
		else{
			return uint128_t(0);
		}
	}
	uint128_t operator>>(int & rhs) const{
            return *this >> uint128_t(rhs);
    }
	uint128_t & operator>>=(const uint128_t & rhs){
		*this = *this >> rhs;
		return *this;
	}


        // Logical Operators
	bool operator!() const{
		return !(bool) (UPPER | LOWER);
	}
        bool operator&&(const uint128_t & rhs) const{
		return ((bool) *this && rhs);
	}
	bool operator||(const uint128_t & rhs) const{
		return ((bool) *this || rhs);
	}

        // Comparison Operators
	bool operator==(const uint128_t & rhs) const{
		return ((UPPER == rhs.UPPER) && (LOWER == rhs.LOWER));
	}
	bool operator==(const uint32_t & rhs) const{
		if (UPPER == 0) {
			return (LOWER == rhs);
		} else {
			return false;
		}
	}
	bool operator==(const int & rhs) const{
        if (UPPER == 0) {
			return (LOWER == rhs);
		} else {
			return false;
		}
    }
	/*
	bool operator==(const long long int & rhs) const{
		return ((UPPER == rhs.UPPER) && (LOWER == rhs.LOWER));
	}
	*/
    bool operator!=(const uint128_t & rhs) const{
		return ((UPPER != rhs.UPPER) | (LOWER != rhs.LOWER));
	}
    bool operator>(const uint128_t & rhs) const{
		if (UPPER == rhs.UPPER){
			return (LOWER > rhs.LOWER);
		}
		return (UPPER > rhs.UPPER);
	}
    bool operator<(const uint128_t & rhs) const{
		if (UPPER == rhs.UPPER){
			return (LOWER < rhs.LOWER);
		}
		return (UPPER < rhs.UPPER);
	}
	bool operator>=(const uint128_t & rhs) const{
		return ((*this > rhs) | (*this == rhs));
	}
        bool operator<=(const uint128_t & rhs) const{
		return ((*this < rhs) | (*this == rhs));
	}

	// Arithmetic Operators
    uint128_t operator+(const uint128_t & rhs) const{
		return uint128_t(UPPER + rhs.UPPER + ((LOWER + rhs.LOWER) < LOWER), LOWER + rhs.LOWER);
	}
	uint128_t operator+(bool rhs) const{
		return uint128_t(UPPER , LOWER + (rhs?1:0));
	}
	uint128_t & operator+=(const uint128_t & rhs){
		this->UPPER += rhs.UPPER + ((LOWER + rhs.LOWER) < LOWER);
		this->LOWER += rhs.LOWER;
		return *this;
	}
	uint128_t operator-(const uint128_t & rhs) const{
		return uint128_t(UPPER - rhs.UPPER - ((LOWER - rhs.LOWER) > LOWER), LOWER - rhs.LOWER);
	}
	uint128_t operator-(bool rhs) const{
		return uint128_t(UPPER , LOWER - (rhs?1:0));
	}
	uint128_t & operator-=(const uint128_t & rhs){
		*this = *this - rhs;
		return *this;
	}
	uint128_t operator*(const uint128_t & rhs) const{
		// split values into 4 32-bit parts
		uint64_t top[4] = {UPPER >> 32, UPPER & 0xffffffff, LOWER >> 32, LOWER & 0xffffffff};
		uint64_t bottom[4] = {rhs.UPPER >> 32, rhs.UPPER & 0xffffffff, rhs.LOWER >> 32, rhs.LOWER & 0xffffffff};
		uint64_t products[4][4];

		// multiply each component of the values
		for(int y = 3; y > -1; y--){
			for(int x = 3; x > -1; x--){
				products[3 - x][y] = top[x] * bottom[y];
			}
		}

		// first row
		uint64_t fourth32 = (products[0][3] & 0xffffffff);
		uint64_t third32  = (products[0][2] & 0xffffffff) + (products[0][3] >> 32);
		uint64_t second32 = (products[0][1] & 0xffffffff) + (products[0][2] >> 32);
		uint64_t first32  = (products[0][0] & 0xffffffff) + (products[0][1] >> 32);

		// second row
		third32  += (products[1][3] & 0xffffffff);
		second32 += (products[1][2] & 0xffffffff) + (products[1][3] >> 32);
		first32  += (products[1][1] & 0xffffffff) + (products[1][2] >> 32);

		//third row
		second32 += (products[2][3] & 0xffffffff);
		first32  += (products[2][2] & 0xffffffff) + (products[2][3] >> 32);

		// fourth row
		first32  += (products[3][3] & 0xffffffff);

		// move carry to next digit
		third32  += fourth32 >> 32;
		second32 += third32  >> 32;
		first32  += second32 >> 32;

		// remove carry from current digit
		fourth32 &= 0xffffffff;
		third32  &= 0xffffffff;
		second32 &= 0xffffffff;
		first32  &= 0xffffffff;

		// combine components
		return uint128_t((first32 << 32) | second32, (third32 << 32) | fourth32);
	}
		uint128_t & operator*=(const uint128_t & rhs){
			*this = *this * rhs;
			return *this;
		}

    public:

		uint128_t operator/(const uint128_t & rhs) const{
			uint128_t quotient(0) , remainder(0);
			divide(*this, rhs, quotient, remainder);
			return quotient;
		}
		
		uint128_t & operator/=(const uint128_t & rhs){
			*this = *this / rhs;
			return *this;
		}
		
		uint128_t operator%(const uint128_t & rhs) const{
			uint128_t quotient(0) , remainder(0);
			divide(*this, rhs, quotient, remainder);
			return remainder;
		}
		
		uint128_t & operator%=(const uint128_t & rhs){
			*this = *this % rhs;
			return *this;
		}

	// Increment Operator
	uint128_t & operator++(){
		return *this += uint128_t(1);
	}
	uint128_t operator++(int){
		uint128_t temp(*this);
		++*this;
		return temp;
	}

        // Decrement Operator
    uint128_t & operator--(){
		return *this -= uint128_t(1);
	}

	uint128_t operator--(int){
		uint128_t temp(*this);
		--*this;
		return temp;
	}

	// Nothing done since promotion doesn't work here
	uint128_t operator+() const{
		return *this;
	}

	// two's complement
	uint128_t operator-() const{
		return ~*this + uint128_t(1);
	}

	// Get private values
	const uint64_t & upper() const{
		return UPPER;
	}
	const uint64_t & lower() const{
		return LOWER;
	}
	int to_int() const {
		return (int)LOWER;
	}
	// Get bitsize of value
	uint8_t bits() const{
		uint8_t out = 0;
		if (UPPER){
			out = 64;
			uint64_t up = UPPER;
			while (up){
				up >>= 1;
				out++;
			}
		} else {
			uint64_t low = LOWER;
			while (low){
				low >>= 1;
				out++;
			}
		}
		return out;
	}

	// Get string representation of value
        //std::string str(uint8_t base = 10, const unsigned int & len = 0) const;
		/*
		uint128_t operator<<(const bool & lhs, const uint128_t & rhs){
			return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const uint8_t & lhs, const uint128_t & rhs){
			return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const uint16_t & lhs, const uint128_t & rhs){
			return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const uint32_t & lhs, const uint128_t & rhs){
			return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const uint64_t & lhs, const uint128_t & rhs){
			return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const int8_t & lhs, const uint128_t & rhs){
		    return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const int16_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const int32_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) << rhs;
		}

		uint128_t operator<<(const int64_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) << rhs;
		}

		uint128_t operator>>(const bool & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const uint8_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const uint16_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const uint32_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const uint64_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const int8_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const int16_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const int32_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}

		uint128_t operator>>(const int64_t & lhs, const uint128_t & rhs){
    		return uint128_t(lhs) >> rhs;
		}
		*/
};

/*
template <typename T>
uint128_t operator=(uint128_t & lhs , const T & rhs){
		UPPER = rhs >> 64;
		LOWER = rhs &  0xFFFFFFFFFFFFFFFFULL;
		return uint128_t(UPPER,LOWER);
}
*/

// Bitwise Operators
template <typename T>
uint128_t operator&(const T & lhs, const uint128_t & rhs){
    return rhs & lhs;
}

template <typename T>
T & operator&=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs & lhs);
}

template <typename T>
uint128_t operator|(const T & lhs, const uint128_t & rhs){
    return rhs | lhs;
}

template <typename T>
T & operator|=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs | lhs);
}

template <typename T>
uint128_t operator^(const T & lhs, const uint128_t & rhs){
    return rhs ^ lhs;
}

template <typename T>
T & operator^=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs ^ lhs);
}

template <typename T>
T & operator<<=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) << rhs);
}

template <typename T>
T & operator>>=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) >> rhs);
}

// Comparison Operators
template <typename T>
bool operator==(const T & lhs, const uint128_t & rhs){
    return (!rhs.upper() && ((uint64_t) lhs == rhs.lower()));
}

template <typename T>
bool operator!=(const T & lhs, const uint128_t & rhs){
    return (rhs.upper() | ((uint64_t) lhs != rhs.lower()));
}

template <typename T>
bool operator>(const T & lhs, const uint128_t & rhs){
    return (!rhs.upper()) && ((uint64_t) lhs > rhs.lower());
}

template <typename T>
bool operator<(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
        return true;
    }
    return ((uint64_t) lhs < rhs.lower());
}

template <typename T>
bool operator>=(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
        return false;
    }
    return ((uint64_t) lhs >= rhs.lower());
}

template <typename T>
bool operator<=(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
        return true;
    }
    return ((uint64_t) lhs <= rhs.lower());
}

// Arithmetic Operators
template <typename T>
uint128_t operator+(const T & lhs, const uint128_t & rhs){
    return rhs + lhs;
}

/*
template <typename T>
T & operator+=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs + lhs);
}
*/

template <typename T>
uint128_t operator-(const T & lhs, const uint128_t & rhs){
    return -(rhs - lhs);
}

/*
template <typename T>
T & operator-=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (-(rhs - lhs));
}
*/


template <typename T>
uint128_t operator*(const T & lhs, const uint128_t & rhs){
    return rhs * lhs;
}

/*
template <typename T>
T & operator*=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs * lhs);
}
*/

/*
template <typename T>
uint128_t operator/(const T & lhs, const uint128_t & rhs){
    return uint128_t(lhs) / rhs;
}
*/

/*
template <typename T>
T & operator/=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) / rhs);
}
*/

/*
template <typename T>
uint128_t operator%(const T & lhs, const uint128_t & rhs){
    return uint128_t(lhs) % rhs;
}


template <typename T>
T & operator%=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) % rhs);
}
*/

#endif



























