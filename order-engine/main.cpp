#include <string>
#include <iostream>
#include "Order.h"

void printMembers( Json::Value &root );
using namespace std;

int main( int argc, const char *argv[] )
{
	try
	{
		OrderEngine engine;
		cout<<"Accepting orders"<<endl;
		string st;
		while (engine.readStdInput(st))
		{
			Json::Value root;
			if (!engine.parseInput(st, root))
				continue;
			engine.processOrder(root);
		}
		engine.printSummary();
	}
	catch ( const std::exception &e )
	{
		cerr<<"Unhandled exception:\n"<<e.what()<<"\n";
	}
	return 0;
}


// Debugging helper
void printMembers( Json::Value &root )
{
	Json::Value::Members members( root.getMemberNames() );

	for ( Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it )
	{
		const std::string &name = *it;
		cout<<name<<" : "<<root[name];
	}
}
