
//~ eminemextra@163.com
#include "bsm_processor.h"
#include "conio.h"
using namespace std;

class func_name_match:public 
	binary_function<pair<string,int>,pair<string,int>,bool>
{
public:
	result_type operator()
		(const pair<string,int>&one,
		const pair<string,int>&two)const{
		return std::operator==(one.first,two.first);
	}
};

int
MyBSM::FuncNameMan::addFunc(const char *new_func)
{
	FuncNameList::iterator pos = 
		find_if(funcNameList.begin(),
				funcNameList.end(),
			bind2nd(func_name_match(),
				make_pair<string,int>(string(new_func),-1))
		);

	if( pos != funcNameList.end() ){
		return distance( funcNameList.begin(),pos);
	}
	funcNameList.push_back( make_pair<string,int>(new_func,-1) );
	return funcNameList.size()-1;
}

const 
MyBSM::FuncNameList&
MyBSM::FuncNameMan::getFuncNameList()const{
	return funcNameList;
}

void
MyBSM::FuncNameMan::markFunc(const char *old_func,int ins_addr)
{
	int sz = funcNameList.size();
	FuncNameList::iterator pos = 
		find_if(funcNameList.begin(),
		funcNameList.end(),
		bind2nd(func_name_match(),
		make_pair<string,int>(string(old_func),-1)));
	if( funcNameList.end() == pos ){
		char szMsg[BUFSIZ];
		sprintf(szMsg,"unknown func name \"%s\"",old_func);
		throw exception(szMsg);
	}
	pos->second = ins_addr;
}

int
MyBSM::FuncNameMan::getFuncAddr(const string& func_name)const
{
	FuncNameList::const_iterator pos = 
		find_if(funcNameList.begin(),
		funcNameList.end(),
		bind2nd(func_name_match(),
		make_pair<string,int>(func_name,-1)));
	if( funcNameList.end() == pos )
		throw exception( string("unknow func name"+func_name).c_str());
	return pos->second;
}

int
MyBSM::FuncNameMan::getFuncAddr(int index)const
{	return funcNameList.at(index).second;}


void
MyBSM::FuncNameMan::show()const
{
	cout<<"func name show {"<<endl;
	vector<pair<string,int> >::const_iterator pos;
	for(pos = funcNameList.begin();pos!=funcNameList.end();++pos){
		cout<<"\""<<pos->first<<"\""<<" at "<<pos->second<<endl;
	}
	cout<<"}"<<endl;
}