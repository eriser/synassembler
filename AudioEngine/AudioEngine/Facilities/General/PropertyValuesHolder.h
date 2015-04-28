
#pragma once

#include <vector>
#include <string>

//class EUKind;

#include "..\..\ElaborationUnits\EUKind.h"
#include "..\..\ElaborationUnits\ModuleServices.h"

class ElaborationUnit;

#define UNAVALIABLE_VALUE -1

class PropertyValuesHolder
{
public:
	PropertyValuesHolder(ModuleServices* pServices, int numProperties, const EUKind* kind, ElaborationUnit* eu);
	int setValue(int propertyIndex, const char* value);
	int getValue(int propertyIndex, char* value, int bufferSize);
	int setDValue(int propertyIndex, double value);
	int getDValue(int propertyIndex, double* value);
	int setIValue(int propertyIndex, int value);
	int getIValue(int propertyIndex, int* value);
private:
	class ValueHolder
	{
	public:
		std::string strValue;
	};
	std::vector<ValueHolder> m_Values;

	class DValueHolder
	{
	public:
		double dValue;
	};
	std::vector<DValueHolder> m_Dvalues;

	class IValueHolder
	{
	public:
		int iValue;
	};
	std::vector<IValueHolder> m_IValues;

	const EUKind* m_pKind;
	ElaborationUnit* m_pEU;
	ModuleServices* m_pServices;
};