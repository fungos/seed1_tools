#ifndef __STRING_H__
#define __STRING_H__

#include "defines.h"

class String
{
	public:
		String(const char *name, const char *ptr);
		~String();

		const char *GetName() const
		{
			return pName;
		}

		u32 GetLength() const
		{
			return iLength;
		}

		const char *GetText() const
		{
			return pPtr;
		}

		void SetLength(u32 len)
		{
			this->iLength = len;
		}

	private:
		String(const String &);
		const String &operator =(const String &);

		const char *pName;
		const char *pPtr;
		u32 		iLength;
		u32 		iUTF8Length;
};

#endif // __STRING_H__
