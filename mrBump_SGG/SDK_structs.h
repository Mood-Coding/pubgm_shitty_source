#pragma once
#include "pch.h"

#ifndef _SDK_STRUCTS_H_
#define _SDK_STRUCTS_H_

namespace SDK
{
	/*template<class t>*/
	/*struct TArray
	{
		friend struct FString;

	public:
		inline TArray()
		{
			Data = nullptr;
			Count = Max = 0;
		};

		inline int Num() const
		{
			return Count;
		};

		inline T& operator[](int i)
		{
			return Data[i];
		};

		inline const T& operator[](int i) const
		{
			return Data[i];
		};

		inline bool IsValidIndex(int i) const
		{
			return i < Num();
		}

	private:
		T* Data;
		int32_t Count;
		int32_t Max;
	};*/

	/*struct FString : private TArray<unsigned short>
	{
		inline FString()
		{
		}

		FString(const std::wstring s)
		{
			Max = Count = !s.empty() ? (s.length() * 2) + 1 : 0;
			if (Count)
			{
				Data = (unsigned short*)(s.data());
			}
		}

		FString(const wchar_t* s) : FString(std::wstring(s)) {
		}

		FString(const wchar_t* s, int len) : FString(std::wstring(s, s + len)) {
		}

		FString(const std::string s) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring ws = converter.from_bytes(s);

			Max = Count = !ws.empty() ? (ws.length() * 2) + 1 : 0;
			if (Count)
			{
				Data = (unsigned short*)(ws.data());
			}
		}

		FString(const char* s) : FString(std::string(s)) {
		}

		FString(const char* s, int len) : FString(std::string(s, s + len)) {
		}

		inline bool IsValid() const
		{
			return Data != nullptr;
		}

		inline const wchar_t* ToWString() const
		{
			wchar_t* output = new wchar_t[Count + 1];

			for (int i = 0; i < Count; i++) {
				const char16_t uc = Data[i];
				if (uc - 0xd800u >= 2048u) {
					output[i] = uc;
				}
				else {
					if ((uc & 0xfffffc00) == 0xd800 && (uc & 0xfffffc00) == 0xdc00)
						output[i] = (uc << 10) + Data[i] - 0x35fdc00;
					else
						output[i] = L'?';
				}
			}

			output[Count] = 0;
			return output;
		}

		inline const char* ToString() const
		{
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			return convert.to_bytes(std::u16string(Data, Data + Count)).c_str();
		}
	};*/

	// ScriptStruct CoreUObject.Vector
	// 0x000C
	struct FVector
	{
		float X; // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float Y; // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float Z; // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	};

	struct FVector2D
	{
		inline FVector2D() : X(0), Y(0) { }

		inline FVector2D(float x, float y) : X(x), Y(y) { }

		float X; // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float Y; // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	};

}



#endif // !_SDK_STRUCTS_H_
