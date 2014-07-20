#include <vector>


template<class T>
class xm_stack
{
private:
	std::vector<T> coll;
public:
	typename std::vector<T>::size_type size()const{
		return coll.size();
	}
	bool empty()const{
		return coll.empty();
	}
	void push(const T& elem){
		coll.push_back(elem);
	}
	T pop(){
		if( coll.empty() )
			throw exception("xm_stack empty when pop");
		T elem( coll.back() );
		coll.pop_back();
		return elem;
	}
	T& top(){
		if( coll.empty() )
			throw exception("xm_stack empty when top");
		return coll.back();
	}
	//~ and the rest is mine
	typedef std::vector<T>::difference_type difference_type;
	difference_type top_pos(){
		if( coll.empty() )
			throw exception("xm_stack empty when top_pos");
		return coll.size()-1;
	}
	//~ for debugging
	const std::vector<T>& getContainer()const{
		return coll;
	}
	T& at(int index){	//~ may throw
		return coll.at(index);
	}
};