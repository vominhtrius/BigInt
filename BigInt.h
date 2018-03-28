/*
	----------------------------CONTACT--------------------------------------
	*					Đồ án Kiến Trúc Máy tính							*
	-------------------------------------------------------------------------
	*																		*
	*					ID: 1512676 - Võ Minh Trí							*
	*																		*
	-------------------------------------------------------------------------
*/

#ifndef _BIGINT_H
#define _BIGINT_H

#include <iostream>
#include <string>

#define N_BIT 128
#define DEC 10
#define BIN 2
#define HEX 16

using namespace std;
	
class BigInt
{
public:

	/*---------------------------------------------------------------*/
	BigInt();
	BigInt(bool bit);
	
	// Base : 2, 10, 16...
	BigInt(const string &str, int base = DEC);

	/*---------------------------------------------------------------*/
	/*
	*	Hàm bật/tắt bit một vị trí trong dãy bit.
	*	Input : 
	*		- bit : true = bật bit | false = tắt bit.
	*		- pos : vị trí cần bật/tắt bit.
	*	Output :
	*/
	void setBit(bool bit, int start_pos);
	void setBit(const string &str, int pos);

	/*
	*	Hàm lấy 1 bit tại vị trong dãy bit của BigInt.
	*	Input : 
	*		- pos : vị trí muốn lấy bit.
	*	Output :
	*		- Trả về bit tại vị trí pos.
	*
	*/
	bool getBit(int pos) const;

	/*
	*	Hàm đổi CHUỖI THẬP PHÂN sang BigInt.
	*	Input :
	*		- str : Chuỗi thập phân cần chuyển đổi.
	*	Output : cập nhật lại giá trị của đối tượng.
	*/
	void convertDecimalStringToBigInt(string str);

	/*
	*	Hàm đổi CHUỖI LỤC THẬP PHÂN sang BigInt.
	*	Input :
	*		- str : Chuỗi thập phân cần chuyển đổi.
	*	Output : cập nhật lại giá trị của đối tượng.
	*/
	void convertHexacalStringToBigInt(string str);

	/*
	*	Hàm chuyển giá trị BigInt sang chuỗi theo hệ cơ số quy định.
	*	Input : 
	*		- base : Hệ cơ số cần chuyển sang {2, 10, 16}.
	*		- compact : true = Rút gọn chuỗi ( VD : 01 1000 -> 1 1000).
	*		- addDelimiter : true = thêm ký tự ngăn cách ( ',' đối với hệ 2, [Space] đối với hệ 10 và 16).
	*		- blockBit : Chỉ có tác dụng với hệ cơ số 2. true = trả về chuối rút gọn dưới dạng block 8-bits.
	*	Output :
	*		- Chuỗi định dạng theo tham số truyền vào. 
	*/
	string toString(int base = BIN, bool compact = true, bool addDelimiter = false, bool blockBit = false) const;

	/*
	*	Hàm chuyển một dãy bit tại một vị trí pos vào trong dãy bit.
	*	Input :
	*		- strBit : Chuỗi bit cần bật tắt.
	*		- pos : vị trí bắt đầu.
	*	Output :
	*/
	void converBinaryStringToBigInt(const string &strBit);

	/*
	*	Hàm chuyển giá trị của BigInt sang hệ 10.
	*	Input :
	*		- addDelimiter : true = thêm ký tự ngăn cách ','.
	*	Output :
	*		- Chuỗi hệ 10 định dạng theo tham số truyền vào.
	*/
	string toDemicalString(bool addDelimiter = false) const;

	/*
	*	Hàm chuyển giá trị của BigInt sang hệ 2.
	*	Input :
	*		- compac : true = rút gọn chuỗi trả về (bỏ bớt bit 0 bên trái)
	*		- addDelimiter : true = thêm ký tự ngăn cách [Space].
	*		- blockBit : true = rút gọn theo các block 8-bits;
	*	Output :
	*		- Chuỗi hệ 2 định dạng theo tham số truyền vào.
	*/
	string toBinaryString(bool compact = true, bool addDelimiter = true, bool blockBit = true) const;

	/*
	*	Hàm chuyển giá trị của BigInt sang hệ 16.
	*	Input :
	*		- addDelimiter : true = thêm ký tự ngăn cách [Space].
	*	Output :
	*		- Chuỗi hệ 16 định dạng theo tham số truyền vào.
	*/
	string toHexadecimalString(bool compact = true, bool addDelimiter = true) const;

	/*---------------------------------------------------------------*/
	bool operator>(const BigInt &b) const;
	bool operator<(const BigInt &b) const;
	BigInt operator+(const BigInt &b) const;
	BigInt operator-(const BigInt &b) const;
	BigInt operator*(const BigInt &b) const;
	BigInt operator/(const BigInt &b) const;
	BigInt operator%(const BigInt &b) const;

	BigInt operator<<(int n) const;
	BigInt operator>>(int n) const;
	BigInt operator~() const;
	BigInt operator|(const BigInt &b) const;
	BigInt operator&(const BigInt &b) const;
	BigInt operator^(const BigInt &b) const;

	BigInt ror() const;
	BigInt rol() const;

	/*
	*	Hàm lấy dãy bit bù 2.
	*	Input : none.
	*	Output:
			- Dãy bit bù 2.
	*/
	BigInt getTwoComplement() const;

	/*---------------------------------------------------------------*/

	// Support methods.

	/*
	*	Hàm chia chuỗi hệ 10 với 1 số nguyên
	*	Input :
	*		- str : Chuỗi số hệ 10 bị chia.
	*		- divisor : Số chia.
	*	Output:
	*		- Chuỗi kết quả phép chia.
	*/
	static string strDiv(string str, int divisor = 2);

	/*
	*	Hàm nhân chuỗi hệ 10 với 1 số nguyên.
	*	Input :
	*		- str : Chuỗi số hệ 10 
	*		- divisor : Số nhân
	*	Output:
	*		- Chuỗi kết quả phép nhân.
	*/
	static string strMul(string str, int multiplier, bool isReverse = false);

	// Hàm tìm bit 1 phải nhất.
	int findPos_1_Bit_RightMost() const;

	// Hàm tìm bit 1 trái nhất.
	int findPos_1_Bit_LeftMost() const;
private:
	
	__int64 arrBits[2];
};
#endif // !_QINT_H