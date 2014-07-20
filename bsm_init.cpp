

#include "bsm_processor.h"

using namespace std;


void 
MyBSM::InitInstru()
{
	insfuncs.insert(make_pair<int,XProc>(INSTR_LABEL,	label_noop));
	insfuncs.insert(make_pair<int,XProc>(INSTR_LVALUE,	lvalue_proc));
	insfuncs.insert(make_pair<int,XProc>(INSTR_RVALUE,	rvalue_proc));
	insfuncs.insert(make_pair<int,XProc>(INSTR_PUSH,	push_imd));
	insfuncs.insert(make_pair<int,XProc>(INSTR_PUSHWAX,	pushwax));
	insfuncs.insert(make_pair<int,XProc>(INSTR_PUSHWBX,	pushwbx));
	insfuncs.insert(make_pair<int,XProc>(INSTR_POPWAX,	popwax));
	insfuncs.insert(make_pair<int,XProc>(INSTR_POPWBX,	popwbx));
	insfuncs.insert(make_pair<int,XProc>(INSTR_GOFALSE,	gofalse));
	insfuncs.insert(make_pair<int,XProc>(INSTR_GOTRUE,	gotrue));
	insfuncs.insert(make_pair<int,XProc>(INSTR_GOTO,	gotowhatever));

	//~*****//////////////////////////////////////
	insfuncs.insert(make_pair<int,XProc>(INSTR_ASSIGN,	assign_to));
	//~双目BEGIN
	//~
	//~ +-* mod% div/
	insfuncs.insert(make_pair<int,XProc>(INSTR_PLUS,	plus));
	insfuncs.insert(make_pair<int,XProc>(INSTR_SUBMIT,	submit));
	insfuncs.insert(make_pair<int,XProc>(INSTR_MULTIPLE,multiple));
	insfuncs.insert(make_pair<int,XProc>(INSTR_DIV,		divide));
	insfuncs.insert(make_pair<int,XProc>(INSTR_KDIV,	kdivide));
	insfuncs.insert(make_pair<int,XProc>(INSTR_MOD,		modulus));
	insfuncs.insert(make_pair<int,XProc>(INSTR_KMOD,	kmodulus));

	//~ unary
	insfuncs.insert(make_pair<int,XProc>(INSTR_UNARY_PLUS,unary_plus));
	insfuncs.insert(make_pair<int,XProc>(INSTR_UNARY_SUBMIT,unary_submit));

	insfuncs.insert(make_pair<int,XProc>(INSTR_NOT_EQUAL,		not_equal));
	insfuncs.insert(make_pair<int,XProc>(INSTR_LESS_EQUAL,		less_equal));
	insfuncs.insert(make_pair<int,XProc>(INSTR_LESS,			less_than));
	insfuncs.insert(make_pair<int,XProc>(INSTR_GREATER_EQUAL,	greater_equal));
	insfuncs.insert(make_pair<int,XProc>(INSTR_EQUAL,			equal_to));
	insfuncs.insert(make_pair<int,XProc>(INSTR_GREATER,			greater_than));

	//~ 逻辑运算的结果只限于0和1
	insfuncs.insert(make_pair<int,XProc>(INSTR_OR,		or_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_AND,		and_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_XOR,		xor_));

	//~单目BEGIN
	insfuncs.insert(make_pair<int,XProc>(INSTR_NOT,		not_));
	//~单目END

		//~注释行不处理嘛
	insfuncs.insert(make_pair<int,XProc>(INSTR_COMMENT,	commenting));

		//Feb19.2007
	insfuncs.insert(make_pair<int,XProc>(INSTR_XDIM,	xdim));
	insfuncs.insert(make_pair<int,XProc>(INSTR_MOVWAX,	movwax));
	insfuncs.insert(make_pair<int,XProc>(INSTR_ZEROWAX,	zerowax));
	insfuncs.insert(make_pair<int,XProc>(INSTR_ADJUST_RVALUE,adjustrvalue));
	insfuncs.insert(make_pair<int,XProc>(INSTR_ADJUST_LVALUE,adjustlvalue));

		//~ Feb28/Mar01.2oo7
	insfuncs.insert(make_pair<int,XProc>(INSTR_CALL,call_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_LOCA_RIGHT_V,locarightv_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_LOCA_LEFT_V,localeftv_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_DECSTACK,decstack_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_RETURN,return_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_PRESTACK,prestack_));
	insfuncs.insert(make_pair<int,XProc>(INSTR_LABELFUNC,labelfunc_));
	
	//~ check check , redundant checkin~~~~~~~~~~~~~~~~~~~~~
#ifndef NDEBUG
	InsFuncs::const_iterator p1,p2;
	for( p1 = insfuncs.begin(); p1 != insfuncs.end(); ++p1 )
		for( p2 = insfuncs.begin();p2!=insfuncs.end();++p2)
			if( p1 != p2 && p1->second == p2->second )
				throw exception("initilization of insfuncs failed");
#endif
};