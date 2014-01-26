#pragma once
#include <BxType.hpp>

#define BxInteger(VALUE)  (BxExpress::Integer::Make<(int)(VALUE)>())
#define BxIntegerX(VALUE) (BxExpress::IntegerX::Make<(int)(VALUE)>())
#define BxFloat(VALUE)    (BxExpress::Float::Make<(fint)((VALUE) * ItoF(1))>())
#define BxFloatX(VALUE)   (BxExpress::FloatX::Make<(fint)((VALUE) * ItoF(1))>())

//! \brief 고속처리
namespace BxExpress
{
	class _Integer;
	class Integer;
	class IntegerX;
	class _Float;
	class Float;
	class FloatX;
	inline Integer operator+(const int v1,       const _Integer& v2);
	inline Float   operator+(const int v1,       const _Float& v2  );
	inline Integer operator+(const _Integer& v1, const int v2      );
	inline Integer operator+(const _Integer& v1, const _Integer& v2);
	inline Float   operator+(const _Integer& v1, const _Float& v2  );
	inline Float   operator+(const _Float& v1,   const int v2      );
	inline Float   operator+(const _Float& v1,   const _Integer& v2);
	inline Float   operator+(const _Float& v1,   const _Float& v2  );
	inline Integer operator-(const int v1,       const _Integer& v2);
	inline Float   operator-(const int v1,       const _Float& v2  );
	inline Integer operator-(const _Integer& v1, const int v2      );
	inline Integer operator-(const _Integer& v1, const _Integer& v2);
	inline Float   operator-(const _Integer& v1, const _Float& v2  );
	inline Float   operator-(const _Float& v1,   const int v2      );
	inline Float   operator-(const _Float& v1,   const _Integer& v2);
	inline Float   operator-(const _Float& v1,   const _Float& v2  );
	inline Integer operator*(const int v1,       const _Integer& v2);
	inline Float   operator*(const int v1,       const _Float& v2  );
	inline Integer operator*(const _Integer& v1, const int v2      );
	inline Integer operator*(const _Integer& v1, const _Integer& v2);
	inline Float   operator*(const _Integer& v1, const _Float& v2  );
	inline Float   operator*(const _Float& v1,   const int v2      );
	inline Float   operator*(const _Float& v1,   const _Integer& v2);
	inline Float   operator*(const _Float& v1,   const _Float& v2  );
	inline Integer operator/(const int v1,       const IntegerX& v2);
	inline Float   operator/(const int v1,       const FloatX& v2  );
	inline Integer operator/(const huge v1,      const IntegerX& v2);
	inline Float   operator/(const huge v1,      const FloatX& v2  );
	inline Integer operator/(const _Integer& v1, const IntegerX& v2);
	inline Float   operator/(const _Integer& v1, const FloatX& v2  );
	inline Float   operator/(const _Float& v1,   const IntegerX& v2);
	inline Float   operator/(const _Float& v1,   const FloatX& v2  );

	class _Integer
	{
		friend class Integer;
		friend class IntegerX;
		friend class Float;
		friend class FloatX;
		friend Integer operator+(const int v1,       const _Integer& v2);
		friend Integer operator+(const _Integer& v1, const int v2      );
		friend Integer operator+(const _Integer& v1, const _Integer& v2);
		friend Float   operator+(const _Integer& v1, const _Float& v2  );
		friend Float   operator+(const _Float& v1,   const _Integer& v2);
		friend Integer operator-(const int v1,       const _Integer& v2);
		friend Integer operator-(const _Integer& v1, const int v2      );
		friend Integer operator-(const _Integer& v1, const _Integer& v2);
		friend Float   operator-(const _Integer& v1, const _Float& v2  );
		friend Float   operator-(const _Float& v1,   const _Integer& v2);
		friend Integer operator*(const int v1,       const _Integer& v2);
		friend Integer operator*(const _Integer& v1, const int v2      );
		friend Integer operator*(const _Integer& v1, const _Integer& v2);
		friend Float   operator*(const _Integer& v1, const _Float& v2  );
		friend Float   operator*(const _Float& v1,   const _Integer& v2);
		friend Integer operator/(const _Integer& v1, const IntegerX& v2);
		friend Float   operator/(const _Integer& v1, const FloatX& v2  );
		// 생성자
		private: _Integer() {}
		private: explicit _Integer(const int v) : Value(v) {}
		// 연산자
		public:  inline bool operator<(const _Integer& v) const {return Value < v.Value;}
		public:  inline bool operator>(const _Integer& v) const {return Value > v.Value;}
		public:  inline bool operator<=(const _Integer& v) const {return Value <= v.Value;}
		public:  inline bool operator>=(const _Integer& v) const {return Value >= v.Value;}
		public:  inline bool operator==(const _Integer& v) const {return Value == v.Value;}
		public:  inline bool operator!=(const _Integer& v) const {return Value != v.Value;}
		public:  inline operator int() const {return Value;}
		// 데이터
		private: int Value;
	};
	class Integer : public _Integer
	{
		friend class _Float;
		friend class Float;
		friend Integer operator+(const int v1,       const _Integer& v2);
		friend Integer operator+(const _Integer& v1, const int v2      );
		friend Integer operator+(const _Integer& v1, const _Integer& v2);
		friend Integer operator-(const int v1,       const _Integer& v2);
		friend Integer operator-(const _Integer& v1, const int v2      );
		friend Integer operator-(const _Integer& v1, const _Integer& v2);
		friend Integer operator*(const int v1,       const _Integer& v2);
		friend Integer operator*(const _Integer& v1, const int v2      );
		friend Integer operator*(const _Integer& v1, const _Integer& v2);
		friend Integer operator/(const int v1,       const IntegerX& v2);
		friend Integer operator/(const huge v1,      const IntegerX& v2);
		friend Integer operator/(const _Integer& v1, const IntegerX& v2);
		// 생성자
		public:  Integer() {}
		public:  Integer(const Integer& v) : _Integer(v.Value) {}
		public:  explicit Integer(const _Integer& v) : _Integer(v.Value) {}
		private: explicit Integer(const int v, void*) : _Integer(v) {}
		// 연산자
		public:  inline Integer operator-() const {return Integer(-Value, nullptr);}
		public:  inline Integer operator<<(const int v) const {return Integer(Value << v, nullptr);}
		public:  inline Integer operator>>(const int v) const {return Integer(Value >> v, nullptr);}
		public:  inline Integer& operator=(const _Integer& v) {Value = v.Value; return *this;}
		// 할당자
		public:  template<int VALUE> global_func const Integer& Make() {global_data const Integer Result(VALUE, nullptr); return Result;}
	};
	class IntegerX : public _Integer
	{
		friend class _Float;
		friend class FloatX;
		friend Integer operator/(const int v1,       const IntegerX& v2);
		friend Integer operator/(const huge v1,      const IntegerX& v2);
		friend Integer operator/(const _Integer& v1, const IntegerX& v2);
		friend Float   operator/(const _Float& v1,   const IntegerX& v2);
		// 생성자
		public:  IntegerX() {}
		public:  IntegerX(const IntegerX& v) : _Integer(v.Value), InverseFF(v.InverseFF) {}
		public:  explicit IntegerX(const Integer& v) : _Integer(v.Value), InverseFF(0) {}
		private: explicit IntegerX(const int v, void*) : _Integer(v), InverseFF(0) {}
		// 연산자
		public:  inline operator int() const {return Value;}
		private: inline IntegerX& operator=(const Integer& v) {Value = v.Value; InverseFF = 0; return *this;}
		public:  inline IntegerX& operator=(const IntegerX& v) {Value = v.Value; InverseFF = v.InverseFF; return *this;}
		// 강제초기화
		public:  void Reset(const Integer& v) {Value = v.Value; InverseFF = 0;}
		// 할당자
		public:  template<int VALUE> global_func const IntegerX& Make() {global_data const IntegerX Result(VALUE, nullptr); return Result;}
		// 데이터
		private: mutable fint InverseFF;
		private: inline fint GetInverseFF() const {return (InverseFF)? InverseFF : (InverseFF = (fint)(ox100000000 / Value));}
	};

	class _Float
	{
		friend class Float;
		friend class FloatX;
		friend Float   operator+(const int v1,       const _Float& v2  );
		friend Float   operator+(const _Integer& v1, const _Float& v2  );
		friend Float   operator+(const _Float& v1,   const int v2      );
		friend Float   operator+(const _Float& v1,   const _Integer& v2);
		friend Float   operator+(const _Float& v1,   const _Float& v2  );
		friend Float   operator-(const int v1,       const _Float& v2  );
		friend Float   operator-(const _Integer& v1, const _Float& v2  );
		friend Float   operator-(const _Float& v1,   const int v2      );
		friend Float   operator-(const _Float& v1,   const _Integer& v2);
		friend Float   operator-(const _Float& v1,   const _Float& v2  );
		friend Float   operator*(const int v1,       const _Float& v2  );
		friend Float   operator*(const _Integer& v1, const _Float& v2  );
		friend Float   operator*(const _Float& v1,   const int v2      );
		friend Float   operator*(const _Float& v1,   const _Integer& v2);
		friend Float   operator*(const _Float& v1,   const _Float& v2  );
		friend Float   operator/(const _Float& v1,   const IntegerX& v2);
		friend Float   operator/(const _Float& v1,   const FloatX& v2  );
		// 생성자
		private: _Float() {}
		private: explicit _Float(const fint v) : ValueF(v) {}
		// 연산자
		public:  inline bool operator<(const _Float& v) const {return ValueF < v.ValueF;}
		public:  inline bool operator>(const _Float& v) const {return ValueF > v.ValueF;}
		public:  inline bool operator<=(const _Float& v) const {return ValueF <= v.ValueF;}
		public:  inline bool operator>=(const _Float& v) const {return ValueF >= v.ValueF;}
		public:  inline bool operator==(const _Float& v) const {return ValueF == v.ValueF;}
		public:  inline bool operator!=(const _Float& v) const {return ValueF != v.ValueF;}
		// 기타
		public:  inline fint ToFixFloat() const {return ValueF;}
		public:  inline int ToInt() const {return FtoI(ValueF);}
		public:  inline Integer ToInteger() const {return Integer(FtoI(ValueF), nullptr);}
		public:  inline IntegerX ToIntegerX() const {return IntegerX(FtoI(ValueF), nullptr);}
		// 데이터
		private: fint ValueF;
	};
	class Float : public _Float
	{
		friend Float   operator+(const int v1,       const _Float& v2  );
		friend Float   operator+(const _Integer& v1, const _Float& v2  );
		friend Float   operator+(const _Float& v1,   const int v2      );
		friend Float   operator+(const _Float& v1,   const _Integer& v2);
		friend Float   operator+(const _Float& v1,   const _Float& v2  );
		friend Float   operator-(const int v1,       const _Float& v2  );
		friend Float   operator-(const _Integer& v1, const _Float& v2  );
		friend Float   operator-(const _Float& v1,   const int v2      );
		friend Float   operator-(const _Float& v1,   const _Integer& v2);
		friend Float   operator-(const _Float& v1,   const _Float& v2  );
		friend Float   operator*(const int v1,       const _Float& v2  );
		friend Float   operator*(const _Integer& v1, const _Float& v2  );
		friend Float   operator*(const _Float& v1,   const int v2      );
		friend Float   operator*(const _Float& v1,   const _Integer& v2);
		friend Float   operator*(const _Float& v1,   const _Float& v2  );
		friend Float   operator/(const int v1,       const FloatX& v2  );
		friend Float   operator/(const huge v1,      const FloatX& v2  );
		friend Float   operator/(const _Integer& v1, const FloatX& v2  );
		friend Float   operator/(const _Float& v1,   const IntegerX& v2);
		friend Float   operator/(const _Float& v1,   const FloatX& v2  );
		// 생성자
		public:  Float() {}
		public:  Float(const Float& v) : _Float(v.ValueF) {}
		public:  explicit Float(const _Float& v) : _Float(v.ValueF) {}
		public:  explicit Float(const _Integer& v) : _Float(ItoF(v.Value)) {}
		private: explicit Float(const fint v, void*) : _Float(v) {}
		// 연산자
		public:  inline Float operator-() const {return Float(-ValueF, nullptr);}
		public:  inline Float operator<<(const int v) const {return Float(ValueF << v, nullptr);}
		public:  inline Float operator>>(const int v) const {return Float(ValueF >> v, nullptr);}
		public:  inline Float& operator=(const _Float& v) {ValueF = v.ValueF; return *this;}
		public:  inline Float& operator=(const _Integer& v) {ValueF = ItoF(v.Value); return *this;}
		// 할당자
		public:  template<fint VALUE> global_func const Float& Make() {global_data const Float Result(VALUE, nullptr); return Result;}
	};
	class FloatX : public _Float
	{
		friend Float   operator/(const int v1,       const FloatX& v2  );
		friend Float   operator/(const huge v1,      const FloatX& v2  );
		friend Float   operator/(const _Integer& v1, const FloatX& v2  );
		friend Float   operator/(const _Float& v1,   const FloatX& v2  );
		// 생성자
		public:  FloatX(const FloatX& v) : _Float(v.ValueF), InverseF(v.InverseF) {}
		public:  explicit FloatX(const Float& v) : _Float(v.ValueF), InverseF(0) {}
		public:  explicit FloatX(const Integer& v) : _Float(ItoF(v.Value)), InverseF(0) {}
		public:  explicit FloatX(const IntegerX& v) : _Float(ItoF(v.Value)), InverseF(0) {}
		private: explicit FloatX(const fint v, void*) : _Float(v), InverseF(0) {}
		// 연산자
		private: inline FloatX& operator=(const Float& v) {ValueF = v.ValueF; InverseF = 0; return *this;}
		public:  inline FloatX& operator=(const FloatX& v) {ValueF = v.ValueF; InverseF = v.InverseF; return *this;}
		private: inline FloatX& operator=(const Integer& v) {ValueF = ItoF(v.Value); InverseF = 0; return *this;}
		private: inline FloatX& operator=(const IntegerX& v) {ValueF = ItoF(v.Value); InverseF = 0; return *this;}
		// 강제초기화
		public:  void Reset(const Float& v) {ValueF = v.ValueF; InverseF = 0;}
		public:  void Reset(const Integer& v) {ValueF = ItoF(v.Value); InverseF = 0;}
		// 할당자
		public:  template<fint VALUE> global_func const FloatX& Make() {global_data const FloatX Result(VALUE, nullptr); return Result;}
		// 데이터
		private: mutable fint InverseF;
		private: inline fint GetInverseF() const {return (InverseF)? InverseF : (InverseF = (fint)(ox100000000 / ValueF));}
	};

	// ADD
	Integer operator+(const int v1,       const _Integer& v2) {return Integer(v1 + v2.Value, nullptr);}
	Float   operator+(const int v1,       const _Float& v2  ) {return Float(ItoF(v1) + v2.ValueF, nullptr);}
	Integer operator+(const _Integer& v1, const int v2      ) {return Integer(v1.Value + v2, nullptr);}
	Integer operator+(const _Integer& v1, const _Integer& v2) {return Integer(v1.Value + v2.Value, nullptr);}
	Float   operator+(const _Integer& v1, const _Float& v2  ) {return Float(ItoF(v1.Value) + v2.ValueF, nullptr);}
	Float   operator+(const _Float& v1,   const int v2      ) {return Float(v1.ValueF + ItoF(v2), nullptr);}
	Float   operator+(const _Float& v1,   const _Integer& v2) {return Float(v1.ValueF + ItoF(v2.Value), nullptr);}
	Float   operator+(const _Float& v1,   const _Float& v2  ) {return Float(v1.ValueF + v2.ValueF, nullptr);}
	// SUB
	Integer operator-(const int v1,       const _Integer& v2) {return Integer(v1 - v2.Value, nullptr);}
	Float   operator-(const int v1,       const _Float& v2  ) {return Float(ItoF(v1) - v2.ValueF, nullptr);}
	Integer operator-(const _Integer& v1, const int v2      ) {return Integer(v1.Value - v2, nullptr);}
	Integer operator-(const _Integer& v1, const _Integer& v2) {return Integer(v1.Value - v2.Value, nullptr);}
	Float   operator-(const _Integer& v1, const _Float& v2  ) {return Float(ItoF(v1.Value) - v2.ValueF, nullptr);}
	Float   operator-(const _Float& v1,   const int v2      ) {return Float(v1.ValueF - ItoF(v2), nullptr);}
	Float   operator-(const _Float& v1,   const _Integer& v2) {return Float(v1.ValueF - ItoF(v2.Value), nullptr);}
	Float   operator-(const _Float& v1,   const _Float& v2  ) {return Float(v1.ValueF - v2.ValueF, nullptr);}
	// MUL
	Integer operator*(const int v1,       const _Integer& v2) {return Integer(v1 * v2.Value, nullptr);}
	Float   operator*(const int v1,       const _Float& v2  ) {return Float((fint)((ItoF(v1) * (huge) v2.ValueF) >> 16), nullptr);}
	Integer operator*(const _Integer& v1, const int v2      ) {return Integer(v1.Value * v2, nullptr);}
	Integer operator*(const _Integer& v1, const _Integer& v2) {return Integer(v1.Value * v2.Value, nullptr);}
	Float   operator*(const _Integer& v1, const _Float& v2  ) {return Float((fint)((ItoF(v1.Value) * (huge) v2.ValueF) >> 16), nullptr);}
	Float   operator*(const _Float& v1,   const int v2      ) {return Float((fint)((v1.ValueF * (huge) ItoF(v2)) >> 16), nullptr);}
	Float   operator*(const _Float& v1,   const _Integer& v2) {return Float((fint)((v1.ValueF * (huge) ItoF(v2.Value)) >> 16), nullptr);}
	Float   operator*(const _Float& v1,   const _Float& v2  ) {return Float((fint)((v1.ValueF * (huge) v2.ValueF) >> 16), nullptr);}
	// DIV
	Integer operator/(const int v1,       const IntegerX& v2) {return Integer((int)((v1 * (huge) v2.GetInverseFF()) >> 32), nullptr);}
	Float   operator/(const int v1,       const FloatX& v2  ) {return Float(v1 * v2.GetInverseF(), nullptr);}
	Integer operator/(const huge v1,      const IntegerX& v2) {return Integer((int)((v1 * v2.GetInverseFF()) >> 32), nullptr);}
	Float   operator/(const huge v1,      const FloatX& v2  ) {return Float((int)(v1 * v2.GetInverseF()), nullptr);}
	Integer operator/(const _Integer& v1, const IntegerX& v2) {return Integer((int)((v1.Value * (huge) v2.GetInverseFF()) >> 32), nullptr);}
	Float   operator/(const _Integer& v1, const FloatX& v2  ) {return Float(v1.Value * v2.GetInverseF(), nullptr);}
	Float   operator/(const _Float& v1,   const IntegerX& v2) {return Float((fint)((v1.ValueF * (huge) v2.GetInverseFF()) >> 32), nullptr);}
	Float   operator/(const _Float& v1,   const FloatX& v2  ) {return Float((fint)((v1.ValueF * (huge) v2.GetInverseF()) >> 16), nullptr);}
}
