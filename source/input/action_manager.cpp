#include "input/action_manager.hpp"

#include <iostream>

namespace input {

ActionManager::ActionManager(thor::ActionMap<std::string>& actionMap, std::string idPrefix) :
mActionMap(actionMap),
mCallbackSystem(),
mActionIds(),
mScopedConnections(),
mIdPrefix(idPrefix)
{
}

ActionManager::~ActionManager() {
	mScopedConnections.clear();

	for(auto& id : mActionIds) {
		mActionMap.removeAction(id);
	}
}

void ActionManager::addAction(std::string actionId, thor::Action action) {
	mActionMap[mIdPrefix + actionId] = action;
	mActionIds.push_back(mIdPrefix + actionId);
}

ActionManager::CallbackId ActionManager::addCallback(std::string actionId,
std::function<void(const thor::ActionContext<std::string>&)> function) {
	mScopedConnections.push_front(thor::ScopedConnection(mCallbackSystem.connect(mIdPrefix + actionId, function)));
	return mScopedConnections.cbegin();
}

thor::ScopedConnection ActionManager::addScopedCallback(std::string actionId,
std::function<void(const thor::ActionContext<std::string>&)> function) {
	return thor::ScopedConnection(mCallbackSystem.connect(mIdPrefix + actionId, function));
}

bool ActionManager::isActive(std::string actionId) {
	return mActionMap.isActive(mIdPrefix + actionId);
}

void ActionManager::removeAction(std::string actionId) {
	mActionIds.erase(std::remove(mActionIds.begin(), mActionIds.end(), mIdPrefix + actionId), mActionIds.end());
}

void ActionManager::removeCallback(CallbackId callbackId) {
	mScopedConnections.erase(callbackId);
}

void ActionManager::invokeCallbacks(sf::Window* window) {
	mActionMap.invokeCallbacks(mCallbackSystem, window);
}

}