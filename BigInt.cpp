
#include "BigInt.h"
#include <queue>
#include <string.h>
#include <map>
using namespace std;

BigInt::BigInt()
{
	arrBits[0] = 0;
	arrBits[1] = 0;
}

BigInt::BigInt(bool bit)
{
	if (bit == 1)
	{
		arrBits[0] = LLONG_MAX;
		arrBits[1] = LLONG_MAX;

		setBit(1, 63);
		setBit(1, 127);
	}
	else
	{
		arrBits[0] = 0;
		arrBits[1] = 0;
	}
}

BigInt::BigInt(const string &str, int base)
{
	arrBits[0] = 0;
	arrBits[1] = 0;
	
	switch (base)
	{
	case 2:
		converBinaryStringToBigInt(str);
		break;
	case 10:
		convertDecimalStringToBigInt(str);
		break;
	case 16:
		convertHexacalStringToBigInt(str);
		break;
	default:
		break;
	}
}

#pragma region get/set ...
void BigInt::setBit(bool bit, int pos)
{
	// vì arrBits là mãng tĩnh nên sizeof(arrBits) != sizeof(_int64*)
	if (pos < 0 || pos >= N_BIT)
		return;

	int divisor = sizeof(arrBits[0]) * 8;
	
	if (bit == 0)
	{	// Tắt bít 
		arrBits[pos / divisor] = arrBits[pos / divisor] & ~((__int64)1 << pos % divisor);
	}
	else
	{	// Bật bit
		arrBits[pos / divisor] = arrBits[pos / divisor] | ((__int64)1 << (pos % divisor));
	}
}

void BigInt::setBit(const string &str, int pos)
{
	if (pos < 0 || pos >= N_BIT)
		return;

	for (int i = str.length() - 1; i >= 0; i--)
	{
		setBit(str[i] - 48, pos);
		pos++;
	}
}

void BigInt::converBinaryStringToBigInt(const string &str)
{

	// reset lại đối tượng
	BigInt();

	// cập nhật dữ liệu
	setBit(str, 0);
}

bool BigInt::getBit(int pos) const
{
	if (pos < 0 || pos >= N_BIT)
		return 0;

	int divisor = sizeof(arrBits[0]) * 8;

	return (arrBits[pos / divisor] >> (pos % divisor)) & 1;
}
#pragma endregion 

void BigInt::convertDecimalStringToBigInt(string str)
{
	BigInt();

	bool isNegative = false;

	if (str[0] == '-')
	{
		isNegative = true;
		str = str.substr(1);
	}

	bool bit = 0;

	int i = 0;

	while (str != "" && i != 128)
	{
		// lấy dư

		bit = (str[str.length() - 1] - '0') % 2;
		
		setBit(bit, i);
		
		// chia 2
		str = strDiv(str);
		i++;
	}

	if (isNegative)
	{
		*this = getTwoComplement();
	}
}

void BigInt::convertHexacalStringToBigInt(string str)
{
	BigInt();

	string myMap[16] = {
		"0", "1", "0010", "11","100",
		"0101", "0110", "0111","1000", "1001",
		"1010", "1011", "1100", "1101", "1110", "1111"
	};

	int pos = 0;

	for (int i = str.length() - 1; i >= 0; i--)
	{
		if (str[i] >= 'A')
		{
			str[i] -= 7;
		}

		setBit(myMap[str[i] - 48], pos * 4);
		pos++;
	}
}

#pragma region toString methods
string BigInt::toString(int base, bool compact, bool addDelimiter, bool blockBit) const
{
	switch (base)
	{
	case 2:
		return toBinaryString(compact, addDelimiter, blockBit);
		break;
	case 10:
		return toDemicalString(addDelimiter);
		break;
	case 16:
		return toHexadecimalString(compact, addDelimiter);
		break;
	default:
		break;
	}

	return "NULL";
}

string BigInt::toDemicalString(bool addComma) const
{
	string res = "0";

	BigInt temp;

	bool isNegative = getBit(N_BIT - 1);
	bool flagMin = false;

	if (isNegative)
	{
		temp.setBit(1, 127);

		if (this->toBinaryString() == temp.toBinaryString())
		{// trường hợp đặc biệt. // MIN
			temp = ~(*this);
			flagMin = true;
		}
		else temp = this->getTwoComplement();

	}
	else 
		temp = *this;

	// Vì là số có dấu nên phạm vi chỉ nàm trong [2^-126, 2^127 - 1]
	for (int i = N_BIT - 2; i >= 0; i--)
	{
		// res * x + bit; x = 2;
		res = strMul(res, 2, false);

		// cộng bit: 0 || 1

		// Bit 0 -> cộng cũng đc mà k cộng cũng đc.
		// -> có thể bỏ dòng if này
		if (temp.getBit(i) == 0)
			continue;

		//321 + 1
		int num = 0;

		for (int j = res.length() - 1; j >= 0; j--)
		{
			num = res[j] - 48 + temp.getBit(i);

			res[j] = num % 10 + 48;
			num = num / 10;

			if (num == 0)
				break;
		}
	}

	if (addComma)
	{
		int count = 0;

		for (int i = res.length() - 1; i >= 0; i--)
		{
			count++;
			if (count % 3 == 0 && i != 0)
			{
				res.insert(res.begin() + i, ',');
			}
		}
	}

	if (isNegative)
	{
		// chơi trick :)) Min = -(Max + 1)
		if (flagMin)
			res[res.length() - 1]++;

		return "-" + res;
	}

	return res;
}

string BigInt::toBinaryString(bool compact, bool addSpace, bool blockBit) const
{
	string res = "";

	int bit = 0;
	int i = 0;

	if (compact)
	{
		// Rút gọn theo dạng đủ block 4-bit
		int pos = findPos_1_Bit_LeftMost();

		if (pos == -1)
			return "0";

		if (blockBit)
		{
			i = (pos / 8 + 1) * 8 - 1;
		}
		else
		{
			i = pos;
		}

		;
	}
	else i = N_BIT - 1;

	
	for (; i >= 0; i--)
	{
		bit = getBit(i);

		res.push_back(bit + '0');

		if (addSpace == true && i % 8 == 0 && i != 0)
			res.push_back(' ');
	}

	return res;
}

string BigInt::toHexadecimalString(bool compact, bool addSpace) const
{
	string res;

	int i = 0;
	if (compact)
	{
		i = (findPos_1_Bit_LeftMost() / 4 + 1 ) * 4 - 1;
	}
	else i = N_BIT - 1;
	
	char str[] = "0123456789ABCDEF";
	int sum = 0;

	while (i >= 0)
	{
		sum = 0;

		for (int j = 3; j >= 0; j--)
		{
			sum += getBit(i) * pow(2, j);
			i--;
		}

		res.push_back(str[sum]);

		if (addSpace == true && (i + 1) % 16 == 0 && i != -1)
			res.push_back(' ');
	}

	return res;
}
#pragma endregion 

#pragma region Operators ...
BigInt BigInt::operator+(const BigInt &b) const
{
	BigInt res;
	int sumBit = 0;
	int bitNho = 0;

	//          Dư : 0, 0, 1, 1
	int arrBit[] = { 0, 1, 0, 1 };

	for (int i = 0; i < N_BIT; i++)
	{
		sumBit = getBit(i) + b.getBit(i) + bitNho;

		bitNho = sumBit / 2;

		res.setBit(arrBit[sumBit], i);
	}

	return res;
}

BigInt BigInt::operator-(const BigInt &b) const
{
	BigInt res, b_twoCmp = b.getTwoComplement();

	res = *this + b_twoCmp;

	return res;
}

BigInt BigInt::operator*(const BigInt &b) const
{
	BigInt A, Q = *this, M = b;
	_int64 bitNho = 0, bitSign = 0, bitTemp = 0;

	for (int i = N_BIT; i >= 1; i--)
	{
		if (Q.getBit(0) == 1 && bitNho == 0)
		{
			A = A - M;
		}

		else if (Q.getBit(0) == 0 && bitNho == 1)
		{
			A = A + M;
		}

		bitNho = Q.getBit(0);
		bitSign = A.getBit(N_BIT - 1);
		bitTemp = A.getBit(0);

		A = A >> 1;
		A.setBit(bitSign, N_BIT - 1);

		Q = Q >> 1;
		Q.setBit(bitTemp, N_BIT - 1);
	}

	return Q;
}

BigInt BigInt::operator/(const BigInt &b) const
{

	bool khacDau = true;
	
	// cùng dấu
	if (this->getBit(127) == b.getBit(127))
	{
		khacDau = false;
	}
	// else khacDau = true;
	
	BigInt A;
	BigInt Q = *this, M = b;

	if (this->getBit(127) == 1)
	{
		Q = Q.getTwoComplement();
	}

	if (b.getBit(127) == 1)
	{
		M = M.getTwoComplement();
	}

	bool bitTemp = 0;

	for (int i = 0; i < N_BIT; i++)
	{
		//Shift Left [A,Q]
		bitTemp = Q.getBit(127);
		A = A << 1;
		Q = Q << 1;
		A.setBit(bitTemp, 0);

		A = A - M;

		if (A.getBit(127) == 1)
		{
			A = A + M;
			Q.setBit(0, 0);
		}
		else
		{
			Q.setBit(1, 0);
		}
	}

	if (khacDau)
	{
		A = A.getTwoComplement();
		Q = Q.getTwoComplement();
	}

	return Q;
}

BigInt BigInt::operator%(const BigInt &b) const
{

	bool khacDau = true;

	// cùng dấu
	if (this->getBit(127) == b.getBit(127))
	{
		khacDau = false;
	}
	// else khacDau = true;

	BigInt A;
	BigInt Q = *this, M = b;

	if (this->getBit(127) == 1)
	{
		Q = Q.getTwoComplement();
	}

	if (b.getBit(127) == 1)
	{
		M = M.getTwoComplement();
	}

	bool bitTemp = 0;

	for (int i = 0; i < N_BIT; i++)
	{
		//Shift Left [A,Q]
		bitTemp = Q.getBit(127);
		A = A << 1;
		Q = Q << 1;
		A.setBit(bitTemp, 0);

		A = A - M;

		if (A.getBit(127) == 1)
		{
			A = A + M;
			Q.setBit(0, 0);
		}
		else
		{
			Q.setBit(1, 0);
		}
	}

	if (khacDau)
	{
		A = A.getTwoComplement();
		Q = Q.getTwoComplement();
	}

	return A;
}

BigInt BigInt::operator<<(int pos) const
{
	if (pos < 0 || pos >= N_BIT)
		return *this;

	BigInt res;

	for (int i = N_BIT - 1; i >= pos; i--)
	{
		res.setBit(getBit(i - pos), i);
	}

	return res;
}

bool BigInt::operator>(const BigInt &b) const
{
	bool bit_a = this->getBit(127);
	bool bit_b = b.getBit(127);

	if (bit_a == 1 && bit_b == 0)
	{
		return false;
	}

	if (bit_a == 0 && bit_b == 1)
	{
		return true;
	}

	for (int i = 126; i >= 0; i--)
	{
		bit_a = this->getBit(i);
		bit_b = b.getBit(i);

		if (bit_a < bit_b)
		{
			return false;
		}
		else if (bit_a > bit_b)
		{
			return true;
		}
	}

	return false;
}

bool BigInt::operator<(const BigInt &b) const
{
	bool bit_a = this->getBit(127);
	bool bit_b = b.getBit(127);

	if (bit_a == 1 && bit_b == 0)
	{
		return true;
	}

	if (bit_a == 0 && bit_b == 1)
	{
		return false;
	}

	for (int i = 126; i >= 0; i--)
	{
		bit_a = this->getBit(i);
		bit_b = b.getBit(i);

		if (bit_a < bit_b)
		{
			return true;
		}
		else if (bit_a > bit_b)
		{
			return false;
		}
	}

	return false;
}

BigInt BigInt::operator>>(int pos) const
{
	if (pos < 0 || pos >= N_BIT)
		return *this;

	BigInt res;

	for (int i = 0; i < N_BIT - pos; i++)
	{
		res.setBit(getBit(i + pos), i);
	}

	return res;
}

BigInt BigInt::operator~() const
{
	BigInt res;

	for (int i = 0; i < N_BIT; i++)
	{
		res.setBit(!getBit(i), i);
	}

	return res;
}

BigInt BigInt::operator|(const BigInt &b) const
{
	BigInt res;
	bool bit_a, bit_b;

	for (int i = 0; i < N_BIT; i++)
	{
		bit_a = this->getBit(i);
		bit_b = b.getBit(i);

		if (bit_a == 0 && bit_b == 0)
			res.setBit(0, i);
		else res.setBit(1, i);
	}

	return res;
}

BigInt BigInt::operator&(const BigInt &b) const
{
	BigInt res;
	
	for (int i = 0; i < N_BIT; i++)
	{
		int bit = this->getBit(i)* b.getBit(i);
		res.setBit(bit, i);
	}

	return res;
}

BigInt BigInt::operator^(const BigInt &b) const
{
	BigInt res;

	for (int i = 0; i < N_BIT; i++)
	{
		int bit = (int)(this->getBit(i) + b.getBit(i)) % 2;

		res.setBit(bit, i);
	}

	return res;
}

BigInt BigInt::ror() const
{
	bool LSB = getBit(0);

	BigInt res = *this >> 1;

	res.setBit(LSB, 127);

	return res;
}

BigInt BigInt::rol() const
{
	bool MSB = getBit(127);

	BigInt res = *this << 1;

	res.setBit(MSB, 0);

	return res;
}

#pragma endregion 

#pragma region Supporter ...
string BigInt::strDiv(string str, int divisor)
{
	if (str.length() == 1)
	{
		if (str[0] - 48 < divisor)
			return "";
	}

	string res;

	queue<char> que;

	for (int i = 0; i < str.length(); i++)
	{
		que.push(str[i]);
	}

	bool checkDu = true;
	int a = que.front() - 48;
	int i = 0;

	if (a < divisor)
	{
		checkDu = false;
	}

	while (!que.empty())
	{
		if (checkDu)
		{
			a = que.front() - 48;
		}
		else
		{
			que.pop();

			if (que.empty())
				break;

			a = a * 10 + que.front() - 48;

		}

		res.push_back(a / divisor + '0');

		if (a % divisor != 0)
		{
			a = a % divisor;
			checkDu = false;
		}
		else
		{
			que.pop();
			checkDu = true;
		}
	}

	return res;
}

string BigInt::strMul(string str, int multiplier, bool isReverse)
{
	string res;

	int kqNhan = 0;
	int soThua = 0;

	for (int i = str.length() - 1; i >= 0; i--)
	{
		kqNhan = (str[i] - 48) * multiplier + soThua;

		soThua = kqNhan / 10;

		res.push_back(kqNhan % 10 + 48);
	}

	if (soThua != 0)
		res.push_back(soThua + 48);

	if (isReverse == true)
		return res;

	return string(res.rbegin(), res.rend());
}

BigInt BigInt::getTwoComplement() const
{
	int pos = findPos_1_Bit_RightMost();
	BigInt a;
	
	if (pos == -1) return a;// a = 0

	a.setBit(1, pos);

	for (int i = pos + 1; i < N_BIT; i++)
	{
		
		//bool bit = getBit(i); debug thôi
		a.setBit(!getBit(i) , i);
	}

	return a;
}

int BigInt::findPos_1_Bit_RightMost() const
{

	for (int i = 0; i < N_BIT; i++)
	{
		if (getBit(i))
			return i;
	}

	return -1;
}

int BigInt::findPos_1_Bit_LeftMost() const
{
	for (int i = N_BIT - 1; i >= 0; i--)
	{
		if (getBit(i))
			return i;
	}

	return -1;
}

#pragma endregion 

