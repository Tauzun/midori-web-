@extends('_includes.base')
@section('body')

    <div class="welcome">
        <div class="wrapper">
            <h1>Midori Browser</h1>
            <header>
                <span>{{ $siteDescription }}</span>
            </header>
        </div>
    </div>
	<br />
    <div class="left-side"><main>
        @markdown

Engineered for the modern web.

[Learn about Midori Browser](about/).

**For interested developers:** _Midori Browser source code is free software available via Gitlab.
The application is a fork of KDE's [Falkon Web browser](https://www.falkon.org/) and is a clean fork with absolutely zero backward compatibility.
If you have any patches or would like to propose a merge request then please feel free via Gitlab. Any and all forms of contribution are very welcome!_

        @endmarkdown
    </main></div>

@stop
