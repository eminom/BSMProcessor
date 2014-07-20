
//~ eminem7409@sina.com
///~ eminemextra@163.com

#include "bsm_processor.h"

#include <iostream>
using namespace std;

void
ProcessOpts(MyBSM::DSets& opts,int argc,const char **argv)
{
	for(int i=0;i<argc;++i){
		if( '-' == argv[i][0] ){
			switch(argv[i][1]){
			case 'd':
				do{
					if( !strcmp(argv[i],"-dstep") ){
						opts.insert(DBUG_STEP);
						cout<<"dstep on"<<endl;
					}
					else if( !strcmp(argv[i],"-dshow") ){
						opts.insert(DBUG_SHOW);
						cout<<"dshow on"<<endl;
					}
					else if( !strcmp(argv[i],"-dcomment")){
						opts.insert(DBUG_COMMENT);
						cout<<"dcomment on"<<endl;
					}
					else if( !strcmp(argv[i],"-dvarsatend")){
						opts.insert(DBUG_VARSATEND);
						cout<<"dvarsatend on"<<endl;
					}
				}while(0);
				break;
			}
		}
	}
}

void UsageX()
{
	cerr<<"bsmpv0p32 [stacker file]\n"
		<<" -d \n"
		<<"    step:     single step\n"
		<<"    show:	 show each instruction\n"
		<<"    comment:  show comments as no-op instructions\n"
		<<"    varsatend:show variables at the termination of program\n"
		<<"built on "<<__TIME__<<" , "<<__DATE__
		<<"eminem7409@163.com"<<endl;

}


int 
main(int argc,const char **argv)
{
	cout<<"[bsm][for amkv0p82] interpreter"<<endl;
	if(argc < 2 ){
		cerr<<"i need one input file"<<endl;
		UsageX();
		exit(1);
	}
	MyBSM::DSets doptionals;
	ProcessOpts(doptionals,argc,argv);
	try{
		MyBSM bsm(argv[1],doptionals);
		bsm.Run();
	}
	catch(exception&err){
		cerr<<err.what()<<endl;
	}
	catch(...){
		cerr<<"something went wrong"<<endl;
	}
	return 0;
}