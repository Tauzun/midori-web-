## Some notes on DRM

Some site owners have decided to implement DRM via the html5 video tag, Blue Hawk does not play DRM protected video content by default.
To play DRM protected content, the files **widevinecdm.dll** and **widevinecdm.dll.sig** from the program files folder of a Chrome installation - for example:
`Google\Chrome\Application\VERSION_NUMBER_HERE\WidevineCdm\_platform_specific\win_x64\`
can be copied into the Blue Hawk application directory to enable DRM content playback. Please see the [Qt documentation](https://doc.qt.io/qt-5/qtwebengine-features.html#html5-drm) for more information.
Doing so, is not recommended however, unless absolutely necessary. DRM is morally reprehensible and ethically wrong.

It is recommended that you avoid DRM obfuscated content at all costs for security and piece of mind.
The DRM software provided by Chrome is proprietary software which may damage your machine because it does not have the wide scrutiny afforded by developing the software in the open, such as free and Open source software.
In short, we don't know what it is and we certainly have absolutely no reason to trust it's vendor.

To test that DRM protected content plays (or not,) visit the [Shaka Player demo](https://shaka-player-demo.appspot.com/demo/) and select Sintel.

Blue Hawk does not support h.264 video content playback by default. h.264 is a patented video codec which requires licensing for it's use in computer software.
h.264 is an old video codec and whilst it has considerable hardware support, there exist now codecs which are superior and not encumbered by patents, not to mention [AV1](https://aomediacodec.github.io/av1-spec/) (which is the future of video on the web.)

To enable h.264 video playback in Blue Hawk (provided that you have a license to do so,) recompile QtWebEngine using the `-webengine-proprietary-codecs` configure flag.
Then add your compiled version of QtWebEngine to Blue Hawk.

Blue Hawk should have no problem with the playback of `video/webm` and `video/ogg` in html documents.

For Web Masters and site owners: The time is right to move towards using WebM to encode video and reduce your licensing costs whilst we wait for AV1 hardware decoding to become ubiquitous.
Please offer your visitors a first class service by migrating away from h.264. Thank You.

You can test html5 video at the following web sites:

- [W3 Schools HTML5 Video](https://www.w3schools.com/html/html5_video.asp)
- [WebM Files dot org](https://www.webmfiles.org/demo-files/)
