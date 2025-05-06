#pragma once

//data values

inline static auto server = fs::path();
inline static auto links = matjson::Value();

//get data
inline void loadServerInf() {
	if (not server.empty()) return;
	auto req = web::WebRequest();
	auto listener = new EventListener<web::WebTask>;
	listener->bind(
		[](web::WebTask::Event* e) {
			if (web::WebResponse* res = e->getValue()) {
				std::string data = res->string().unwrapOr("no res");
				if ((res->code() < 399) and (res->code() > 10)) {
					server = std::regex_replace(data, std::regex("\\s"), "");
					log::debug("server = {}", server);
				}
				else {
				}
			}
		}
	);
	listener->setFilter(req.send(
		"GET", (raw_content_repo_lnk + "/data/" + "/server.inf")
	));
}
$on_mod(Loaded){ loadServerInf(); }

inline void loadLinks() {
	auto req = web::WebRequest();
	auto listener = new EventListener<web::WebTask>;
	listener->bind(
		[](web::WebTask::Event* e) {
			if (web::WebResponse* res = e->getValue()) {
				std::string data = res->string().unwrapOr("no res");
				auto parse = res->json();
				if ((res->code() < 399) and (res->code() > 10) and parse.isOk()) {
					links = parse.unwrapOrDefault();
					log::debug("links = {}", links.dump());
				}
				else {
				}
			}
		}
	);
	listener->setFilter(req.send(
		"GET", (raw_content_repo_lnk + "/data/" + "/url.list.json")
	));
}
$on_mod(Loaded){ loadLinks(); }

//send
#include <Geode/modify/CCHttpClient.hpp>
class $modify(CCHttpClientLinksReplace, CCHttpClient) {
	$override void send(CCHttpRequest * req) {
		std::string url = req->getUrl();
		if (not server.empty()) url = std::regex_replace(
			url, 
			std::regex("www.boomlings.com\\/database"), 
			server.string()
		);
		log::debug("{}.url = {}", __FUNCTION__, url);
		if (SETTING(bool, "redir request urls")) req->setUrl(url.c_str());
		return CCHttpClient::send(req);
	}
};


//url open
#include <Geode/modify/CCApplication.hpp>
class $modify(CCApplicationLinksReplace, CCApplication) {
	$override void openURL(const char* url) {
		if (links.contains(url)) {
			url = links[url].asString().unwrapOr(url).c_str();
		}
		//log::debug("{}.url = {}", __FUNCTION__, url);
		return CCApplication::openURL(url);
	}
};