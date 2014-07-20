
#include "bsm_processor.h"
using namespace std;


namespace _InsStr2InsOrd_space
{
	struct InsStrToInsOrd{
		char *pszIns;
		int  ins;
	};

	InsStrToInsOrd _with_str[]={
		"call ",		INSTR_CALL,
		"labelfunc ",	INSTR_LABELFUNC,
	};

	InsStrToInsOrd _single[]={
			"pushwax",	INSTR_PUSHWAX,
			"pushwbx",	INSTR_PUSHWBX,
			"popwax",	INSTR_POPWAX,
			"popwbx",	INSTR_POPWBX,
			//~
			":=",		INSTR_ASSIGN,
			"+[s]",		INSTR_UNARY_PLUS,
			"+",		INSTR_PLUS,
			"-[s]",		INSTR_UNARY_SUBMIT,
			"-",		INSTR_SUBMIT,
			"*",		INSTR_MULTIPLE,
			"/",		INSTR_DIV,
			"%",		INSTR_MOD,
			"div",		INSTR_KDIV,
			"mod",		INSTR_KMOD,
			//~
			"<>",		INSTR_NOT_EQUAL,
			"=",		INSTR_EQUAL,
			"<=",		INSTR_LESS_EQUAL,
			"<",		INSTR_LESS,
			">=",		INSTR_GREATER_EQUAL,
			">",		INSTR_GREATER,
			//~ logical
			"or",		INSTR_OR,
			"and",		INSTR_AND,
			"xor",		INSTR_XOR,
			"not",		INSTR_NOT,
			//~ r-l value
			"adjustlvalue",	INSTR_ADJUST_LVALUE,
			"adjustrvalue",	INSTR_ADJUST_RVALUE,
			"zerowax",		INSTR_ZEROWAX,
	};

	InsStrToInsOrd _direct[]={
			"label ",		INSTR_LABEL,
			"push ",		INSTR_PUSH,
			"gofalse ",		INSTR_GOFALSE,
			"gotrue ",		INSTR_GOTRUE,
			"goto ",		INSTR_GOTO,
				//~ function supportin'
			"locarightv ",	INSTR_LOCA_RIGHT_V,
			"localeftv ",	INSTR_LOCA_LEFT_V,
			"return ",	INSTR_RETURN,
			"decstack ",INSTR_DECSTACK,
			"prestack ",INSTR_PRESTACK,
	};

	//~&*(#!&@*#&(*!@&#*(&!@#&*(!@&#&!@(*#*(!@*(#!@*#*!@$^@!^$(!@
	InsStrToInsOrd _indct[]={
			"lvalue ",	INSTR_LVALUE,
			"rvalue ",	INSTR_RVALUE,
	};
};

namespace _SOS = _InsStr2InsOrd_space;

void MyBSM::ReadItAll(const char *inputName)
{
	string theName(inputName);
	theName+=".stacker";
	ifstream input(theName.c_str());
	if(!input)
		throw exception("can not open input stream");
	while( !input.eof() )
	{
		char buf[BUFSIZ];
		input.getline(buf,sizeof buf);
		const string preLine(buf);
		string delim(" ");
		string::size_type pos = preLine.find_first_not_of(delim);
		if( string::npos == pos )
			continue;
		const string tLine( preLine.substr(pos) );
		if( 0 == tLine.find("//") ){
			if( is_debug_on(DBUG_COMMENT) )
				codes.push_back(make_pair<string,pair<int,int> >(tLine,make_pair<int,int>(INSTR_COMMENT,-1)));
			else
				cerr<<"dispose "<<preLine<<endl;
			continue;
		}

		//~ process the instruction in string format
		/* in all: the instrcuction line with one parameters
		 */
		//~ special care of xdim and movwax
		if( !strncmp(tLine.c_str(),"xdim ",4) ){
			char vname[BUFSIZ];
			sscanf(tLine.c_str(),"xdim %s",vname);
			const int sz = runstate.wax;
			if( sz < 0 )
				throw exception("xdim failed for wax is less than zero");
			if( getVarAddress(vname) >= 0 )
				throw exception("xdim failed");
			installVar(vname);
			for(int i=1;i<sz;i++){
				char adna[BUFSIZ];
				sprintf(adna,"%s[%d]",vname,i);
				installVar(adna);
			}
			continue;	//~ do not install this line
		}
		else if( !strncmp(tLine.c_str(),"movwax ",7) ){
			int val;
			int is =sscanf(tLine.c_str(),"movwax %d",&val);
			if( is != 1 )
				throw exception("corrupted instruction of movwax");
			runstate.wax = val;
			continue;	//~ do not install this line either
		}

		//~ translate string instructions into int vals
		bool bProcessed(false);
		int i;
		for(i=0;i<sizeof _InsStr2InsOrd_space::_single/sizeof _InsStr2InsOrd_space::_single[0];++i){
			if( !strncmp(_InsStr2InsOrd_space::_single[i].pszIns,tLine.c_str(),strlen(_InsStr2InsOrd_space::_single[i].pszIns)) ){
				codes.push_back(make_pair<string,pair<int,int> >( tLine,make_pair<int,int>(_InsStr2InsOrd_space::_single[i].ins,-1)));
				bProcessed=true;
				break;
			}
		}
		if( bProcessed )
			continue;
		/////////////////////////////////////////////////////~~~~~~~~~~~~~~~~~~~~
		for(i=0;i<sizeof(_InsStr2InsOrd_space::_direct)/sizeof(_InsStr2InsOrd_space::_direct[0]);++i){
			if( !strncmp(	_InsStr2InsOrd_space::_direct[i].pszIns, 
							tLine.c_str() ,
							strlen(_InsStr2InsOrd_space::_direct[i].pszIns))){
				int operand;
				const char *inputPattern;
				switch(_InsStr2InsOrd_space::_direct[i].ins){
				case INSTR_LABEL:
				case INSTR_GOFALSE:
				case INSTR_GOTRUE:
				case INSTR_GOTO:
					inputPattern ="%x";
					break;
				case INSTR_RETURN:
				case INSTR_DECSTACK:
				case INSTR_PRESTACK:
				case INSTR_PUSH:
					inputPattern ="%d";
					break;
				case INSTR_LOCA_RIGHT_V:
				case INSTR_LOCA_LEFT_V:
					inputPattern ="#%d";
					break;
				default:
					throw exception("damn it !");
					break;
				}
				int is = sscanf(tLine.c_str() + strlen(_InsStr2InsOrd_space::_direct[i].pszIns),inputPattern, &operand);
				if( 1 != is )
					throw exception("unknonw operand !");
				codes.push_back(make_pair<string,pair<int,int> >
					(tLine,
					make_pair<int,int>(_InsStr2InsOrd_space::_direct[i].ins,operand)));
				bProcessed = true;
				break;
			}
		}
		if( bProcessed )
			continue;

		for(i=0;i<sizeof(_SOS::_with_str)/sizeof(_SOS::_with_str[0]);++i){
			if( !strncmp(_SOS::_with_str[i].pszIns,tLine.c_str(),
				strlen(_SOS::_with_str[i].pszIns))){
				char fname[BUFSIZ];
				int is = sscanf(tLine.c_str()+strlen(_SOS::_with_str[i].pszIns),"%s",fname);
				if( 1 != is )
					throw exception("lack of func name for instruction");
				int name_index = funcNameMan.addFunc( fname );
				if( name_index < 0 || name_index >= funcNameMan.getFuncNameList().size() )
					throw exception("addFunc failure");
				codes.push_back(
					make_pair<string,pair<int,int> >(
					tLine,make_pair<int,int>(
					_SOS::_with_str[i].ins,
					name_index)));
				bProcessed = true;
				break;
			}
		}

		if(bProcessed)
			continue;
		for(i=0;i<sizeof(_InsStr2InsOrd_space::_indct)/sizeof(_InsStr2InsOrd_space::_indct[0]);++i){
			if( !strncmp( _InsStr2InsOrd_space::_indct[i].pszIns,
				      tLine.c_str(),
					  strlen(_InsStr2InsOrd_space::_indct[i].pszIns))){
				char vname[BUFSIZ];
				int is = sscanf(tLine.c_str() + strlen(_InsStr2InsOrd_space::_indct[i].pszIns),"%s",vname);
				if( 1 != is )
					throw exception("unknown var name!");
				switch( _InsStr2InsOrd_space::_indct[i].ins ){
				case INSTR_LVALUE:
				case INSTR_RVALUE:
					do{
						int paddr =getVarAddress(vname);
						if( paddr < 0 )
							paddr = installVar(vname);
						if( paddr < 0 )	//~ twice 
							throw exception("fatal error !$(#$*");
						codes.push_back(
							make_pair<string,pair<int,int> >(
							tLine,
							make_pair<int,int>(_InsStr2InsOrd_space::_indct[i].ins,paddr)));
						bProcessed=true;
					}while(0);
					break;
				default:
					throw exception("fatal error $!@$!@");
					break;
				}
				if( bProcessed )
					break;
			}
		}
		//~
		if( bProcessed )
			continue;
		else
			throw exception("can not process this line");
	}//~ end of while(!input.eof())

	//~set_debug_mode(DBUG_CHECKINSTRUCTIONS);//~ for test.stacker
	if( 0 ){	//~ whatever it's for debug mode of special input file
		CodeCon::const_iterator pos;
		int it = 0;
		for(pos = codes.begin();pos!=codes.end();++pos,++it){
			cout<<"["<<it<<"]: "
				<<pos->second.first<<","<<pos->second.second<<endl;
			assert( pos->second.first == it );
		}
	}
}