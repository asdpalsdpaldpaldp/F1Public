#pragma once

#include "SDK.h"

class CDumper
{
	std::fstream m_file;

	std::string TypeToString(SendPropType type)
	{
		switch(type)
		{
		case DPT_Int:
			return "INT";
		case DPT_Float:
			return "FLOAT";
		case DPT_Vector:
			return "VECTOR3";
		case DPT_VectorXY:
			return "VECTOR2";
		case DPT_String:
			return "STRING";
		case DPT_Array:
			return "ARRAY";
		case DPT_DataTable:
			return "TABLE";
		}

		return "UNKNOWN";
	}

public:
	CDumper()
	{
		m_file.open("netDump.txt", std::ios::out | std::ios::trunc);
	}

	~CDumper()
	{
		m_file.close();
	}

	void SaveDump()
	{
		ClientClass *pList = gInts->Client->GetAllClasses();

		while(pList)
		{
			DumpTable(pList->Table, 0);

			pList = pList->pNextClass;
		}
	}

	void DumpTable(RecvTable *pTable, int iLevel)
	{
		if(!pTable)
			return;

		for(int j = 0; j < iLevel; j++)
			m_file << " ";

		m_file << pTable->GetName() << std::endl;

		iLevel += 2;

		for(int i = 0; i < pTable->GetNumProps(); ++i)
		{
			RecvProp *pProp = pTable->GetProp(i);

			if(!pProp)
				continue;

			if(isdigit(pProp->GetName()[0]))
				continue;

			if(pProp->GetDataTable())
			{
				DumpTable(pProp->GetDataTable(), iLevel + 1);
			}

			for(int j = 0; j < iLevel; j++)
				m_file << " ";

			m_file << pProp->GetName() << " : 0x" << std::hex << pProp->GetOffset() << " [" << TypeToString(pProp->GetType()) << "]" << std::endl;
		}

		if(iLevel == 2)
			m_file << std::endl;
	}
};
