#include "Order.h"
#include <algorithm>

using namespace std;

OrderStateMachine::OrderStateMachine()
{
	// A hash map is being initialized here. Duplicate keys will get overwritten with new values.
	// There are no compile time warnings for this.
	stateTable =
	{
		/*From State	To State		Transition Function					*/
		{{NEW,		COOKING},		&OrderStateMachine::NewToCooking},
		{{NEW,		CANCELED},		&OrderStateMachine::NewToCanceled},
		{{COOKING,	DELIVERING},		&OrderStateMachine::CookingToDelivering},
		{{COOKING,	CANCELED},		&OrderStateMachine::CookingToCanceled},
		{{DELIVERING,	DELIVERED},		&OrderStateMachine::DeliveringToDelivered},
		{{DELIVERING,	CANCELED},		&OrderStateMachine::DeliveringToCanceled},
		{{DELIVERED,	REFUNDED},		&OrderStateMachine::DeliveredToRefunded}
	};
}

bool OrderEngine::parseInput(std::string& line, Json::Value& root)
{
	Json::Reader reader( Json::Features::strictMode() );
	if ( !reader.parse(line, root) )
	{
		cerr<<"Failed to parse JSON. "<<reader.getFormatedErrorMessages().c_str()<<endl;
		return false;
	}

	if (!validateOrder(root))
	{
		cerr<<"Invalid order."<<endl;
		return false;
	}
	return true;
}

void OrderEngine::processOrder(Json::Value& newOrder)
{
	int orderId = newOrder["orderId"].asInt();
	int newUpdateId = newOrder["updateId"].asInt();
	OrderStatus newStatusCode = GetStatusCode(newOrder["status"].asCString());
	if (orderList.count(orderId) == 1)
	{
		// This order is already in. The new message is an update for it.
		Json::Value& oldOrder = orderList[orderId];
		int oldUpdateId = oldOrder["updateId"].asInt();

		// Dont process old updates again.
		if (newUpdateId <= oldUpdateId)
		{
			cerr<<"Skipping repeated/old update."<<endl;
			return;
		}
		OrderStatus oldStatusCode = GetStatusCode(oldOrder["status"].asCString());
		
		// Find the right state transition function from our table. O(1) lookup since its a hash map.
		OrderStateMachine::Transitions::const_iterator target = stateMachine.stateTable.find(make_pair(oldStatusCode, newStatusCode));
		
		if (target != stateMachine.stateTable.end())
		{
			try
			{
				target->second(stateMachine, oldOrder, newOrder);
				
				// On successful transition always carry forward the "amount" into the next JSON update since its needed at the end.
				newOrder["amount"] = oldOrder["amount"].asInt();
				oldOrder = newOrder;
			}
			catch ( const std::bad_function_call &e )
			{
				cerr<<"Bad function defined for the transition "<<GetStatusName(oldStatusCode)<<" to "<<GetStatusName(newStatusCode)<<endl;
			}
		}
		else
		{
			cerr<<"Invalid transition. Cannot go from "<<GetStatusName(oldStatusCode)<<" to "<<GetStatusName(newStatusCode)<<endl;
		}
	}
	else
	{
		// This is a new order. Add it to the list.
		if (newStatusCode != NEW)
		{
			cerr<<"Invalid order. Order cannot begin as "<<GetStatusName(newStatusCode)<<endl;
			return;
		}
		if (orderList.insert( std::make_pair(newOrder["orderId"].asInt(), newOrder) ).second)
			cout<<"Order "<<orderId<<" has come in."<<endl;
	}
}

bool OrderEngine::validateOrder(Json::Value& order)
{
	if (order.isMember("orderId") && order["orderId"].type() == Json::intValue &&
		order.isMember("updateId") && order["updateId"].type() == Json::intValue &&
		order.isMember("status") && order["status"].type() == Json::stringValue)
	{
		const char* status = order["status"].asCString();
		if (strcmp(status, "NEW") == 0)
		{
			if (!order.isMember("amount") || order["amount"].asInt() < 0)
				return false;
		}

		bool comp = false;
		for (int i = 0; (i < 6) && !comp; ++i)
			if (strcmp(OrderStatusNames[i], status) == 0)
				comp = true;
		return comp;
	}
	return false;
}

void OrderEngine::printSummary()
{
	U32 New = 0, Cooking = 0, Delivering = 0, Delivered = 0, Refunded = 0, Canceled = 0, Total = 0;
	
	// Iterate through the list and print summary.
	for (OrderMap::const_iterator it = orderList.begin(), end = orderList.end(); it != end; ++it)
	{
		Json::Value::Members members( it->second.getMemberNames() );
		OrderStatus statusCode = GetStatusCode(it->second["status"].asCString());
		bool addToSum = true;
		switch (statusCode)
		{
			case NEW: { addToSum = false; ++New; break; }
			case COOKING: { ++Cooking; break; }
			case DELIVERING: { ++Delivering; break; }
			case DELIVERED: { ++Delivered; break; }
			case REFUNDED: { addToSum = false; ++Refunded; break; }
			case CANCELED: { addToSum = false; ++Canceled; break; }
		}

		if (addToSum)
			Total += it->second["amount"].asInt();
	}
	cout<<endl<<"New: "<<New<<endl;
	cout<<"Cooking: "<<Cooking<<endl;
	cout<<"Delivering: "<<Delivering<<endl;
	cout<<"Delivered: "<<Delivered<<endl;
	cout<<"Canceled: "<<Canceled<<endl;
	cout<<"Refunded: "<<Refunded<<endl;
	cout<<"Total: $"<<Total<<endl;
}


void OrderStateMachine::NewToCooking(Json::Value& oldOrder, Json::Value& newOrder)
{
	cout<<"Order "<<oldOrder["orderId"].asInt()<<" is now cooking."<<endl;
}

void OrderStateMachine::NewToCanceled(Json::Value& oldOrder, Json::Value& newOrder)
{
	cout<<"New order "<<oldOrder["orderId"].asInt()<<" is now canceled."<<endl;
}

void OrderStateMachine::CookingToDelivering(Json::Value& oldOrder, Json::Value& newOrder)
{
	cout<<"Order "<<oldOrder["orderId"].asInt()<<" is now delivering."<<endl;
}

void OrderStateMachine::CookingToCanceled(Json::Value& oldOrder, Json::Value& newOrder)
{
	cout<<"Order "<<oldOrder["orderId"].asInt()<<" canceled while being cooked."<<endl;
}

void OrderStateMachine::DeliveringToDelivered(Json::Value& oldOrder, Json::Value& newOrder)
{
	cout<<"Order "<<oldOrder["orderId"].asInt()<<" has been delivered."<<endl;
}

void OrderStateMachine::DeliveringToCanceled(Json::Value& oldOrder, Json::Value& newOrder)
{
	cout<<"Order "<<oldOrder["orderId"].asInt()<<" canceled while being delivered."<<endl;
}

void OrderStateMachine::DeliveredToRefunded(Json::Value& oldOrder, Json::Value& newOrder)
{
	cout<<"Order "<<oldOrder["orderId"].asInt()<<" is now refunded."<<endl;
}

