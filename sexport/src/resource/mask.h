#ifndef __MASK_H__
#define __MASK_H__

#include "../defines.h"
#include "../iobject.h"
#include "../iresource.h"


class Exporter;


class Mask : public IResource
{
	private:
		Mask(const Mask &);
		Mask &operator=(const Mask &);

	public:
		Mask(const char *name, const char *lang);
		virtual ~Mask() {}

		virtual void SetFilename(const char *fileName);
};


#endif
