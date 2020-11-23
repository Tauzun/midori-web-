<div align="center">
  <a href="https://astian.org/midori-browser/"><img src="static/icons/icon.png" width="256"></a>
</div>

<div align="center">
  <h1>Midori Browser</h1> 

[![Actions Status](https://img.shields.io/badge/GitLab-Midori--Browser-brightgreen?style=flat&logo=gitlab)](https://gitlab.com/midori-web/)
[![PayPal](https://img.shields.io/badge/PayPal-Donate-brightgreen?style=flat&logo=PayPal)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=UQPXCVUG2CKNG&source=url)
[![Telegram](https://img.shields.io/badge/Telegram-EN-brightgreen?style=flat&logo=telegram)](https://t.me/midoriweb)
[![Telegram](https://img.shields.io/badge/Telegram-ES-brightgreen?style=flat&logo=telegram)](https://t.me/midoriwebes)

Midori is a lightweight yet powerful web browser which runs just as well on little embedded computers named for delicious pastries as it does on beefy machines with a core temperature exceeding that of planet earth. And it looks good doing that, built on top of <code>Electron</code> and <code>React</code>. It aims to be fast, private, beautiful, extensible and functional.
</div>

# Features

- ***Privacy out of the box***
  - Adblock filter list support
  - Private browsing
  - Manage cookies and scripts
<a/>

- **Chromium without Google services and low resources usage** - Since Midori Browser uses Electron under the hood which is based on only several and the most important Chromium components, it's not bloated with redundant Google tracking services and others.

- **Simple and Modern UI**

- **Fast and fluent UI** - The animations are really smooth and their timings are perfectly balanced.

- **Highly customizable new tab page** - Customize almost an every aspect of the new tab page!
- **Customizable browser UI** - Choose whether Midori Browser should have compact or normal UI.
- **Tab groups** - Easily group tabs, so it's hard to get lost.
- **Scrollable tabs**
- **Partial support for Chrome extensions** - Install some extensions directly from Chrome Web Store.
- **Packages** - Extend Midori Browser for your needs, by installing or developing your own packages and themes.

# Screenshots

![image](https://user-images.githubusercontent.com/11065386/81024159-d9388f80-8e72-11ea-85e7-6c30e3b66554.png)

UI normal variant:
![image](https://user-images.githubusercontent.com/11065386/81024186-f40b0400-8e72-11ea-976e-cd1ca1b43ad8.png)

UI compact variant:
![image](https://user-images.githubusercontent.com/11065386/81024222-13099600-8e73-11ea-9fc9-3c63a034403d.png)
![image](https://user-images.githubusercontent.com/11065386/81024252-2ddc0a80-8e73-11ea-9f2f-6c9a4a175c60.png)

# Downloads
- [Midori](astian.org/midori-browser/download)


# Contributing

If you have found any bugs or just want to see some new features in Midori Browser, feel free to open an issue. We're open to any suggestions. Bug reports would be really helpful for us and appreciated very much. Midori Browser is in heavy development and some bugs may occur. Also, please don't hesitate to open a pull request. This is really important to us and for the further development of this project.

## License

[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fgrupoastian%2Fmidori-desktop.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2Fgrupoastian%2Fmidori-desktop?ref=badge_large)

# FAQ

## Why Electron?
<div style="text-align: justify;text-justify: inter-word;">
<p>
Midori Browser has been hated by many people for using Electron by saying things like it's a web browser inside a web browser.

It's somewhat true, but technically it doesn't matter (also please keep in mind that browsers like Firefox also have the UI built with web technologies). It doesn't make the browser any slower or heavier, it's rather the opposite based on Midori Browser resources usage compared to Chrome.

Choosing Electron was the best option to build the browser. We can build the UI however we want and make the customization even better. We don't have enough resources to build Chromium for weeks and edit almost 25 million lines of code and search for weeks for example the code responsible for changing button icons. Instead we chose Electron which uses only several Chromium components required to properly display the external content without any of the Google services, which makes the browser much lighter. Despite using only some part of Chromium, it doesn't really affect on the browser functionalities. We can implement all of the Chromium features except the Google services.
</p>
</div>

This Midori renovation uses Wexond as a base but little by little it will be integrating many new innovations, thanks to Mert Gör. 
 
