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

    (function (a) {

        var scriptData = {};
        var editingId = -1;
        var ignoreNextChanged = false;
        var jQ = jQuery.noConflict();

	function b64DecodeUnicode(str) {
	    return decodeURIComponent(atob(str).split('').map(function(c) {
        	return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
	    }).join(''));
	}

        function emitChanged(pages) {
            ignoreNextChanged = true;
            external.speedDial.changed(pages);
        }

        a.addSpeedDial = function () {
            onEditClick(addBox('', scriptData.newPage, ''));
            alignPage();
        }

        a.configureSpeedDial = function () {
            // ====================== LOAD SETTINGS ========================= //
            jQ('#PgInRow').val(scriptData.maxPagesRow);
            jQ('#sliderValuePg').html(scriptData.maxPagesRow);
            jQ('#SdSize').val(scriptData.dialWidth);
            jQ('#SdSizeToggle').prop('checked', scriptData.dialWidth != 240);
            jQ('#sliderValueSd').html(scriptData.dialWidth);
            jQ('#BgImgSelSiz').val(scriptData.bSize).attr('selected', 'selected');
            jQ('#BgImgToggle').prop('checked', scriptData.imgBackground != "");
            jQ('#SdCntrToggle').prop('checked', scriptData.sdCenter == "true");
            jQ('#BgImgToggle').is(':checked') ? jQ('#BgImgSel').removeAttr('disabled') : jQ('#BgImgSel').attr('disabled', 'disabled');
            jQ('#BgImgToggle').is(':checked') ? jQ('#BgImgSelSiz').removeAttr('disabled') : jQ('#BgImgSelSiz').attr('disabled', 'disabled');
            jQ('#SdSizeToggle').is(':checked') ? jQ('#SdSize').removeAttr('disabled') : jQ('#SdSize').attr('disabled', 'disabled');
            if (jQ('#BgImgToggle').prop('checked') != true) {
                jQ('#ImgSelectorMenu').css({ 'color': 'rgba(0,0,0, 0.0)', 'text-shadow': 'none' });
                jQ('#BgImgSel').css({ 'color': 'rgba(0,0,0, 0.0)', 'text-shadow': 'none' });
                jQ('#BgImgSelSiz').css('visibility', 'hidden');
            }
            if (jQ('#SdSizeToggle').prop('checked') != true)
                jQ('#SdSizeStateColor').css('color', 'rgba(0,0,0, 0.0)');
            // ======================== SHOW DIALOG ======================== //
            jQ('#fadeOverlay2').css({ 'filter': 'alpha(opacity=100)' }).fadeIn("fast");
            jQ('#fadeOverlay2').click(function () { jQ(this).fadeOut('fast'); });
            jQ('#settingsBox').click(function (event) { event.stopPropagation(); });
        }

        function escapeTitle(title) {
            title = title.replace(/"/g, '&quot;');
            title = title.replace(/'/g, '&apos;');
            return title;
        }

        function unescapeTitle(title) {
            title = title.replace(/&quot;/g, '"');
            title = title.replace(/&apos;/g, '\'');
            return title;
        }

        function escapeUrl(url) {
            url = url.replace(/"/g, '');
            url = url.replace(/'/g, '');
            return url;
        }

        function onRemoveClick(box) {
            removeBox(jQ(box).index());
        }

        function onFetchTitleClick(checkbox) {
            var displayStyle;
            checkbox.checked ? displayStyle = 'hidden' : displayStyle = 'visible';
            jQ('#titleLine').css({ 'visibility': displayStyle });
        }

        function hideEditBox() {
            jQ('#fadeOverlay').fadeOut("fast", function () { jQ("#fadeOverlay").remove(); });
        }

        function onEditClick(box) {
            editingId = jQ(box).index();
            var boxUrl = jQ(box).children('a').first().attr('href');
            var boxTitle = escapeTitle(jQ(box).children('span').first().text());
            if (boxUrl === '')
                boxUrl = 'https://';

            jQ('body').append('<div id="fadeOverlay" class="overlay" style="display:none;">' +
                '<div id="overlay-edit">' +
                '<img src="' + jQ(box).children('img').first().attr('src') + '"> ' +
                '<table><tr><td>' + scriptData.url + ': </td><td>' +
                '<input type="text" id="formUrl" value="' + boxUrl + '"></td></tr>' +
                '<tr><td></td><td><div class="checkbox"><input type="checkbox" checked="checked" id="fetchTitle">' +
                '<label for="fetchTitle"></label>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  ' + scriptData.titleFetchTitle + ' </div></td></tr>' +
                '<tr id="titleLine"><td>' + scriptData.title + ': </td><td>' +
                '<input type="text" id="formTitle" value="' + boxTitle + '"></td></tr>' +
                '</table><p class="buttonbox"><input id="cancelEditBox" type="button" value=" ' + scriptData.cancel + ' ">&nbsp;&nbsp;&nbsp;' +
                '<input id="acceptEditBox" type="button" value="   ' + scriptData.edit + '   "></p>' +
                '</div></div>');

            jQ('#titleLine').css({ 'visibility': 'hidden' });

            jQ('#fetchTitle').click(function () {
                onFetchTitleClick(this);
            });

            jQ('#cancelEditBox').click(function () {
                if (boxTitle === 'New Page') {
                    jQ(box).remove();
                }
                hideEditBox();
                location.reload();
            });

            jQ('#acceptEditBox').click(boxEdited);

            jQ('#fadeOverlay').css({ 'filter': 'alpha(opacity=100)' }).fadeIn("fast");
            jQ('#fadeOverlay').click(function () { hideEditBox() });
            jQ('#overlay-edit').click(function (event) { event.stopPropagation(); });

            var temp = jQ('#formUrl').val();
            jQ('#formUrl').focus().val('').val(temp); // focus and move cursor to end
        }

        function onReloadClick(box) {
            var url = jQ(box).children('a').first().attr('href');
            var img = jQ(box).children('img').first();

            if (url === '')
                return;

            jQ(img).attr('src', scriptData.loadingImage);
            external.speedDial.loadThumbnail(url, false);
        }

        function boxEdited() {
            if (editingId == -1)
                return;

            external.speedDial.urlFromUserInput(jQ('#formUrl').val(), function (newUrl) {
                var box = document.getElementById('quickdial').getElementsByTagName('div')[editingId];
                var a = box.getElementsByTagName('a')[0];
                var originalUrl = a.getAttribute('href');
                setBoxUrl(editingId, newUrl);
                setBoxTitle(editingId, jQ('#formTitle').val());
                var changedUrl = a.getAttribute('href');
                var fetchTitleChecked = document.getElementById('fetchTitle').checked;

                var pages = allPages();

                if (fetchTitleChecked || (originalUrl != changedUrl && changedUrl !== '')) {
                    var img = box.getElementsByTagName('img')[0];
                    img.setAttribute('src', scriptData.loadingImage);

                    jQ('#fadeOverlay').fadeOut("fast", function () {
                        jQ("#fadeOverlay").remove();
                    });
                    external.speedDial.loadThumbnail(a.getAttribute('href'), fetchTitleChecked);
                } else {
                    hideEditBox();
                }
                emitChanged(pages);
                //location.reload();
            });
        }

        function allPages() {
            var urls = jQ('a[class="boxUrl"]');
            var titles = jQ('span[class="boxTitle"]');
            var value = "";
            jQ('div.entry').each(function (i) {
                var url = jQ(this).children('a').first().attr('href');
                var title = jQ(this).children('span[class="boxTitle"]').first().text();

                value += 'url:"' + escapeUrl(url) + '"|title:"' + escapeTitle(title) + '";';
            });

            return value;
        }

        function reloadAll() {
            if (confirm(scriptData.titleWarnRel))
                jQ('div.entry').each(function (i) {
                    onReloadClick(jQ(this));
                });
        }

        function addBox(url, title, img_source) {
            var div = document.createElement('div');
            div.setAttribute('class', 'entry');
            var img = document.createElement('img');
            img.setAttribute('src', img_source);
            var a = document.createElement('a');
            a.setAttribute('href', url);
            a.setAttribute('class', 'boxUrl');
            var span1 = document.createElement('span');
            span1.setAttribute('class', 'boxTitle');
            span1.setAttribute('title', unescapeTitle(title));
            span1.innerText = unescapeTitle(title);
            var span2 = document.createElement('span');
            span2.setAttribute('class', 'edit');
            span2.setAttribute('title', scriptData.titleEdit);
            span2.onclick = function () {
                onEditClick(div);
            };
            var span3 = document.createElement('span');
            span3.setAttribute('class', 'close');
            span3.setAttribute('title', scriptData.titleRemove);
            span3.onclick = function () {
                onRemoveClick(div);
            };
            var span4 = document.createElement('span');
            span4.setAttribute('class', 'reload');
            span4.setAttribute('title', scriptData.titleReload);
            span4.onclick = function () {
                onReloadClick(div);
            };
            div.appendChild(img);
            div.appendChild(img);
            div.appendChild(a);
            div.appendChild(span1);
            div.appendChild(span2);
            div.appendChild(span3);
            div.appendChild(span4);
            document.getElementById("quickdial").appendChild(div);
            if (img_source == scriptData.loadingImage) {
                /* Image already exists */
                external.speedDial.loadThumbnail(url, false);
            }
            alignPage();
            return div;
        }

        function setBoxImage(id, img_source) {
            var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
            if (box === undefined)
                return;
            var img = box.getElementsByTagName('img')[0];
            img.setAttribute('src', img_source + '?' + new Date());
        }

        function setTitleToUrl(url, title) {
            var changed = false;
            var boxes = document.getElementById('quickdial').getElementsByTagName('div');
            for (i = 0; i < boxes.length; ++i) {
                var box = boxes[i];

                if (box === undefined)
                    continue;

                var boxUrl = box.getElementsByTagName('a')[0].getAttribute('href');
                if (url != boxUrl)
                    continue;

                var span = box.getElementsByTagName('span')[0];
                if (span.innerText != title) {
                    changed = true;
                    span.innerText = title;
                }
            }
            if (changed)
                emitChanged(allPages());
        }

        function setImageToUrl(url, img_source) {
            var aElement = jQ('a[href="' + url + '"]');
            jQ(aElement).each(function () {
                var box = jQ(this).parent();
                var imgElement = jQ(box).children("img").first();
                if (jQ(imgElement).size() == 0)
                    return;
                jQ(imgElement).attr('src', img_source/* + '?' + new Date()*/);
            });
        }

        function setBoxUrl(id, url) {
            var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
            if (box === undefined)
                return;
            var a = box.getElementsByTagName('a')[0];
            a.setAttribute('href', url);
        }

        function setBoxTitle(id, title) {
            var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
            if (box === undefined)
                return;
            var span = box.getElementsByTagName('span')[0];
            span.innerText = title;
        }

        function removeBox(id) {
            if (confirm(scriptData.titleWarn))
                var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
            if (box === undefined)
                return;
            var url = box.getElementsByTagName('a')[0].getAttribute('href');
            document.getElementById("quickdial").removeChild(box);
            alignPage();
            external.speedDial.removeImageForUrl(url);
            emitChanged(allPages());
        }

        function alignPage() {
            var dialWidth = parseInt(scriptData.dialWidth);
            var dialHeight = Math.floor(Math.round(dialWidth / 1.54));
            jQ('head').append('<style>#quickdial img{height:auto;width:' + dialWidth + 'px}</style>');
            jQ('#quickdial div.entry').css({
                'width': dialWidth + 'px',
                'height': dialHeight + 'px'
            });
            var fullwidth = jQ(window).width();
            var width = Math.floor(fullwidth - 76);
            var height = jQ(window).height();
            var boxWidth = Math.floor(dialWidth + 12);
            var boxHeight = dialHeight + 22;
            var maxBoxes = Math.floor(width / boxWidth);
            if (maxBoxes > scriptData.maxPagesRow) maxBoxes = scriptData.maxPagesRow;
            if (maxBoxes < 1) maxBoxes = 1;
            var maxwidth = maxBoxes * boxWidth;
            jQ("#quickdial").css('width', maxwidth + 'px');
            var boxesCount = jQ("#quickdial").children("div").size();
            var rows = Math.ceil(boxesCount / maxBoxes);
            var margintop = (height - rows * boxHeight) / 2;
            if (margintop < 0) margintop = 0;
            jQ("#quickdial").css('margin-top', margintop + 'px');
            jQ("span.boxTitle").css('font-size', ((dialHeight - dialWidth / 1.77) / 1.5) + 'px');
            if (scriptData.sdCenter == "true") {
                enableCentering();
            } else {
                disableCentering();
            }
        }

        function bgImageSel() {
            external.speedDial.getOpenFileName(function (arr) {
                if (arr.length) {
                    document.getElementById('BgImgHold').value = arr[0];
                    document.getElementById('BgImgHoldUrl').value = arr[1];
                    bgImgUpdate();
                }
            });
        }

        function saveSettings() {
            scriptData.maxPagesRow = jQ('#PgInRow').val();
            scriptData.dialWidth = jQ('#SdSize').val();
            scriptData.sdCenter = jQ('#SdCntrToggle').prop('checked');
            external.speedDial.setBackgroundImage(jQ('#BgImgHoldUrl').val());
            external.speedDial.setBackgroundImageSize(jQ('#BgImgSelSiz').val());
            external.speedDial.setPagesInRow(scriptData.maxPagesRow);
            external.speedDial.setSdSize(scriptData.dialWidth);
            external.speedDial.setSdCentered(scriptData.sdCenter == "true");
            alignPage();
        }

        function bgImgToggle() {
            var check = document.getElementById('BgImgToggle');
            var BgImgSel = document.getElementById('BgImgSel');
            var BgImgHoldUrl = document.getElementById('BgImgHoldUrl');
            var BgImgSz = document.getElementById('BgImgSelSiz');
            BgImgSel.disabled = (check.checked ? false : true);
            BgImgHoldUrl.disabled = (check.checked ? false : true);
            BgImgSz.disabled = (check.checked ? false : true);
            BgImgHoldUrl.value = (check.checked ? scriptData.urlBackground : '');
            if (jQ('#BgImgToggle').prop('checked') != true) {
                jQ('#ImgSelectorMenu').css({ 'color': 'rgba(0,0,0, 0.0)', 'text-shadow': 'none' });
                jQ('#BgImgSel').css({ 'color': 'rgba(0,0,0, 0.0)', 'text-shadow': 'none' });
                jQ('#BgImgSelSiz').css('visibility', 'hidden');
            } else {
                jQ('#ImgSelectorMenu').css({ 'color': '#eaeaea', 'text-shadow': '1px 1px 2px #000000, 0 0 1em #000000' });
                jQ('#BgImgSel').css({ 'color': '#eaeaea', 'text-shadow': '1px 1px 2px #000000, 0 0 1em #000000' });
                jQ('#BgImgSelSiz').css('visibility', 'visible');
            };
        }

        function sdSizeToggle() {
            var check = document.getElementById('SdSizeToggle');
            var SdSize = document.getElementById('SdSize');
            var SdSizeSl = document.getElementById('sliderValueSd');
            SdSize.disabled = (check.checked ? false : true);
            SdSize.value = (check.checked ? SdSize.value : 240);
            SdSizeSl.innerHTML = (check.checked ? scriptData.dialWidth : 240);
            if (jQ('#SdSizeToggle').prop('checked') != true) {
                jQ('#SdSizeStateColor').css('color', 'rgba(0,0,0, 0.0)');
            } else {
                jQ('#SdSizeStateColor').css('color', '#eaeaea')
            }
        }

        function bgImgUpdate() {
            var imgUrl = document.getElementById('BgImgHold').value;
            var imgSize = document.getElementById('BgImgSelSiz').value;
            var imgThumb = document.getElementById('thumb');
            imgThumb.style.backgroundImage = 'url("' + imgUrl + '")';
            imgThumb.title = imgUrl.substring(imgUrl.lastIndexOf('/') + 1);
            imgThumb.style.backgroundSize = imgSize;
            document.documentElement.style.backgroundImage = 'url("' + imgUrl + '")';
            document.documentElement.style.backgroundSize = imgSize;
        }

        function enableCentering() {
            jQ('#quickdial div.entry').css({
                float: 'none',
                display: 'inline-block'
            });
        }

        function disableCentering() {
            jQ('#quickdial div.entry').css({
                float: scriptData.leftStr,
                display: 'block'
            });
        }

        function init() {
            scriptData = document.getElementById("script-data").dataset;

            document.getElementById("button-configure-speed-dial").onclick = a.configureSpeedDial;
            document.getElementById("button-add-speed-dial").onclick = a.addSpeedDial;
            document.getElementById("PgInRow").oninput = function () {
                jQ('#sliderValuePg').html(this.value);
            };
            document.getElementById("SdSizeToggle").onchange = sdSizeToggle;
            document.getElementById("SdSize").oninput = function () {
                jQ('#sliderValueSd').html(this.value);
            };
            document.getElementById("BgImgSel").onclick = function () {
                if (jQ('#BgImgSelSiz').attr('disabled') != 'disabled') {
                    bgImageSel();
                }
            };
            document.getElementById("BgImgToggle").onchange = function () {
                bgImgToggle();
                bgImgUpdate();
            };
            document.getElementById("BgImgSelSiz").onchange = bgImgUpdate;
            document.getElementById("button-cancel").onclick = function () {
                jQ('#fadeOverlay2').fadeOut('fast');
                //location.reload();
            };
            document.getElementById("button-apply").onclick = function () {
                saveSettings();
                jQ('#fadeOverlay2').fadeOut('fast');
                //location.reload();
            };

            if (scriptData.imgBackground == '') {
                document.getElementById("html").style.backgroundSize = "cover";
            }

            jQ(document).keyup(function (e) {
                if (editingId == -1)
                    return;
                if (e.keyCode == 13)
                    boxEdited();
                else if (e.keyCode == 27)
                    jQ('#fadeOverlay').click();
            });

            var pages = JSON.parse(b64DecodeUnicode(scriptData.initialScript));
            for (var i = 0; i < pages.length; ++i) {
                var page = pages[i];
                addBox(page.url, page.title, page.img);
            }

            external.speedDial.pagesChanged.connect(function () {
                if (ignoreNextChanged) {
                    ignoreNextChanged = false;
                    return;
                }
                //window.location.reload();
                setTimeout(function () {
                    jQ('#quickdial').load('browser:speeddial #quickdial', function () {

                    });
                }, 5000);
            });

            external.speedDial.thumbnailLoaded.connect(setImageToUrl);
            external.speedDial.pageTitleLoaded.connect(setTitleToUrl);

            jQ(window).resize(function () { alignPage(); });
            jQ("#quickdial").sortable({
                revert: true,
                cursor: 'move',
                containment: 'document',
                opacity: 0.8,
                distance: 40,
                start: function (event, ui) {
                    disableCentering();
                },
                stop: function (event, ui) {
                    if (scriptData.sdCenter == "true")
                        enableCentering();
                },
                update: function (event, ui) {
                    emitChanged(allPages());
                }
            });

            alignPage();
        }
        // Initialize
        if (window._browser_external) {
            init();
        } else {
            document.addEventListener("_browser_external_created", init);
        }

    }(window.a = window.a || {}));