
//~ eminem7409@sina.com
#include <iostream>
#include <string>
#include <fstream>

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <bitset>
#include <functional>
#include <algorithm>
#include <cassert>

#include "virgin_map.h"
#include "xm_stack.h"

enum{
	DBUG_STEP,
	DBUG_SHOW,
	DBUG_COMMENT,
	DBUG_VARSATEND,
	DBUG_COUNT
};

enum{
	INSTR_LABEL,
	INSTR_LVALUE,
	INSTR_RVALUE,
	INSTR_PUSH,
	INSTR_PUSHWAX,
	INSTR_PUSHWBX,
	INSTR_POPWAX,
	INSTR_POPWBX,
	INSTR_GOFALSE,
	INSTR_GOTRUE,
	INSTR_GOTO,

	//~
	INSTR_ASSIGN,

	//
	INSTR_PLUS,
	INSTR_SUBMIT,
	INSTR_MULTIPLE,
	INSTR_DIV,
	INSTR_MOD,
	INSTR_KDIV,	//~ keyword div
	INSTR_KMOD,	//~ keyword mod

	//~
	INSTR_NOT_EQUAL,
	INSTR_LESS_EQUAL,
	INSTR_LESS,
	INSTR_GREATER_EQUAL,
	INSTR_GREATER,
	INSTR_EQUAL,

	//~
	INSTR_OR,
	INSTR_AND,
	INSTR_XOR,
	INSTR_NOT,
	//~
	INSTR_COMMENT,

	//~ new on Feb19.2oo7
	INSTR_XDIM,
	INSTR_MOVWAX,
	INSTR_ZEROWAX,
	INSTR_ADJUST_RVALUE,
	INSTR_ADJUST_LVALUE,

	INSTR_UNARY_PLUS,
	INSTR_UNARY_SUBMIT,

	//~ new on Feb28/March01.2oo7
	INSTR_CALL,
	INSTR_LOCA_RIGHT_V,
	INSTR_LOCA_LEFT_V,
	INSTR_DECSTACK,
	INSTR_RETURN,
	INSTR_PRESTACK,
	INSTR_LABELFUNC,

	LAST_INSTR_COUNT
};

class MyBSM
{
private:
	class takeOn:public std::unary_function<int,void>
	{
	public:
		result_type operator()(argument_type one){
			rbsm.set_debug_mode(one);
		}
	takeOn(MyBSM &rbsm_):rbsm(rbsm_){}
	private:
		MyBSM& rbsm;
	};
public:
	typedef std::set<int> DSets;
	MyBSM(const char *inputName,const DSets& dopts);
	void Run();
private:
	void set_debug_mode(int opt);
	bool is_debug_on(int opt)const;

private:
	std::bitset<DBUG_COUNT> dbglist;
	//~ codess
	void ReadItAll(const char *inputName);
	//~ transf
	void MakeTrans();
	//~ instructions;
	void InitInstru();
	//~ varis
	int installVar(const std::string varname);
	int getVarAddress(const std::string&)const;			//~ -1 for error

	//~ data
	enum{
		POISON_NONE,
		POISON_GLOBAL,
		POISON_STACK,
		POISON_INS_ADDR,
	
		//~POISON_LOCAL_VAR_STO,
	};
	typedef std::vector<std::pair<std::string,std::pair<int,int> > >CodeCon;
	typedef std::vector<int>							TransCon;
	typedef std::vector<std::pair<std::string,int> >	VarsCon;
	typedef xm_stack<std::pair<int,int> >				RTStack;//~ runtime stack 
	typedef std::vector<std::pair<std::string,int> >	FuncNameList;//~第二个是ins_addr
	typedef void (MyBSM::*XProc)(int);
	typedef safe_map<int,XProc>							InsFuncs;
	typedef xm_stack<RTStack::difference_type>			RunTimePosRefer;

	enum{T_INIT_SIZE=1024,T_EXPAND_SZ=512};

	CodeCon			codes;
	TransCon		trans;		//~ label to wip entry of(for) codes
	VarsCon			vars;
	InsFuncs		insfuncs;
	RunTimePosRefer	posRefer;	//~ new on Feb28/Mar01 :function supportin'

	class FuncNameMan{
	private:
		FuncNameList funcNameList;
	public:
		const FuncNameList& getFuncNameList()const;
		int addFunc(const char *new_func);	//~ return value is the position 
											//~of the name in the list
		void markFunc(const char *old_func,int ins_addr);
		int getFuncAddr(const std::string&)const;
		int getFuncAddr(int)const;
		void show()const;
	};
	FuncNameMan	funcNameMan;

	struct RunState
	{
		RTStack wstack;
		int wax,wbx;		//~ register;
		int wip;
	}runstate;
private:
	//~
	void label_noop	(int);
	void lvalue_proc(int);
	void rvalue_proc(int);
	void push_imd	(int);
	void pushwax	(int);
	void pushwbx	(int);
	void popwax		(int);
	void popwbx		(int);

	void gofalse	(int);
	void gotrue		(int);
	void gotowhatever(int);

	void assign_to	(int);

	//~
	//~ +-* mod% div/
	void plus		(int);
	void unary_plus	(int);	//~ no-op
	void submit		(int);
	void unary_submit(int);
	void multiple	(int);
	void modulus	(int);
	void divide		(int);
	void kdivide	(int);
	void kmodulus	(int);

	//~ compare
	void not_equal	(int);
	void less_than	(int);
	void less_equal	(int);
	void equal_to	(int);
	void greater_equal(int);
	void greater_than(int);
		//~ logical
	void or_		(int);
	void and_		(int);
	void xor_		(int);
	void not_		(int);
		//~
	void commenting	(int);
		//~
	void xdim		(int);
	void movwax		(int);
	void zerowax	(int);
	void adjustrvalue(int);
	void adjustlvalue(int);

		//~new on Feb28/Mar01.2oo7
	void call_		(int);
	void locarightv_(int);
	void localeftv_	(int);
	void decstack_	(int);
	void return_	(int);
	void prestack_	(int);
	void labelfunc_	(int);
};

