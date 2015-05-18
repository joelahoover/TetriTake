#ifndef _INPUT__ACTION_MANAGER_HPP_
#define _INPUT__ACTION_MANAGER_HPP_

#include <functional>
#include <list>

#include <Thor/Input.hpp>

namespace input {

class ActionManager {
public:
	using CallbackId = std::list<thor::ScopedConnection>::const_iterator;

public:
	ActionManager(thor::ActionMap<std::string>&, std::string idPrefix);
	~ActionManager();
	
	void addAction(std::string actionId, thor::Action);
	CallbackId addCallback(std::string actionId, std::function<void(const thor::ActionContext<std::string>&)>);

	thor::ScopedConnection addScopedCallback(std::string actionId, std::function<void(const thor::ActionContext<std::string>&)>);

	bool isActive(std::string actionId);

	void removeAction(std::string actionId);
	void removeCallback(CallbackId);

	void invokeCallbacks(sf::Window*);

private:
	thor::ActionMap<std::string>& mActionMap;
	thor::ActionMap<std::string>::CallbackSystem mCallbackSystem;

	std::vector<std::string> mActionIds;
	std::list<thor::ScopedConnection> mScopedConnections;

	std::string mIdPrefix;
};

}

#endif // _INPUT__ACTION_MANAGER_HPP_
