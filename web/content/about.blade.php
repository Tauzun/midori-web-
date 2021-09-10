@extends('_includes.base')
@section('pageTitle', '- About')
@section('additional-css')
<link rel="stylesheet" href="@url('assets/css/spectre-exp.min.css')" />
@stop
@section('body')

    <div class="left-side"><main>
        @markdown

# About

Blue Hawk is a Web browser for Windows 10. The application is considered experimental and comes with absolutely no warranty of any kind whatsoever; as permitted by applicable laws.

        @endmarkdown

<div class="col-8 col-mx-auto">

<div class="carousel">
  <!-- carousel locator -->
  <input class="carousel-locator" id="slide-1" type="radio" name="carousel-radio" hidden="" checked="" />
  <input class="carousel-locator" id="slide-2" type="radio" name="carousel-radio" hidden="" />
  <input class="carousel-locator" id="slide-3" type="radio" name="carousel-radio" hidden="" />
  <input class="carousel-locator" id="slide-4" type="radio" name="carousel-radio" hidden="" />
  
  <!-- carousel container -->
  <div class="carousel-container">
    <!-- carousel item -->
    <figure class="carousel-item">
      <label class="item-prev btn btn-action btn-lg bg-dark" for="slide-4"><i class="icon icon-arrow-left"></i></label>
      <label class="item-next btn btn-action btn-lg bg-dark" for="slide-2"><i class="icon icon-arrow-right"></i></label>
      <img class="img-responsive rounded" src="@url('assets/images/screens/cap1.png')" alt="Blue Hawk screen capture 1" />
    </figure>
    <figure class="carousel-item">
      <label class="item-prev btn btn-action btn-lg bg-dark" for="slide-1"><i class="icon icon-arrow-left"></i></label>
      <label class="item-next btn btn-action btn-lg bg-dark" for="slide-3"><i class="icon icon-arrow-right"></i></label>
      <img class="img-responsive rounded" src="@url('assets/images/screens/cap2.png')" alt="Blue Hawk screen capture 2" />
    </figure>
    <figure class="carousel-item">
      <label class="item-prev btn btn-action btn-lg bg-dark" for="slide-2"><i class="icon icon-arrow-left"></i></label>
      <label class="item-next btn btn-action btn-lg bg-dark" for="slide-4"><i class="icon icon-arrow-right"></i></label>
      <img class="img-responsive rounded" src="@url('assets/images/screens/cap3.png')" alt="Blue Hawk screen capture 3" />
    </figure>
    <figure class="carousel-item">
      <label class="item-prev btn btn-action btn-lg bg-dark" for="slide-3"><i class="icon icon-arrow-left"></i></label>
      <label class="item-next btn btn-action btn-lg bg-dark" for="slide-1"><i class="icon icon-arrow-right"></i></label>
      <img class="img-responsive rounded" src="@url('assets/images/screens/cap4.png')" alt="Blue Hawk screen capture 4" />
    </figure>
  </div>
  <!-- carousel navigation -->
  <div class="carousel-nav">
    <label class="nav-item text-hide c-hand text-dark" for="slide-1">1</label>
    <label class="nav-item text-hide c-hand text-dark" for="slide-2">2</label>
    <label class="nav-item text-hide c-hand text-dark" for="slide-3">3</label>
    <label class="nav-item text-hide c-hand text-dark" for="slide-4">4</label>
  </div>

</div>
<div class="text-center text-italic"><span class="text-dark">Application screen capture images</span></div>
</div>
<br />
<hr />
<br />
        @markdown

### System Requirements

To run Blue Hawk, your computer system must meet the following specs:

- Currently running any normal 64 bit edition of Windows 10 (not running in "S" mode)
- Contain a modern CPU (which supports [AVX2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#CPUs_with_AVX2))

### Installation Instructions

- Download a copy of the release archive by clicking on the button in the top right hand corner of this web page.
- Optionally, validate the downloaded archive's checksum. From the command prompt: `certUtil -hashfile bhawk.zip SHA256`
- Extract the downloaded zip archive to a location on your computer.
- Run the file **bhawk.exe** inside the program folder.

### Releases

There is no hard decision regarding the timing frequency of releases. Compiled binaries are offered for testing purposes and depend upon the external services which build them.

Release archives are created in an open, reproducible and transparent way. You can follow all the way from [commit](https://gitlab.com/TW3/bhawk/commits/staging) to [build](https://gitlab.com/TW3/bhawk/-/pipelines) via log files.
In short, this means that you can absolutely trust that the files you are downloading have been created by a clean source. Installing or using the software carries no hidden risks which could in any way knowingly or deliberately, compromise the security of your computer.

Releases will be made as and when ready. Usually based upon major feature completion or security updates. However, depending upon code churn; this may change at any time.

### Updates

There is currently no automatic update routine within the program. Automatic updates are a feature which may be added at some point in the future but there are currently no plans to do so just yet.

To update the program, simply copy the contents of a newer release archive over the top of an existing installation.

### Bug Reports

Please [report](https://gitlab.com/TW3/bhawk/issues) any bugs you find.

### Infrastructure

Code hosting is being provided by [Gitlab](https://about.gitlab.com/). As too also is the hosting of this web site.

### Project Steering/Direction

The application is written mainly using C++14, utilises the [QT](https://www.qt.io/) toolkit and [QtWebEngine](https://wiki.qt.io/QtWebEngine/ChromiumVersions).
The main priorities when making design and engineering decisions regarding the project going forward; are as follows:

- Dark mode by default (both UI and page rendering)
- A main toolbar javascript toggle button as default
- Ad/nuisance block support built in and enabled by default
- Tabs below the address bar as default
- Sensible default settings - with a focus on security and portability
- Attempt to provide [Flatpack](https://flatpak.org/) builds in the future for Linux users

> **_“You affect the world by what you browse.”_**
 > ― _Sir Tim Berners-Lee_

### Contact

Please use [Gitlab](https://gitlab.com/TW3/) for any and all communication. We would **love** to hear from you!

### Press

If you are a news reporter or other type of content creator, please feel free to use the graphic below or any of the screen captures at the top of this page to promote the application; within your own content. **Thanks in advance!**

        @endmarkdown

<img src="@url('assets/images/bhawk-blue.svg')" class="img-responsive height-256" alt="Blue Hawk Logo" />

    </main></div>

<hr />
<nav>
    <ul class="breadcrumb">
        <li class="breadcrumb-item">
            <a href="@url('/')" class="internal-link">Home</a>
        </li>
        <li class="breadcrumb-item">About</li>
    </ul>
</nav>
<hr />
<br />

@stop