#include "Value.h"

Value::Value(int avgV, int sumV) {
	this->avgV = avgV;
	this->sumV = sumV;
	this->all = avgV + sumV;
}
Value::Value() {}