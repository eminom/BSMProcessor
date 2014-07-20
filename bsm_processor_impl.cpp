
//~ eminemextra@163.com
#include "bsm_processor.h"
#include "conio.h"
using namespace std;

void
MyBSM::
set_debug_mode(int opt)
{
	if( opt < 0 || opt >= dbglist.size() )
		throw exception("invalid opt for debug mode");
	dbglist.set(opt);
}

bool 
MyBSM::
is_debug_on(int opt)const{
	if( opt < 0 || opt >= dbglist.size() )
		throw exception("invalid opt for debug query");
	return dbglist[opt];
}

MyBSM::MyBSM(const char *inputName,const MyBSM::DSets &dopts)
	:trans(T_INIT_SIZE,-1)
{
	for_each(dopts.begin(),dopts.end(),takeOn(*this));
	if( is_debug_on(DBUG_STEP) )
		set_debug_mode(DBUG_SHOW) ;

	//~ grows up with -1
	assert(T_INIT_SIZE == trans.size() );
	InitInstru();
	ReadItAll(inputName);
	MakeTrans();
}


void MyBSM::MakeTrans()
{
	CodeCon::iterator pos;
	for(pos = codes.begin();pos!=codes.end();++pos){
		if( INSTR_LABEL == pos->second.first  ){
			int nLabel = pos->second.second;
			while( nLabel >= trans.size() )
				trans.resize( trans.size() + T_EXPAND_SZ , -1 );
			trans.at(nLabel) = distance(codes.begin(),pos);
		}
		else if( INSTR_LABELFUNC == pos->second.first){
			char func_name[BUFSIZ];
			int is = sscanf(pos->first.c_str(),"labelfunc %s",func_name);
			if( is != 1 )
				throw exception("corrupted labelfunc");
			funcNameMan.markFunc( func_name, 
				distance(codes.begin(),pos) );
		}
	}
	//~
	funcNameMan.show();
}

int 
MyBSM::
installVar(const std::string var_name)
{	
	int ri = vars.size();
	vars.push_back(pair<string,int>(var_name,0));
	return ri;
}

int 
MyBSM::
getVarAddress(const string &var_name)const
{
	VarsCon::const_iterator pos;
	for(pos = vars.begin();pos!=vars.end();++pos)
		if( pos->first == var_name )
			return distance(vars.begin(),pos);
	return -1;
}

void
MyBSM::Run()
{
	runstate.wip = 0;
	runstate.wax = -1;
	runstate.wbx = -1;
	while( !runstate.wstack.empty() )
		runstate.wstack.pop();
	const int endi = codes.size();
	while( runstate.wip < endi ){
		const int instructionz = codes.at(runstate.wip).second.first;
		const int operand = codes.at(runstate.wip).second.second;
		const string& present = codes.at(runstate.wip).first;
		if( is_debug_on(DBUG_SHOW) )
			cout<<present<<endl;
		if( is_debug_on(DBUG_STEP) )
			while(1){
				char ch = tolower(getch());
				if( 'w'==ch ){
					VarsCon::const_iterator p1;
					for(p1=vars.begin();p1!=vars.end();++p1){
						cout<<"\t"<<p1->first<<"="<<p1->second<<',';
					}
					cout<<endl;
				}
				else if('r' == ch ){
					cout<<"\twax = "<<runstate.wax
						<<", wbx = "<<runstate.wbx<<endl;
				}
				else if( 's' == ch ){
					//~ show me the stack
					const vector<pair<int,int> > &container=runstate.wstack.getContainer();
					vector<pair<int,int> >::const_iterator pt;
					for(pt=container.begin();pt!=container.end();++pt){
						cout<<pt->first;
						if( POISON_STACK == pt->second )
							cout<<"(s)";
						else if( POISON_GLOBAL == pt->second )
							cout<<"(g)";
						else if( POISON_INS_ADDR == pt->second )
							cout<<"(a)";
						cout<<",";
					}
					cout<<endl;
				}
				else if('q' == ch ){
					exit(1);	//~ program aborted.
				}
				else
					break;
			}
		++runstate.wip;
		//~
		InsFuncs::iterator pos;
		pos = insfuncs.find(instructionz);
		if( insfuncs.end() != pos ){
			if( NULL == pos->second )
				throw exception("null pointer for instruction!");
			(this->*pos->second)(operand);
		}
		else
			throw exception("instruction not found");
	}
	if( !runstate.wstack.empty() )
		throw exception("runtime-stack not balanced");
	if( !posRefer.empty() )
		throw exception("pos refer not empty");
	if( is_debug_on(DBUG_VARSATEND) ){
		VarsCon::const_iterator pos;
		for(pos = vars.begin();pos!=vars.end();++pos){
			cout<<" "<<pos->first<<" = "<<pos->second<<endl;
		}
	}
	cout<<"stacker execution done"<<endl;
	cout<<"wax = "<<runstate.wax<<endl;
}