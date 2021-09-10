@extends('_includes.base')
@section('body')

    <div class="welcome">
        <div class="wrapper">
            <h1>Blue Hawk</h1>
            <header>
                <span>{{ $siteDescription }}</span>
            </header>
        </div>
    </div>
	<br />
    <div class="left-side"><main>
        @markdown

Engineered for the modern web.

[Learn about Blue Hawk](about/).

**For interested developers:** _Blue Hawk's source code is free software available via Gitlab.
The application is a fork of KDE's [Falkon Web browser](https://www.falkon.org/) and is a clean fork with absolutely zero backward compatibility.
If you patches or merge requests, please feel free to propose your work via Gitlab. Any and all forms of contribution are very welcome!_

        @endmarkdown
    </main></div>

@stop