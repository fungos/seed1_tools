#ifndef __GROUPMANAGER_H__
#define __GROUPMANAGER_H__

#include <map>
#include "defines.h"
#include "group.h"

using namespace std;

typedef map<const char *, Group *, StringComparator>::iterator GroupMapIterator;

class GroupManager
{
	private:
		map<const char *, Group *, StringComparator> pGroups;

	public:
		GroupManager();
		~GroupManager();

		void Add(Group *g);
		Group *Get(const char *groupName);

		GroupMapIterator GetFirst();
		GroupMapIterator GetLast();
};

#endif
