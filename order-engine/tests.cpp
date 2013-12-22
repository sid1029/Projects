#include "Order.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>

namespace {
	using namespace std;
	
	TEST(OrderParsingTest, ValidJSON) {
		OrderEngine engine;
		std::ifstream inputfile ("testinputs_valid.json");
	   	
	   	if (inputfile.is_open())
		{
			string line;
			while ( getline (inputfile,line) )
			{
				cout<<endl<<endl<<"Testing input : "<<line<<endl;
				Json::Value root;
				bool isInputValid = engine.parseInput(line, root);
				if (isInputValid)
					cout<<"Parsing and validation passed"<<endl;
				EXPECT_TRUE(isInputValid);
				EXPECT_GE(root.size(), 3);
			}
		}
		inputfile.close();
	}
	
	TEST(OrderParsingTest, InValidJSON) {
		OrderEngine engine;
		std::ifstream inputfile ("testinputs_invalid.json");
	   	
	   	if (inputfile.is_open())
		{
			string line;
			while ( getline (inputfile,line) )
			{
				cout<<endl<<endl<<"Testing input : "<<line<<endl;
				Json::Value root;
				bool isInputValid = engine.parseInput(line, root);
				EXPECT_FALSE(isInputValid);
			}
		}
		inputfile.close();
	}
	
	TEST(OrderParsingTest, OrderSummary) {
		OrderEngine engine;
		std::ifstream inputfile ("testinputs_valid.json");
		
	   	if (inputfile.is_open())
		{
			string line;
			while ( getline (inputfile,line) )
			{
				cout<<endl<<endl<<"Testing input : "<<line<<endl;
				Json::Value root;
				bool isInputValid = engine.parseInput(line, root);
				EXPECT_TRUE(isInputValid);
				if (isInputValid)
					engine.processOrder(root);

				EXPECT_GE(root.size(), 3);
			}
		}
		inputfile.close();
		engine.printSummary();
	}

}  // namespace

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
