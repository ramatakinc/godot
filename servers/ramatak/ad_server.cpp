#include "ad_server.h"

AdServer *AdServer::singleton = nullptr;

void AdServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_available_plugins"), &AdServer::get_available_plugins);

	ClassDB::bind_method(D_METHOD("set_plugin_priority_order", "priorities"), &AdServer::set_plugin_priority_order);
	ClassDB::bind_method(D_METHOD("get_plugin_priority_order"), &AdServer::get_plugin_priority_order);
}

void AdServer::register_ad_plugin(StringName p_name, Ref<AdPlugin> p_plugin) {
	if (this->ad_plugins.find(p_name) == nullptr) {
		this->ad_plugin_list.push_back(p_name);
	}
	this->ad_plugins[p_name] = p_plugin;
}

AdServer *AdServer::get_singleton() {
	return AdServer::singleton;
}

TypedArray<StringName> AdServer::get_available_plugins() const {
	return this->ad_plugin_list;
}

TypedArray<StringName> AdServer::get_plugin_priority_order() const {
	return this->ad_plugin_priorities;
}

void AdServer::set_plugin_priority_order(TypedArray<StringName> p_priorites) {
	this->ad_plugin_priorities = p_priorites;
}

AdServer::AdServer() {
	AdServer::singleton = this;
}

AdServer::~AdServer() {
	AdServer::singleton = nullptr;
}