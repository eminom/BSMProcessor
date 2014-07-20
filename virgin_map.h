
#ifndef VIRGIN_MAP_
#define VIRGIN_MAP_

#include <utility>
#include <map>

template <class KEY_TYPE,class VALUE_TYPE>
class safe_map:public std::map<KEY_TYPE,VALUE_TYPE>
{
	typedef std::map<KEY_TYPE,VALUE_TYPE> baseClass;
public:
	void insert(std::pair<KEY_TYPE,VALUE_TYPE>&elem)
	{
		baseClass::iterator pos = find(elem.first);
		if( pos != end() )
			throw exception("not a virgin key");
		baseClass::insert(elem);
	}
private:
	VALUE_TYPE operator[](KEY_TYPE key)const{
		baseClass::const_iterator pos=find(key);
		if( pos != end() )
			return pos->second;
		return VALUE_TYPE();
	}
};

#endif