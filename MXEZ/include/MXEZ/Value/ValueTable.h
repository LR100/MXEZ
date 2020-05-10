#ifndef VALUETABLE_H_
#define VALUETABLE_H_

#include <vector>

namespace MXEZ
{
	template <typename T>
	class PrecomputeValueTable
	{
	public:
		PrecomputeValueTable() {};
		virtual ~PrecomputeValueTable();

		T				GetValue(unsigned int inc) const;
		void			ApplyMulCoef(T coef);
		unsigned int	Size() const;

	protected:
		std::vector<T>	_table;
	};


	class PrecomputeFloatValueTable
	{
	public:
		PrecomputeFloatValueTable(unsigned int precision);
		virtual ~PrecomputeFloatValueTable();

		virtual float	ComputeValue(unsigned int precision, unsigned int nbValue, unsigned int inc) const = 0;
		unsigned int	GetPrecision() const;

		float			GetValue(unsigned int inc) const;
		void			ApplyMulCoef(float coef);
		unsigned int	Size() const;

	protected:

		std::vector<float>	_table;
		void				InitValueTable(unsigned int precision, unsigned int nbValue);

	private:
		unsigned int	_precision;
	};

	class CosTable : public PrecomputeFloatValueTable
	{
	public:
		CosTable(unsigned int precision);

		virtual float	ComputeValue(unsigned int precision, unsigned int nbValue, unsigned int inc) const override;
	};

	class SinTable : public PrecomputeFloatValueTable
	{
	public:
		SinTable(unsigned int precision);

		virtual float	ComputeValue(unsigned int precision, unsigned int nbValue, unsigned int inc) const override;
	};

}

#endif // VALUETABLE_H_