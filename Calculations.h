#pragma once

inline int stringToInt(QString String)
{
	if(String.contains(","))
	{
		if(String.length() == String.indexOf(",") +2)
		{
			String += "0";
		}
		else if(String.length() < String.indexOf(",") +2)
		{
			String += "00";
		}
	}
	else
	{
		String += "00";
	}

	String = String.replace(",", "");
	int Integer = String.toInt();
	return Integer;
}

inline QString intToString(int Integer)
{
	bool Negative = false;
	if(Integer < 0)
	{
		Integer = Integer * -1;
		Negative = true;
	}
	int Modulo = Integer % 100;
	QString Cents = QString::number(Modulo);
	if(Modulo < 10)
	{
		Cents.prepend("0");
	}
	QString String = QString::number(Integer / 100) + "," + Cents;
	if(Negative)
	{
		String.prepend("-");
	}
	return String;
}

inline QString getVATAmount(const QString &GrossAmount, const QString &VAT)
{
	int GrossInt = stringToInt(GrossAmount);
	int VATInt = ((GrossInt * (100 + VAT.toInt()) + 50) / 100) - GrossInt;
	QString VATAmount = intToString(VATInt);
	return VATAmount;
}

inline QString getNettAmount(const QString &GrossAmount, const QString &VATAmount)
{
	int GrossInt = stringToInt(GrossAmount);
	int VATInt = stringToInt(VATAmount);
	QString NettAmount = intToString(GrossInt + VATInt);
	return NettAmount;
}