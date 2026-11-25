#include <cctype>
#include <climits>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>

bool isValidNum(const std::string& str ){
    if(str.empty()) return false;
    size_t start = (str[0] == '+' || str[0] == '-') ? 1 : 0;
    
    for(size_t i = start; i < str.length(); ++i){
      if (!std::isdigit(static_cast<unsigned char>(str[i]))) return false;
    }

    return start == 0 || start != str.length();
}

class CBigInt {
    private:

    void convertToBASE(std::string& num); // Converts string to base 2^32
    static const uint32_t BASE = UINT32_MAX; // Base 2^32 = (BASE + 1) 
    std::vector<uint32_t> digits; // VERY big num = Σ(arr[i] * 2^32*i)i=0 ^(arr.size() - 1) 
    bool positive;

    public:
    
    // implicit constructor, value 0
    CBigInt(): positive(true)  { digits.push_back(0); }
    // int constructor
    CBigInt(int num): positive(num >= 0) {
        if (num == INT_MIN) 
            digits.push_back(static_cast<uint32_t>(INT_MAX) + 1);
        else
            digits.push_back(static_cast<uint32_t>(positive ? num : -num));
    }
    // string constructor
    CBigInt(const std::string& num) : positive(true) {
        if(!isValidNum(num)) throw std::invalid_argument("Invalid number representation!");
        if( num[0] == '-' ) positive = false;
        std::string temp = num;
        if(!std::isdigit(static_cast<unsigned char>(num[0])))
            temp.erase(0,1);  // ignore +/- for calculation
        
        temp.erase(0, temp.find_first_not_of('0')); // remove leading 0's
        if(temp.empty()){
            temp = "0";
            positive = true;  // -000
        } 

        // Number can be inserted in one position 
        if (temp.length() < 10)
            digits.push_back(static_cast<uint32_t>(std::stoul(temp)));
        else if (temp.length() == 10) {
            const auto value = std::stoull(temp);

            if (value <= BASE) digits.push_back(static_cast<uint32_t>(value));
            else convertToBASE(temp); // => Populates vector digits
        } else convertToBASE(temp);
        
    }
    
    // Assignment from int, string or CBigInt
    CBigInt& operator = (const CBigInt & other) {
        if (this != &other) { 
            positive = other.positive;
            digits = other.digits;
        }
        return *this;
    }
    CBigInt& operator = (const std::string & other) { 
        CBigInt o(other);
        digits = o.digits;
        positive = o.positive;
        return *this;
    }
    CBigInt& operator = (const int other) {
        digits.clear();
        positive = other >= 0;
        if (other == INT_MIN) 
            digits.push_back(static_cast<uint32_t>(INT_MAX) + 1);
        else
            digits.push_back(static_cast<uint32_t>(positive ? other : -other));
        return *this;
    }

    // operator +, any combination {CBigInt/int/string} + {CBigInt/int/string}
    CBigInt operator + (const CBigInt& other) const;
    CBigInt operator + (const int other) const{
        return (*this) + CBigInt(other); 
    }
    CBigInt operator + (const std::string& other) const{
        return (*this) + CBigInt(other);
    }
    friend CBigInt operator + (const int num, const CBigInt& bigNum ){
        return CBigInt(num) + bigNum;
    }
    friend CBigInt operator + (const std::string& num, const CBigInt& bigNum ){
        return CBigInt(num) + bigNum;
    }

    // operator *, any combination {CBigInt/int/string} * {CBigInt/int/string}
    CBigInt operator * (const CBigInt& other) const;
    CBigInt operator * (const int other) const{
        return (*this) * CBigInt(other);
    }
    CBigInt operator * (const std::string& other) const{
        return (*this) * CBigInt(other);
    }
    friend CBigInt operator * (const int num, const CBigInt& bigNum){
        return CBigInt(num) * bigNum;
    }
    friend CBigInt operator * (const std::string& num, const CBigInt& bigNum){
        return CBigInt(num) * bigNum;
    }
    // operator +=, any of {CBigInt/int/string}
    CBigInt& operator += (const CBigInt& other){
        *this = *this + other;
        return *this;
    }
    CBigInt& operator += (const int other){
        *this = *this + CBigInt(other);
        return *this;
    }
    CBigInt& operator += (const std::string& other){
        *this = *this + CBigInt(other);
        return *this;
    }
    // operator *=, any of {CBigInt/int/string}
    CBigInt& operator *= (const CBigInt& other){
        *this = *this * other;
        return *this;
    }
    CBigInt& operator *= (const int other){
        *this = *this * CBigInt(other);
        return *this;
    }
    CBigInt& operator *= (const std::string& other){
        *this = *this * CBigInt(other);
        return *this;
    }
    // Comparison operators, any combination {CBigInt/int/string} {<,<=,>,>=,==,!=} {CBigInt/int/string}
    bool operator == (const CBigInt & other) const;
    bool operator == (const int other) const{
        return *this == CBigInt(other);
    }
    bool operator == (const std::string& other) const{
        return *this == CBigInt(other);
    }
    friend bool operator == (const std::string& num, const CBigInt& bigNum) {
        return CBigInt(num) == bigNum;
    }
    friend bool operator == (const int num, const CBigInt& bigNum) {
        return CBigInt(num) == bigNum;
    }

    bool operator != (const CBigInt & other) const{
        return !(*this == other);
    }
    bool operator != (const int other) const{
        return !(*this == CBigInt(other));
    }
    bool operator != (const std::string& other) const{
        return !(*this == CBigInt(other));
    }
    friend bool operator != (const std::string& num, const CBigInt& bigNum) {
        return !(CBigInt(num) == bigNum);
    }
    friend bool operator != (const int num, const CBigInt& bigNum) {
        return !(CBigInt(num) == bigNum);
    }

    bool operator >= (const CBigInt & other) const{
        return !( *this < other );
    };
    bool operator >= (const int other) const{
        return !( *this < CBigInt(other) );
    }
    bool operator >= (const std::string& other) const{
        return !( *this < CBigInt(other) );
    }
    friend bool operator >= (const std::string& num, const CBigInt& bigNum) {
        return !( CBigInt(num) < bigNum );
    }
    friend bool operator >= (const int num, const CBigInt& bigNum) {
        return !( CBigInt(num) < bigNum );
    }

    bool operator <= (const CBigInt & other) const{
        return !( *this > other );
    };
    bool operator <= (const int other) const{
        return !( *this > CBigInt(other) );
    }
    bool operator <= (const std::string& other) const{
        return !( *this > CBigInt(other) );
    }
    friend bool operator <= (const std::string& num, const CBigInt& bigNum) {
        return !( CBigInt(num) > bigNum );
    }
    friend bool operator <= (const int num, const CBigInt& bigNum) {
        return !( CBigInt(num) > bigNum );
    }

    bool operator < (const CBigInt & other) const;
    bool operator < (const int other) const{
        return *this < CBigInt(other);
    }
    bool operator < (const std::string& other) const{
        return *this < CBigInt(other);
    }
    friend bool operator < (const std::string& num, const CBigInt& bigNum) {
        return CBigInt(num) < bigNum;
    }
    friend bool operator < (const int num, const CBigInt& bigNum) {
        return CBigInt(num) < bigNum;
    }
    bool operator > (const CBigInt & other) const;
    bool operator > (const int other) const{
        return *this > CBigInt(other);
    }
    bool operator > (const std::string& other) const{
        return *this > CBigInt(other);
    }
    friend bool operator > (const std::string& num, const CBigInt & bigNum) {
        return CBigInt(num) > bigNum;
    }
    friend bool operator > (const int num, const CBigInt & bigNum) {
        return CBigInt(num) > bigNum;
    }
    uint32_t divideBy10() {
        uint64_t remainder = 0;
        std::vector<uint32_t> result;

        for (size_t i = digits.size(); i-- > 0;) {
            // Combine the curr digit with the remainder from last operation
            uint64_t temp = (remainder << 32) | digits[i]; 
            uint32_t quotientDigit = temp / 10;
            remainder = temp % 10;

            // Prepend the quotient digit to the result (Little-endian)
            if (!result.empty() || quotientDigit > 0)  
                result.push_back(quotientDigit);
        }
        std::reverse(result.begin(), result.end());
        digits = result.empty() ? std::vector<uint32_t>{0} : result;
        return static_cast<uint32_t>(remainder);
    }
    // output operator <<
    friend std::ostream & operator << (std::ostream & os, const CBigInt & bigNum) {
        if (bigNum.digits.size() == 1 && bigNum.digits[0] == 0) {
            os << "0";
            return os;
        }
        if (!bigNum.positive) 
            os << "-";
    
        bool convertHex = (os.flags() & std::ios_base::hex) != 0;
        std::ostringstream ss;
        if(convertHex){ 
            // Convert into hexadecimal
            ss << std::hex << bigNum.digits[bigNum.digits.size() - 1];
            if (bigNum.digits.size() > 1) {
                for(size_t i = bigNum.digits.size() - 2; ; --i){
                    ss << std::setfill('0') << std::setw(8) << std::hex << bigNum.digits[i];
                    if(i == 0) break;
                }
            }
        }else {
            // Convert into decimal
            std::string decimal;
            CBigInt tmp = bigNum;
            while (!(tmp.digits.size() == 1 && tmp.digits[0] == 0)) {
                uint32_t remainder = tmp.divideBy10();
                decimal.push_back('0' + remainder);
            }
            std::reverse(decimal.begin(), decimal.end());
            ss << decimal;
            
        }
        return os << ss.str();
    } 
    
    // input operator >>
    friend std::istream & operator >> (std::istream & is, CBigInt & bigNum) {
        std::string num;
        is >> std::ws; // skip whitespaces
        char c = is.peek();
        // Check first character
        if (c == '+' || c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
            is >> c; 
            num += c; 
        } else { // Incorrect input
            is.setstate(std::ios::failbit);
            return is;
        }
        // Keep reading
        while (is.peek() != std::istream::traits_type::eof() &&
                std::isdigit(static_cast<unsigned char>(is.peek()))) {
            is >> c; 
            num += c;
        }
        // Only read sign = Incorrect input
        if (num.size() == 1 && (num[0] == '+' || num[0] == '-')) {
            is.setstate(std::ios::failbit);
            return is;
        }
        // Correct input
        CBigInt newBigNum(num);
        bigNum = newBigNum;    
        return is; 
    } 
};

// Converts decimal number to BASE
// Little-endian
void CBigInt::convertToBASE(std::string& num){
    uint64_t divisor = static_cast<uint64_t>(BASE) + 1;
    std::string quotient = num;

    while (!quotient.empty() && quotient != "0") {
        std::string newQuotient;
        uint64_t remainder = 0;
        for (char c : quotient) {
            int digit = c - '0';
            uint64_t temp = remainder * 10 + digit;
            remainder = static_cast<uint32_t>(temp % divisor);
            uint64_t qDigit = temp / divisor;
            if (!(newQuotient.empty() && qDigit == 0)) { // Skip leading zeros
                newQuotient.push_back('0' + qDigit);
            }
        }
        digits.push_back(remainder); // Store the remainder
        quotient = newQuotient;
    }
}

CBigInt CBigInt::operator + (const CBigInt& other) const{
    CBigInt result;
    if ( positive == other.positive ) {
        result.positive = positive;
        // Summation
        uint32_t carry = 0;
        size_t maxDigits = std::max(digits.size(), other.digits.size());
        result.digits.resize(maxDigits + 1, 0);
        for (size_t i = 0; i < maxDigits || carry; ++i) {
            uint64_t sum = carry;
            if (i < digits.size()) 
                sum += digits[i];

            if(i < other.digits.size())
                sum += other.digits[i];
            
            result.digits[i] = static_cast<uint32_t>(sum);
            carry = sum >> 32; // Extract carry
        }
        while (result.digits.size() > 1 && result.digits.back() == 0)
            result.digits.pop_back();
    }else{
        // Find negative
        CBigInt abs = other;
        bool iamNegative = false;
        if(!positive){
            abs = *this;
            iamNegative = true;
        }
        abs.positive = true;    
        result.positive = iamNegative ? abs == other : abs == *this;
        // Find which abs number is bigger and set result sign
        CBigInt bigger;
        CBigInt smaller; // Compare absolute values
        if(abs > (iamNegative ? other : *this)){
            bigger = iamNegative ? *this : other;
            smaller = iamNegative ? other : *this; 
            if(!result.positive)
                result.positive = iamNegative ? positive : other.positive;
        }else{
            bigger = iamNegative ? other : *this;
            smaller = iamNegative ? *this : other;
            if(!result.positive)
                result.positive = iamNegative ? other.positive : positive;
        }
        // Subtraction
        result.digits.resize(bigger.digits.size(), 0);
        uint32_t borrow = 0;
        for(size_t i = 0; i < bigger.digits.size(); ++i){ // EDGE CASE: smaller.digits[i] = 2^32 - 1 + borrow == 2^32 OVERFLOW
            uint64_t sub = i < smaller.digits.size() ? static_cast<uint64_t>(smaller.digits[i]) + borrow : borrow;
            if(bigger.digits[i] < sub){
                uint64_t num = (static_cast<uint64_t>(BASE) + 1) + bigger.digits[i];
                result.digits[i] = static_cast<uint32_t>(num - sub);
                borrow = 1;
            }else{
                result.digits[i] = static_cast<uint32_t>(bigger.digits[i] - sub);
                borrow = 0;
            }
        }
        // Remove any leading zeros from the result
        while (result.digits.size() > 1 && result.digits.back() == 0)
            result.digits.pop_back();
    }
    return result;
}

CBigInt CBigInt::operator * (const CBigInt& other) const{
    CBigInt result;
    result.positive = positive == other.positive; // Multiplication by 0
    if((other.digits.size() == 1 && other.digits[0] == 0 ) || ( digits.size() == 1 && digits[0] == 0)){
        result.positive = true;
        result.digits.push_back(0);
        return result;
    }
    result.digits.resize(digits.size() + other.digits.size(), 0);
    // Multiply each number from *this bigNum with each from other bigNum 
    for (size_t i = 0; i < digits.size(); ++i) {
        uint64_t carry = 0; 
        for (size_t j = 0; j < other.digits.size() || carry != 0; ++j) {
            uint64_t mult = result.digits[i + j] + carry;
            if (j < other.digits.size()) 
                mult += static_cast<uint64_t>(digits[i]) * other.digits[j];
            
            result.digits[i + j] = static_cast<uint32_t>(mult);
            carry = mult >> 32;
        }
    }
    // Remove leading zeros
    while (result.digits.size() > 1 && result.digits.back() == 0) 
        result.digits.pop_back();

    return result;
}

bool CBigInt::operator == (const CBigInt & other) const{
    if(positive != other.positive || digits.size() != other.digits.size())  
        return false;
    for(size_t i = 0; i < digits.size(); i++){
        size_t j = digits.size() - 1 - i;
        if( digits[i] != other.digits[i] ) // check left pairs
            return false;
        if ( i >= j ) break;
        if( digits[j] != other.digits[j] ) // check right pairs O(n/2)
            return false;
    }
    return true;
}
bool CBigInt::operator < (const CBigInt & other) const{
    if(positive != other.positive) 
        return positive ? false : true;
    if(digits.size() != other.digits.size())
        return positive ? digits.size() < other.digits.size() : digits.size() > other.digits.size();

    for(size_t i = digits.size(); i-- > 0 ;){
        if( digits[i] != other.digits[i] ) 
            return positive ? digits[i] < other.digits[i] : digits[i] > other.digits[i];
    }
    return false;
}
bool CBigInt::operator > (const CBigInt & other) const{
    if(positive != other.positive) 
        return positive ? true : false;
    if(digits.size() != other.digits.size())
        return positive ? digits.size() > other.digits.size() : digits.size() < other.digits.size();

    for(size_t i = digits.size(); i-- > 0 ;){
        if( digits[i] != other.digits[i] ) 
            return positive ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// TESTING ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


int main()
{
    int failures = 0;

    CBigInt zero;
    CBigInt one(1);
    CBigInt negative(-25);
    CBigInt fromString("-123456789012345678901234567890");
    CBigInt copied = fromString;

    std::cout << "Constructors:\n";
    std::cout << "  zero = " << zero << "\n";
    std::cout << "  negative = " << negative << "\n";
    std::cout << "  string = " << fromString << "\n";

    if (zero != 0) {
        std::cerr << "FAIL: default constructor\n";
        ++failures;
    }
    if (negative != -25) {
        std::cerr << "FAIL: integer constructor\n";
        ++failures;
    }
    if (copied != fromString) {
        std::cerr << "FAIL: copy constructor / assignment\n";
        ++failures;
    }

    const auto checkValue = [&failures](const std::string& label, const CBigInt& value, const std::string& expected) {
        std::ostringstream actual;
        actual << value;
        if (actual.str() != expected) {
            std::cerr << "FAIL: " << label << " (expected " << expected
                      << ", got " << actual.str() << ")\n";
            ++failures;
        }
    };
    const auto checkCondition = [&failures](const std::string& label, bool condition) {
        if (!condition) {
            std::cerr << "FAIL: " << label << "\n";
            ++failures;
        }
    };

    std::cout << "\nConstructor edge cases:\n";
    const struct {
        const char* input;
        const char* expected;
    } constructorCases[] = {
        {"0", "0"},
        {"-0", "0"},
        {"+0000000000", "0"},
        {"0000000001", "1"},
        {"-0000000001", "-1"},
        {"999999999", "999999999"},
        {"1000000000", "1000000000"},
        {"-1234567890", "-1234567890"},
        {"4294967295", "4294967295"},
        {"-4294967295", "-4294967295"},
        {"4294967296", "4294967296"},
        {"-4294967296", "-4294967296"},
        {"9999999999", "9999999999"},
        {"18446744073709551616", "18446744073709551616"},
        {"-18446744073709551616", "-18446744073709551616"},
        {"+000000000018446744073709551616", "18446744073709551616"}
    };
    for (const auto& test : constructorCases)
        checkValue(std::string("construct ") + test.input, CBigInt(test.input), test.expected);
    checkValue("construct INT_MIN", CBigInt(INT_MIN), std::to_string(INT_MIN));

    CBigInt sum = CBigInt("999999999999999999") + CBigInt("1");
    CBigInt difference = CBigInt("100") + CBigInt("-125");
    CBigInt product = CBigInt("123456789") * CBigInt("987654321");
    CBigInt compound = CBigInt("10");
    compound += 5;
    compound *= CBigInt("3");

    std::cout << "\nArithmetic:\n";
    std::cout << "  sum = " << sum << "\n";
    std::cout << "  sum hex = " << std::hex << sum << "\n";
    CBigInt sumExpected("1000000000000000000");
    std::cout << "  sum expected hex = " << std::hex << sumExpected << "\n";
    std::cout << "  difference = " << difference << "\n";
    std::cout << "  product = " << product << "\n";
    std::cout << "  compound = " << compound << "\n";

    if (sum != "1000000000000000000") {
        std::cerr << "FAIL: addition\n";
        ++failures;
    }
    if (difference != "-25") {
        std::cerr << "FAIL: mixed-sign subtraction\n";
        ++failures;
    }
    if (product != "121932631112635269") {
        std::cerr << "FAIL: multiplication\n";
        ++failures;
    }
    if (compound != 45) {
        std::cerr << "FAIL: += and *=\n";
        ++failures;
    }

    std::cout << "\nArithmetic edge cases:\n";
    checkValue("carry across BASE", CBigInt("4294967295") + CBigInt(1), "4294967296");
    checkValue("borrow across BASE", CBigInt("4294967296") + CBigInt(-1), "4294967295");
    checkValue("borrow across multiple digits",
               CBigInt("18446744073709551616") + CBigInt(-1), "18446744073709551615");
    checkValue("opposite-sign cancellation",
               CBigInt("-18446744073709551616") + CBigInt("18446744073709551616"), "0");
    checkValue("BASE squared",
               CBigInt("4294967295") * CBigInt("4294967295"), "18446744065119617025");
    checkValue("negative product",
               CBigInt("-4294967295") * CBigInt(2), "-8589934590");
    checkValue("negative zero product", CBigInt("-0") * CBigInt("-123"), "0");

    CBigInt small("25");
    CBigInt large("100");
    std::cout << "\nComparisons:\n";
    std::cout << "  25 < 100: " << (small < large) << "\n";
    std::cout << "  100 >= 25: " << (large >= small) << "\n";
    std::cout << "  25 == \"25\": " << (small == std::string("25")) << "\n";
    std::cout << "  -25 < 25: " << (negative < one) << "\n";

    if (!(small < large) || !(large >= small) ||
        !(small == std::string("25")) || !(negative < one)) {
        std::cerr << "FAIL: comparisons\n";
        ++failures;
    }
    checkCondition("BASE+1 > BASE",
                   CBigInt("4294967296") > CBigInt("4294967295"));
    checkCondition("-BASE-1 < -BASE",
                   CBigInt("-4294967296") < CBigInt("-4294967295"));
    checkCondition("negative values ordered by magnitude",
                   CBigInt("-100000000000000000000") < CBigInt("-99999999999999999999"));

    CBigInt input("123456789012345678901234567890");
    std::istringstream in("-00042");
    in >> input;
    std::cout << "\nInput/output:\n";
    std::cout << std::dec << "  input = " << input << "\n";

    if (input != "-42") {
        std::cerr << "FAIL: stream input/output\n";
        ++failures;
    }

    const std::string invalidByte(1, static_cast<char>(0xFF));
    bool invalidByteRejected = false;
    try {
        CBigInt invalid(invalidByte);
    } catch (const std::invalid_argument&) {
        invalidByteRejected = true;
    }
    checkCondition("reject non-ASCII byte in constructor", invalidByteRejected);

    CBigInt unchanged("123456789012345678901234567890");
    std::istringstream invalidInput(invalidByte);
    invalidInput >> unchanged;
    checkCondition("reject non-ASCII byte in stream", invalidInput.fail());
    checkCondition("failed stream extraction preserves existing value",
                   unchanged == "123456789012345678901234567890");

    std::cout << "\n";
    if (failures == 0)
        std::cout << "All tests passed.\n";
    else
        std::cout << failures << " test(s) failed.\n";

    return failures == 0 ? 0 : 1;
}