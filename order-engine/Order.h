#ifndef ORDER_H
#define ORDER_H

#include <json/json.h>
#include <unordered_map>
#include <string>
#include <string.h>
#include <iostream>
#include <functional>

typedef unsigned int U32;

enum OrderStatus
{
	NEW = 0,
	COOKING,
	DELIVERING,
	DELIVERED,
	REFUNDED,
	CANCELED,

	NUM_STATUSES
};

static const char* OrderStatusNames[] = 
{
	"NEW",
	"COOKING",
	"DELIVERING",
	"DELIVERED",
	"REFUNDED",
	"CANCELED"
};


inline static const char* GetStatusName(OrderStatus status)
{
	return (status >= NEW && status < NUM_STATUSES) ? OrderStatusNames[status] : "Unknown";
}

static OrderStatus GetStatusCode(const char* statusString)
{
	for (int i = NEW; i < NUM_STATUSES; ++i)
		if (strcmp(OrderStatusNames[i], statusString) == 0)
			return (OrderStatus)i;
	return NUM_STATUSES;
}

// Specializing the std::hash functor for pair<OrderStatus, OrderStatus>
namespace std {
	template <>	struct hash<pair<OrderStatus, OrderStatus> > {
	public:
	    inline size_t operator()(const std::pair<OrderStatus, OrderStatus>& x) const throw() {
	         return (size_t)(hash<int>()(x.first) * 6737 + hash<int>()(x.second));
	    }
	};
}

class OrderStateMachine
{
public:
	typedef std::unordered_map<std::pair<OrderStatus, OrderStatus>, std::function<void (OrderStateMachine&, Json::Value&, Json::Value&)> > Transitions;
	
	OrderStateMachine();
	
	// Defining state transition functions.
	void NewToCooking(Json::Value&, Json::Value&);
	void NewToCanceled(Json::Value&, Json::Value&);
	void CookingToDelivering(Json::Value&, Json::Value&);
	void CookingToCanceled(Json::Value&, Json::Value&);
	void DeliveringToDelivered(Json::Value&, Json::Value&);
	void DeliveringToCanceled(Json::Value&, Json::Value&);
	void DeliveredToRefunded(Json::Value&, Json::Value&);
	
	// Transitions table. Populated in constructor
	Transitions stateTable;
};


class OrderEngine
{
public:
	typedef std::unordered_map<int, Json::Value> OrderMap;
	
	bool parseInput(std::string& line, Json::Value& root);
	void processOrder(Json::Value& newOrder);
	bool validateOrder(Json::Value& order);
	void printSummary();
	inline bool readStdInput(std::string& line) { return (getline(std::cin >> std::ws, line) && !std::cin.eof()); }
private:
	OrderMap orderList;
	OrderStateMachine stateMachine;
};

#endif //ORDER_H
