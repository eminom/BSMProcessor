#include "bsm_run_component.h"
#include "bsm_processor.h"
using namespace std;


void MyBSM::label_noop(int){}

void MyBSM::lvalue_proc(int paddr)
{	runstate.wstack.push(make_pair<int,int>(paddr,POISON_GLOBAL));}

void MyBSM::rvalue_proc(int paddr)
{	//~ȡֵ�����Ӻ���
		//~ ������ѹ����ǵ�ַ,�������ջ������һ�����ֵ
	runstate.wstack.push(make_pair<int,int>(paddr,POISON_NONE));
}

void MyBSM::push_imd(int immediate)
{	runstate.wstack.push(make_pair<int,int>(immediate,POISON_NONE));}

void MyBSM::pushwax(int)
{	runstate.wstack.push(make_pair<int,int>(runstate.wax,POISON_NONE));}

void MyBSM::pushwbx(int)
{	runstate.wstack.push(make_pair<int,int>(runstate.wbx,POISON_NONE));}

void MyBSM::popwax(int)
{
	if( runstate.wstack.empty() )
		throw exception("attempt to popwax from empty stack");
	runstate.wax = runstate.wstack.top().first;
	runstate.wstack.pop();
	if( is_debug_on(DBUG_SHOW) )
		cout<<" wax = "<<runstate.wax<<endl;
}

void MyBSM::popwbx(int)
{
	if( runstate.wstack.empty() )
		throw exception("attempt to popwbx from empty stack");
	runstate.wbx = runstate.wstack.top().first;
	runstate.wstack.pop();
}

void MyBSM::gotowhatever(int nLabel)
{
	int next_ip = trans.at(nLabel);
	if( next_ip < 0 )
		throw exception("gofalse to an unknown label");
	runstate.wip = trans.at(nLabel);
}

void MyBSM::gofalse(int nLabel)
{
	//~ this may be slow,
	//~ but it checks the stacker source for good
	if( !runstate.wax ){
		int next_ip = trans.at(nLabel);
		if( next_ip < 0 )
			throw exception("gofalse to an unknown label");
		runstate.wip = trans.at(nLabel);
	}
}

void MyBSM::gotrue(int nLabel)
{
	if( runstate.wax ){
		int next_ip = trans.at(nLabel);
		if( next_ip < 0 )
			throw exception("gotrue to an unknown label");
		runstate.wip = trans.at(nLabel);
	}
}

void MyBSM::assign_to(int)
{
	//~ ��wax��ֵ����ջ���洢�ĵ�ֵַ���Ǹ�����!
	if( runstate.wstack.empty() )
		throw exception("stack empty for assignment");

	//~ ����assign_to����Ϊ:��������������ɸ�ֵ
	switch( runstate.wstack.top().second )
	{
	case POISON_GLOBAL:
		do{
			int index = runstate.wstack.top().first;
			if( index < 0 || index >= vars.size() )
				throw exception("invalid lvalue for assignment");
			runstate.wstack.pop();	//~ ��address����,�Ѿ��ù�����
				//~ logicor_wrapped ��ֵ����͵���wax
			vars.at(index).second = runstate.wax;//~may throw
			//~
			if( is_debug_on(DBUG_STEP) )
				cout<<"    "<<vars.at(index).first<<" turns "<<runstate.wax<<endl;
		}while(0);
		break;
	case POISON_STACK:
		do{	//~ reinterpretin'...
			int _offset = runstate.wstack.top().first;
			int pt = posRefer.top();
			pt+=_offset;
			runstate.wstack.pop();	//~
			runstate.wstack.at(pt).first = runstate.wax;	//~ ������ ������ջ�ϵ���
			
					//~�ѵ�ַ������
		}while(0);
		break;
	default:
		throw exception("error type code for runtime-stack assignment operation");
		break;
	}
}

void MyBSM::unary_plus(int)
{}

void MyBSM::plus(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for +");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~	
	runstate.wstack.push(make_pair<int,int>(val1+val2,POISON_NONE));
}

void MyBSM::unary_submit(int)
{
	if( runstate.wstack.empty() )
		throw exception("stack empty for unary -");
	runstate.wstack.top().first = -runstate.wstack.top().first;
}

void MyBSM::submit(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for -");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push(make_pair<int,int>(val1-val2,POISON_NONE));
}

void MyBSM::multiple(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for *");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push(make_pair<int,int>( val1 * val2 ,POISON_NONE) );
}

void MyBSM::kdivide(int)
{	return divide(0);}

void MyBSM::divide(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for /");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push(make_pair<int,int>(val1 / val2,POISON_NONE) );	//~ may throw
}

void MyBSM::kmodulus(int)
{	return modulus(0);}

void MyBSM::modulus(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for %");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();

	//~
	runstate.wstack.push(make_pair<int,int>( val1 % val2,POISON_NONE) );
}

void MyBSM::not_equal(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for <>");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	if( is_debug_on(DBUG_STEP) )
		cout<<"   "<<val1<<"<>"<<val2<<" = "<<(val1!=val2)<<endl;
	runstate.wstack.push( make_pair<int,int>(val1 != val2 ,POISON_NONE));
}


void MyBSM::less_than(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for <");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	if( is_debug_on(DBUG_STEP) )
		cout<<"  ->>  "<<val1<<"<"<<val2<<" = "<<(val1<val2)<<endl;
	runstate.wstack.push( make_pair<int,int>(val1 < val2,POISON_NONE) );
}

void MyBSM::less_equal(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for <=");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	if( is_debug_on(DBUG_STEP) )
		cout<<"  ->>  "<<val1<<"<="<<val2<<" = "<<(val1<=val2)<<endl;
	runstate.wstack.push(make_pair<int,int>( val1 <= val2 ,POISON_NONE));
}

void MyBSM::equal_to(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for =");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	if( is_debug_on(DBUG_STEP) )
		cout<<"   ->> "<<val1<<" equal to "<< val2 <<" = "<<(val1==val2)<<endl;
	runstate.wstack.push( make_pair<int,int>(val1 == val2 ,POISON_NONE));
}

void MyBSM::greater_equal(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for >=");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push( make_pair<int,int>(val1 >= val2,POISON_NONE) );
}

void MyBSM::greater_than(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for >");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push( make_pair<int,int>( val1 > val2 , POISON_NONE));
}

void MyBSM::or_(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for or");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push(make_pair<int,int>( val1 || val2 ,POISON_NONE));
}

void MyBSM::and_(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for and");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push( make_pair<int,int>(val1 && val2 ,POISON_NONE));
}

void MyBSM::xor_(int)
{
	if( runstate.wstack.size() < 2 )
		throw exception("stack corrupted for xor");
	int val2 = runstate.wstack.top().first;
	runstate.wstack.pop();
	int val1 = runstate.wstack.top().first;
	runstate.wstack.pop();
	//~
	runstate.wstack.push( make_pair<int,int>( (val1&&!val2)||(!val1&&val2)  , POISON_NONE));
}

void MyBSM::not_(int)
{
	if( runstate.wstack.empty() )
		throw exception("stack corrupted for not");
	//~ ��ջ����
	runstate.wstack.top().first =
	 !runstate.wstack.top().first;
}

void MyBSM::commenting(int){}

void MyBSM::xdim(int)
{}	//~ ����ʱ��������,ֻ������ʱ����

void MyBSM::movwax(int val)
{	runstate.wax =  val;}

void MyBSM::zerowax(int)
{	runstate.wax = 0; }

	//~	�����¼��������ַ
	//~ ���Ұ�ֵ������ջ��
void MyBSM::adjustrvalue(int)
{
	if( runstate.wstack.empty() )
		throw exception("stack corrupted for adjust rvalue");
	int paddr = runstate.wax + runstate.wstack.top().first;
	runstate.wstack.top().first = vars.at(paddr).second;
}

	//~ ���÷���ջ����ƫ��
void MyBSM::adjustlvalue(int)
{
	if( runstate.wstack.empty() )
		throw exception("stack corrutped for adjust lvalue");
	runstate.wstack.top().first += runstate.wax;
}

//~ New On Feb28/Mar01.2oo7
void MyBSM::call_(int func_name_index)
{
	//~ ��ס wip�Ѿ���������
	runstate.wstack.push( make_pair<int,int>(runstate.wip,POISON_INS_ADDR) );
	posRefer.push( runstate.wstack.top_pos() );
	runstate.wip = funcNameMan.getFuncAddr( func_name_index );
	if( runstate.wip < 0 || runstate.wip >= codes.size() )
		throw exception("func call to an unknown address");
}

void MyBSM::locarightv_(int _offset)
{
	int pt = posRefer.top();
	pt+=_offset;
	pair<int,int> clone_one = runstate.wstack.at(pt);
	runstate.wstack.push( clone_one );	
}

void MyBSM::localeftv_(int _offset)
{
	runstate.wstack.push( make_pair<int,int>( _offset, POISON_STACK ) );
}

void MyBSM::decstack_(int offcount)
{
	while( offcount-- > 0 )
		runstate.wstack.pop();
}

void MyBSM::return_(int offcount)
{
	if( POISON_INS_ADDR != runstate.wstack.top().second )
		throw exception("return addr corrupted");
	runstate.wip = runstate.wstack.pop().first;
	posRefer.pop();

	while( offcount-- > 0 )
		runstate.wstack.pop();

	//whi
}

void MyBSM::prestack_(int offcount)
{
	//~ �������Ĩ���
	while( offcount -- > 0 )
		runstate.wstack.push( make_pair<int,int>(0,POISON_NONE) );
}

void MyBSM::labelfunc_(int)
{}//~�Բ���Ӧ��-ʯ�ɽ�


