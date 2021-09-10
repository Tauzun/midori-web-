// ==UserScript==
// @name     _browser_speeddial
// @run-at   document-end
// @include  browser:speeddial
// ==/UserScript==

%THREEJS%
%VANTA-WAVES%
%VANTA-CODE%

%JQUERY%
%JQUERY-UI%

	(function (q) {
		function H(b) { return decodeURIComponent(atob(b).split("").map(function (d) { return "%" + ("00" + d.charCodeAt(0).toString(16)).slice(-2) }).join("")) } function r(b) { w = !0; external.speedDial.changed(b) } function z(b) { b = b.replace(/"/g, "&quot;"); return b = b.replace(/'/g, "&apos;") } function A(b) { b = b.replace(/&quot;/g, '"'); return b = b.replace(/&apos;/g, "'") } function x() { a("#fadeOverlay").fadeOut("fast", function () { a("#fadeOverlay").remove() }) } function B(b) {
			k = a(b).index(); var d = a(b).children("a").first().attr("href"),
				e = z(a(b).children("span").first().text()); "" === d && (d = "https://"); a("body").append('<div id="fadeOverlay" class="overlay" style="display:none;"><div id="overlay-edit"><img src="' + a(b).children("img").first().attr("src") + '"> <table><tr><td>' + f.url + ': </td><td><input type="text" id="formUrl" value="' + d + '"></td></tr><tr><td></td><td><div class="checkbox"><input type="checkbox" checked="checked" id="fetchTitle"><label for="fetchTitle"></label>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  ' + f.titleFetchTitle + ' </div></td></tr><tr id="titleLine"><td>' +
					f.title + ': </td><td><input type="text" id="formTitle" value="' + e + '"></td></tr></table><p class="buttonbox"><input id="cancelEditBox" type="button" value=" ' + f.cancel + ' ">&nbsp;&nbsp;&nbsp;<input id="acceptEditBox" type="button" value="   ' + f.edit + '   "></p></div></div>'); a("#titleLine").css({ visibility: "hidden" }); a("#fetchTitle").click(function () { var c; this.checked ? c = "hidden" : c = "visible"; a("#titleLine").css({ visibility: c }) }); a("#cancelEditBox").click(function () { "New Page" === e && a(b).remove(); x(); location.reload() });
			a("#acceptEditBox").click(C); a("#fadeOverlay").css({ filter: "alpha(opacity=100)" }).fadeIn("fast"); a("#fadeOverlay").click(function () { x() }); a("#overlay-edit").click(function (c) { c.stopPropagation() }); d = a("#formUrl").val(); a("#formUrl").focus().val("").val(d)
		} function C() {
			-1 != k && external.speedDial.urlFromUserInput(a("#formUrl").val(), function (b) {
				var d = document.getElementById("quickdial").getElementsByTagName("div")[k], e = d.getElementsByTagName("a")[0], c = e.getAttribute("href"); I(k, b); J(k, a("#formTitle").val());
				b = e.getAttribute("href"); var g = document.getElementById("fetchTitle").checked, h = t(); g || c != b && "" !== b ? (d.getElementsByTagName("img")[0].setAttribute("src", f.loadingImage), a("#fadeOverlay").fadeOut("fast", function () { a("#fadeOverlay").remove() }), external.speedDial.loadThumbnail(e.getAttribute("href"), g)) : x(); r(h)
			})
		} function t() {
			a('a[class="boxUrl"]'); a('span[class="boxTitle"]'); var b = ""; a("div.entry").each(function (d) {
				var e = a(this).children("a").first().attr("href"); d = a(this).children('span[class="boxTitle"]').first().text();
				var c = b; e = e.replace(/"/g, ""); e = e.replace(/'/g, ""); b = c + ('url:"' + e + '"|title:"' + z(d) + '";')
			}); return b
		} function D(b, d, e) {
			var c = document.createElement("div"); c.setAttribute("class", "entry"); var g = document.createElement("img"); g.setAttribute("src", e); var h = document.createElement("a"); h.setAttribute("href", b); h.setAttribute("class", "boxUrl"); var l = document.createElement("span"); l.setAttribute("class", "boxTitle"); l.setAttribute("title", A(d)); l.innerText = A(d); d = document.createElement("span"); d.setAttribute("class",
				"edit"); d.setAttribute("title", f.titleEdit); d.onclick = function () { B(c) }; var u = document.createElement("span"); u.setAttribute("class", "close"); u.setAttribute("title", f.titleRemove); u.onclick = function () { var m = a(c).index(); if (confirm(f.titleWarn)) var p = document.getElementById("quickdial").getElementsByTagName("div")[m]; void 0 !== p && (m = p.getElementsByTagName("a")[0].getAttribute("href"), document.getElementById("quickdial").removeChild(p), n(), external.speedDial.removeImageForUrl(m), r(t())) }; var v = document.createElement("span");
			v.setAttribute("class", "reload"); v.setAttribute("title", f.titleReload); v.onclick = function () { var m = a(c).children("a").first().attr("href"), p = a(c).children("img").first(); "" !== m && (a(p).attr("src", f.loadingImage), external.speedDial.loadThumbnail(m, !1)) }; c.appendChild(g); c.appendChild(g); c.appendChild(h); c.appendChild(l); c.appendChild(d); c.appendChild(u); c.appendChild(v); document.getElementById("quickdial").appendChild(c); e == f.loadingImage && external.speedDial.loadThumbnail(b, !1); n(); return c
		} function K(b,
			d) { var e = !1, c = document.getElementById("quickdial").getElementsByTagName("div"); for (i = 0; i < c.length; ++i) { var g = c[i]; if (void 0 !== g) { var h = g.getElementsByTagName("a")[0].getAttribute("href"); b == h && (g = g.getElementsByTagName("span")[0], g.innerText != d && (e = !0, g.innerText = d)) } } e && r(t()) } function L(b, d) { var e = a('a[href="' + b + '"]'); a(e).each(function () { var c = a(this).parent(); c = a(c).children("img").first(); 0 != a(c).size() && a(c).attr("src", d) }) } function I(b, d) {
				var e = document.getElementById("quickdial").getElementsByTagName("div")[b];
				void 0 !== e && e.getElementsByTagName("a")[0].setAttribute("href", d)
			} function J(b, d) { var e = document.getElementById("quickdial").getElementsByTagName("div")[b]; void 0 !== e && (e.getElementsByTagName("span")[0].innerText = d) } function n() {
				var b = parseInt(f.dialWidth), d = Math.floor(Math.round(b / 1.54)); a("head").append("<style>#quickdial img{height:auto;width:" + b + "px}</style>"); a("#quickdial div.entry").css({ width: b + "px", height: d + "px" }); var e = a(window).width(), c = Math.floor(e - 76); e = a(window).height(); var g = Math.floor(b +
					12), h = d + 22; c = Math.floor(c / g); c > f.maxPagesRow && (c = f.maxPagesRow); 1 > c && (c = 1); g *= c; a("#quickdial").css("width", g + "px"); g = a("#quickdial").children("div").size(); e = (e - Math.ceil(g / c) * h) / 2; 0 > e && (e = 0); a("#quickdial").css("margin-top", e + "px"); a("span.boxTitle").css("font-size", (d - b / 1.77) / 1.5 + "px"); "true" == f.sdCenter ? E() : F()
			} function M() { external.speedDial.getOpenFileName(function (b) { b.length && (document.getElementById("BgImgHold").value = b[0], document.getElementById("BgImgHoldUrl").value = b[1], y()) }) } function N() {
				var b =
					document.getElementById("SdSizeToggle"), d = document.getElementById("SdSize"), e = document.getElementById("sliderValueSd"); d.disabled = b.checked ? !1 : !0; d.value = b.checked ? d.value : 240; e.innerHTML = b.checked ? f.dialWidth : 240; 1 != a("#SdSizeToggle").prop("checked") ? a("#SdSizeStateColor").css("color", "rgba(0,0,0, 0.0)") : a("#SdSizeStateColor").css("color", "#eaeaea")
			} function y() {
				var b = document.getElementById("BgImgHold").value, d = document.getElementById("BgImgSelSiz").value, e = document.getElementById("thumb"); e.style.backgroundImage =
					'url("' + b + '")'; e.title = b.substring(b.lastIndexOf("/") + 1); e.style.backgroundSize = d; document.documentElement.style.backgroundImage = 'url("' + b + '")'; document.documentElement.style.backgroundSize = d
			} function E() { a("#quickdial div.entry").css({ "float": "none", display: "inline-block" }) } function F() { a("#quickdial div.entry").css({ "float": f.leftStr, display: "block" }) } function G() {
				f = document.getElementById("script-data").dataset; document.getElementById("button-configure-speed-dial").onclick = q.configureSpeedDial;
				document.getElementById("button-add-speed-dial").onclick = q.addSpeedDial; document.getElementById("PgInRow").oninput = function () { a("#sliderValuePg").html(this.value) }; document.getElementById("SdSizeToggle").onchange = N; document.getElementById("SdSize").oninput = function () { a("#sliderValueSd").html(this.value) }; document.getElementById("BgImgSel").onclick = function () { "disabled" != a("#BgImgSelSiz").attr("disabled") && M() }; document.getElementById("BgImgToggle").onchange = function () {
					var c = document.getElementById("BgImgToggle"),
					g = document.getElementById("BgImgSel"), h = document.getElementById("BgImgHoldUrl"), l = document.getElementById("BgImgSelSiz"); g.disabled = c.checked ? !1 : !0; h.disabled = c.checked ? !1 : !0; l.disabled = c.checked ? !1 : !0; h.value = c.checked ? f.urlBackground : ""; 1 != a("#BgImgToggle").prop("checked") ? (a("#ImgSelectorMenu").css({ color: "rgba(0,0,0, 0.0)", "text-shadow": "none" }), a("#BgImgSel").css({ color: "rgba(0,0,0, 0.0)", "text-shadow": "none" }), a("#BgImgSelSiz").css("visibility", "hidden")) : (a("#ImgSelectorMenu").css({
						color: "#eaeaea",
						"text-shadow": "1px 1px 2px #000000, 0 0 1em #000000"
					}), a("#BgImgSel").css({ color: "#eaeaea", "text-shadow": "1px 1px 2px #000000, 0 0 1em #000000" }), a("#BgImgSelSiz").css("visibility", "visible")); y()
				}; document.getElementById("BgImgSelSiz").onchange = y; document.getElementById("button-cancel").onclick = function () { a("#fadeOverlay2").fadeOut("fast") }; document.getElementById("button-apply").onclick = function () {
					f.maxPagesRow = a("#PgInRow").val(); f.dialWidth = a("#SdSize").val(); f.sdCenter = a("#SdCntrToggle").prop("checked");
					external.speedDial.setBackgroundImage(a("#BgImgHoldUrl").val()); external.speedDial.setBackgroundImageSize(a("#BgImgSelSiz").val()); external.speedDial.setPagesInRow(f.maxPagesRow); external.speedDial.setSdSize(f.dialWidth); external.speedDial.setSdCentered("true" == f.sdCenter); n(); a("#fadeOverlay2").fadeOut("fast")
				}; "" == f.imgBackground && (document.getElementById("html").style.backgroundSize = "cover"); a(document).keyup(function (c) { -1 != k && (13 == c.keyCode ? C() : 27 == c.keyCode && a("#fadeOverlay").click()) }); for (var b =
                    JSON.parse(H(f.initialScript)), d = 0; d < b.length; ++d) { var e = b[d]; D(e.url, e.title, e.img) } external.speedDial.pagesChanged.connect(function () { w ? w = !1 : setTimeout(function () { a("#quickdial").load("browser:speeddial #quickdial", function () { }) }, 5E3) }); external.speedDial.thumbnailLoaded.connect(L); external.speedDial.pageTitleLoaded.connect(K); a(window).resize(function () { n() }); a("#quickdial").sortable({
						revert: !0, cursor: "move", containment: "document", opacity: .8, distance: 40, start: function (c, g) { F() }, stop: function (c,
							g) { "true" == f.sdCenter && E() }, update: function (c, g) { r(t()) }
					}); n()
			} var f = {}, k = -1, w = !1, a = jQuery.noConflict(); q.addSpeedDial = function () { B(D("", f.newPage, "")); n() }; q.configureSpeedDial = function () {
				a("#PgInRow").val(f.maxPagesRow); a("#sliderValuePg").html(f.maxPagesRow); a("#SdSize").val(f.dialWidth); a("#SdSizeToggle").prop("checked", 240 != f.dialWidth); a("#sliderValueSd").html(f.dialWidth); a("#BgImgSelSiz").val(f.bSize).attr("selected", "selected"); a("#BgImgToggle").prop("checked", "" != f.imgBackground); a("#SdCntrToggle").prop("checked",
					"true" == f.sdCenter); a("#BgImgToggle").is(":checked") ? a("#BgImgSel").removeAttr("disabled") : a("#BgImgSel").attr("disabled", "disabled"); a("#BgImgToggle").is(":checked") ? a("#BgImgSelSiz").removeAttr("disabled") : a("#BgImgSelSiz").attr("disabled", "disabled"); a("#SdSizeToggle").is(":checked") ? a("#SdSize").removeAttr("disabled") : a("#SdSize").attr("disabled", "disabled"); 1 != a("#BgImgToggle").prop("checked") && (a("#ImgSelectorMenu").css({ color: "rgba(0,0,0, 0.0)", "text-shadow": "none" }), a("#BgImgSel").css({
						color: "rgba(0,0,0, 0.0)",
						"text-shadow": "none"
					}), a("#BgImgSelSiz").css("visibility", "hidden")); 1 != a("#SdSizeToggle").prop("checked") && a("#SdSizeStateColor").css("color", "rgba(0,0,0, 0.0)"); a("#fadeOverlay2").css({ filter: "alpha(opacity=100)" }).fadeIn("fast"); a("#fadeOverlay2").click(function () { a(this).fadeOut("fast") }); a("#settingsBox").click(function (b) { b.stopPropagation() })
			}; window._browser_external ? G() : document.addEventListener("_browser_external_created", G)
	})(window.a = window.a || {});
