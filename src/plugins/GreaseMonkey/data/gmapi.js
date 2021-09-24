/**
 * Userscript for Midori Web Browser
 *
 * Modified from https://gist.githubusercontent.com/arantius/3123124/raw/grant-none-shim.js
 *
 * This API attempts to provide as much Greasemonkey, Tampermonkey, etc API as possible
 * with hopefully some compatibility for older scripts. The target API level here is
 * version '4' which is based upon using js promises.
 *
 * Online documentation and points of reference are:
 * https://sourceforge.net/p/greasemonkey/wiki/Greasemonkey_Manual:API/
 * https://wiki.greasespot.net/Greasemonkey_Manual%3aAPI
 * https://www.tampermonkey.net/documentation.php
 *
 * Patches to expand, fix or improve this are very welcome :)
 *
 */
// GM.nameHash = unique script id
GM.nameHash = "%1";

var asyncCall = (func) => {
	if (window._browser_external) {
		func();
	} else {
		document.addEventListener("_browser_external_created", func);
	}
};

var decode = (val) => {
	val = String(val);
	if (!val.length) {
		return val;
	}
	var v = val.substr(1);
	if (val[0] == "b") {
		return Boolean(v == "true" ? true : false);
	} else if (val[0] == "i") {
		return Number(v);
	} else if (val[0] == "s") {
		return v;
	} else {
		return undefined;
	}
};

var encode = (val) => {
	if (typeof val == "boolean") {
		return "b" + (val ? "true" : "false");
	} else if (typeof val == "number") {
		return "i" + String(val);
	} else if (typeof val == "string") {
		return "s" + val;
	} else {
		return "";
	}
};
//Public Methods
GM.deleteValue = function (name) {
	return new Promise((resolve, reject) => {
		asyncCall(() => {
			external.extra.greasemonkey.deleteValue(GM.nameHash, name, (res) => {
				if (res) {
					resolve();
				} else {
					reject();
				}
			});
		});
	});
};

GM.getValue = function (name, value) {
	return new Promise((resolve) => {
		asyncCall(() => {
			external.extra.greasemonkey.getValue(GM.nameHash, name, encode(value), (res) => {
				resolve(decode(res));
			});
		});
	});
};

GM.listValues = function () {
	return new Promise((resolve) => {
		asyncCall(() => {
			external.extra.greasemonkey.listValues(GM.nameHash, resolve);
		});
	});
};

GM.openInTab = function (url) {
	return window.open(url);
};

GM.setClipboard = function (text) {
	external.extra.greasemonkey.setClipboard(text);
};

GM.setValue = function (name, value) {
	return new Promise((resolve, reject) => {
		asyncCall(() => {
			external.extra.greasemonkey.setValue(GM.nameHash, name, encode(value), (res) => {
				if (res) {
					resolve();
				} else {
					reject();
				}
			});
		});
	});
};

GM.xmlhttpRequest = function (/* object */ details) {
	details.method = details.method.toUpperCase() || "GET";
	if (!details.url) {
		if (console) console.error("The call to GM.xmlhttpRequest or GM_xmlhttpRequest requires a URL.");
		throw ("The call to GM.xmlhttpRequest or GM_xmlhttpRequest didn't contain a URL.");
	}
	// build XMLHttpRequest object
	var oXhr = new XMLHttpRequest;
	// run it
	if ("onreadystatechange" in details)
		oXhr.onreadystatechange = function () { details.onreadystatechange(oXhr) };
	if ("onload" in details)
		oXhr.onload = function () { details.onload(oXhr) };
	if ("onerror" in details)
		oXhr.onerror = function () { details.onerror(oXhr) };

	oXhr.open(details.method, details.url, true);

	if ("headers" in details)
		for (var header in details.headers)
			oXhr.setRequestHeader(header, details.headers[header]);

	if ("data" in details)
		oXhr.send(details.data);
	else
		oXhr.send();
};
// Unsupported parts of the GM API
GM.getResourceUrl = function (resourceName) {
	if (console) console.error('GM.getResourceUrl not supported');
	return new Promise((resolve, reject) => {
		reject();
	});
};

GM.legacyRegisterMenuCommand = function (caption, commandFunc, accessKey) { if (console) console.error('GM_registerMenuCommand not supported'); };

GM.legacyGetResourceUrl = function (resourceName) { if (console) console.error('GM_getResourceUrl not supported'); };

GM.notification = function (text, title, image, onclick) { if (console) console.error('GM.notification not supported'); };
//Legacy Public Methods
GM.legacyAddStyle = function (/* string */ styles) {

	if ((new RegExp("^http?://(?!(test.local)).*$")).test(document.location.href) || (document.location.href.indexOf("browser:") != 1) || (document.location.href.indexOf("ftp://") != 1) || (document.location.href.indexOf("file://") != 1)) {

		var head = document.getElementsByTagName("head")[0];
		if (head === undefined) {
			document.onreadystatechange = function () {
				if (document.readyState == "interactive") {
					var oStyle = document.createElement("style");
					oStyle.setAttribute("type", "text/css");
					oStyle.appendChild(document.createTextNode(styles));
					document.getElementsByTagName("head")[0].appendChild(oStyle);
				}
			}
		} else {
			var oStyle = document.createElement("style");
			oStyle.setAttribute("type", "text/css");
			oStyle.appendChild(document.createTextNode(styles));
			head.appendChild(oStyle);
		}

	} else {
		if (console) console.log('GM_addStyle was called but the conditions to do anything were not met.');
	}

};

GM.legacyDeleteValue = function (aKey) {
	localStorage.removeItem(GM.nameHash + aKey);
};

GM.legacyGetValue = function (aKey, aDefault) {
	var val = localStorage.getItem(GM.nameHash + aKey)
	if (null === val) return aDefault;
	return val;
};

GM.legacyInfo = function () {
	return GM.info;
};

GM.legacyListValues = function () {
	var values = [];
	for (var i = 0; i < localStorage.length; i++) {
		var k = localStorage.key(i);
		if (k.indexOf(GM.nameHash) === 0) {
			values.push(k.replace(GM.nameHash, ""));
		}
	}
	return values;
};

GM.legacyLog = function (log) {
	if (console)
		console.log(log);
};

GM.legacySetValue = function (aKey, aVal) {
	localStorage.setItem(GM.nameHash + aKey, aVal);
};
// GreaseMonkey 3.0 support
GM_openInTab = GM.openInTab;
GM_setClipboard = GM.setClipboard;
GM_xmlhttpRequest = GM.xmlhttpRequest;
GM_addStyle = GM.legacyAddStyle;
GM_deleteValue = GM.legacyDeleteValue;
GM_getValue = GM.legacyGetValue;
GM_info = GM.legacyInfo;
GM_listValues = GM.legacyListValues;
GM_log = GM.legacyLog;
GM_setValue = GM.legacySetValue;
GM_registerMenuCommand = GM.legacyRegisterMenuCommand;
GM_getResourceUrl = GM.legacyGetResourceUrl;

unsafeWindow = function () { GM.legacyLog('unsafeWindow not supported'); };

window.GM = GM;
GM.legacyLog('Midori Browser GreaseMonkey API Initialized');
