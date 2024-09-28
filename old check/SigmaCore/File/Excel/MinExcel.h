#ifndef _MIN_EXCEL_H__
#define _MIN_EXCEL_H__

#pragma once

#include <string>
#include <boost/tr1/memory.hpp>
#include <boost/lexical_cast.hpp>
//#include "BasicExcel.hpp"
#include "../../String/StringFormat.h"
#include "../../RanThirdParty/LibXl/include_cpp/libxl.h"
#include "../../log/LogMan.h"

namespace sc
{

	class Excel
	{
	public:
		//! FileName : 파일명 / 경로 와 .xls 필요없음
		//! SaveFileName : 저장될 파일명(경로포함)
		//! SheetNumber : Excel 파일 Sheet 개수 0 이면 기본값 3 이됨
		//! bAutoName : 파일이름에 날짜시간을 붙일것인지 결정
		Excel(const char* FileName, std::string& SaveFileName, WORD SheetNumber, bool bAutoName);
		Excel(const std::string& FileName, std::string& SaveFileName, WORD SheetNumber, bool bAutoName);
		Excel(const std::string& FileName, std::string& PathString, std::string& SaveFileName, WORD SheetNumber, bool bAutoName);
		Excel();
		~Excel();

	protected:
		//std::tr1::shared_ptr<YExcel::BasicExcel> m_spXls;
		libxl::Book* m_pXls;
		//! 주의! Unicode 가 아니면 일부 한글로 된 폴더에 문제가 발생함
		std::wstring m_FileName;
		bool m_bSave;
		int m_Row; //! 현재 Row
		int m_Col; //! 현재 Column
		int m_Sheet; //! 현재 Sheet

	protected:
		void Init(const std::string& strPath, bool bAutoName, WORD SheetNumber);
		void CreateBook();
		void ErrorMessageBox() const;

	public:
		bool Load(const std::wstring& FileName);
		bool Load(const std::string& FileName);
		bool Save();

		void IncRowResetColumn()
		{
			IncRow();
			ResetColumn();
		}

		void IncRow() { ++m_Row; }
		void ResetColumn() { m_Col = 0; }
		void SetActiveSheet(int Sheet);

		//void SetCell(int row, int col, int val);
		//void SetCell(int row, int col, double val);
		template <typename T>
		void SetCell(int row, int col, T val)
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(0);
			if (pSheet)
			{
				if (!pSheet->writeNum(row, col, static_cast<double> (val)));
				{
					sc::writeLogInfo(sc::string::format("%1% Sheet%2%:|row,col:%3% %4%", m_pXls->errorMessage(), pSheet->name(), row, col));
					//ErrorMessageBox();
				}
			}
		}

		template <>
		void SetCell(int row, int col, const char* szStr)
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(0);
			if (pSheet && szStr)
				if (!pSheet->writeStr(row, col, szStr))
				{
					sc::writeLogInfo(sc::string::format("%1% Sheet%2%:|row,col:%3% %4%", m_pXls->errorMessage(), pSheet->name(), row, col));
					//ErrorMessageBox();
				}
		}

		template <>
		void SetCell(int row, int col, const wchar_t* str)
		{
			if (str)
			{
				std::wstring Temp(str);
				std::string AnsiStr = sc::string::unicodeToAnsi(Temp);
				SetCell(row, col, AnsiStr.c_str());
			}
		}

		template <>
		void SetCell(int row, int col, const std::string& str)
		{
			SetCell(row, col, str.c_str());
		}

		template <>
		void SetCell(int row, int col, const std::wstring& str)
		{
			std::string Temp = sc::string::unicodeToAnsi(str);
			SetCell(row, col, Temp.c_str());
		}

		template <typename T>
		void SetCell(int Sheet, int row, int col, T val)
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
			if (pSheet)
				if (!pSheet->writeNum(row, col, static_cast<double> (val)))
				{
					sc::writeLogInfo(sc::string::format("%1% Sheet%2%:|row,col:%3% %4%", m_pXls->errorMessage(), pSheet->name(), row, col));
					//ErrorMessageBox();
				}
		}

		//void SetCell(int Sheet, int row, int col, unsigned short val);
		//void SetCell(int Sheet, int row, int col, int val);
		template <>
		void SetCell(int Sheet, int row, int col, float val)
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
			if (pSheet)
				if (!pSheet->writeNum(row, col, val))
				{
					sc::writeLogInfo(sc::string::format("%1% Sheet%2%:|row,col:%3% %4%", m_pXls->errorMessage(), pSheet->name(), row, col));
					//ErrorMessageBox();
				}
		}
		//void SetCell(int Sheet, int row, int col, double val);
		//template <>
		void SetCell(int Sheet, int row, int col, const char* szStr)
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
			if (pSheet)
				if (!pSheet->writeStr(row, col, szStr))
				{
					sc::writeLogInfo(sc::string::format("%1% Sheet%2%:|row,col:%3% %4%", m_pXls->errorMessage(), pSheet->name(), row, col));
					//ErrorMessageBox();
				}
		}

		//template <>
		void SetCell(int Sheet, int row, int col, const wchar_t* str)
		{
			std::wstring Temp(str);
			std::string AnsiStr = sc::string::unicodeToAnsi(Temp);
			SetCell(Sheet, row, col, AnsiStr.c_str());
		}

		//template <>
		void SetCell(int Sheet, int row, int col, const std::string& str)
		{
			SetCell(Sheet, row, col, str.c_str());
		}

		//template <>
		void SetCell(int Sheet, int row, int col, const std::wstring& str)
		{
			std::string Temp = sc::string::unicodeToAnsi(str);
			SetCell(Sheet, row, col, Temp.c_str());
		}

		///< Get an integer value. Returns 0 if cell does not contain an integer.
		template <typename T>
		bool GetValue(int Sheet, int row, int col, T& Value) const
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
			if (pSheet)
			{
				double Temp = pSheet->readNum(row, col);
				Value = static_cast<T> (Temp);
				return true;
			}
			else
			{
				return false;
			}
		}
		bool GetValueStrAndInteger(int Sheet, int row, int col, std::string& Value) const
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
			if (pSheet)
			{
				const CHAR* szStr = pSheet->readStr(row, col);
				if (szStr)
				{
					Value = szStr;
					return true;
				}
				else
				{
					char Temp[255];
					int ni = (int)pSheet->readNum(row, col);
					Value = itoa(ni,Temp,10);
					return true;
				}
			}
			else
			{
				sc::writeLogInfo(sc::string::format("%1% Sheet%2%:|row,col:%3% %4%", m_pXls->errorMessage(), pSheet->name(), row, col));
				//ErrorMessageBox();
				return false;
			}
		}
		//bool GetValue(int Sheet, int row, int col, short& Value) const;
		//bool GetValue(int Sheet, int row, int col, int& Value) const;
		//bool GetValue(int Sheet, int row, int col, unsigned short& Value) const;
		//bool GetValue(int Sheet, int row, int col, unsigned long& Value) const;

		///< Get a double value. Returns 0.0 if cell does not contain a double.
		//bool GetValue(int Sheet, int row, int col, double& Value) const;
		//bool GetValue(int Sheet, int row, int col, float& Value) const;

		//bool GetValue(int Sheet, int row, int col, __time64_t& Value) const;
		//template <>
		bool GetValue(int Sheet, int row, int col, bool& Value) const
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
			if (pSheet)
			{
				double Temp = pSheet->readNum(row, col);
				Value = Temp == 0 ? false : true;
				return true;
			}
			else
			{
				return false;
			}
		}

		///< Get an ANSI string. Returns 0 if cell does not contain an ANSI string.
		//template <>
		bool GetValue(int Sheet, int row, int col, std::string& Value) const
		{
			libxl::Sheet* pSheet = m_pXls->getSheet(Sheet);
			if (pSheet)
			{
				const CHAR* szStr = pSheet->readStr(row, col);
				if (szStr)
				{
					Value = szStr;
					return true;
				}
				else
				{
					//MessageBox(NULL, szStr, szStr, MB_OK);
					sc::writeLogInfo(sc::string::format("GetValue, %1% Sheet%2%:|row,col:%3% %4%", m_pXls->errorMessage(), pSheet->name(), row, col));
					// ErrorMessageBox();
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		///< Get an Unicode string. Returns 0 if cell does not contain an Unicode string.
		//template <>
		bool GetValue(int Sheet, int row, int col, std::wstring& Value) const
		{
			std::string Temp;
			if (GetValue(Sheet, row, col, Temp))
			{
				Value = sc::string::ansiToUnicode(Temp);
			}
			else
			{
				return false;
			}
		}

		bool RenameWorksheet(int WorkSheet, const std::wstring& WorkSheetName);
		bool RenameWorksheet(int WorkSheet, const std::string& WorkSheetName);
		void AddWorksheet(std::string& SheetName);

		///< Total number of Excel worksheets in current Excel workbook.
		int WorksheetCount();

		///< Total number of rows in current Excel worksheet.
		///< If Error -1
		int WorkSheetTotalRow(int SheetIndex);

		///< Total number of columns in current Excel worksheet.
		///< If Error -1
		int WorkSheetTotalCols(int SheetIndex);
	};

}; // namespace CORE_COMMON
#endif // _MIN_EXCEL_H__

