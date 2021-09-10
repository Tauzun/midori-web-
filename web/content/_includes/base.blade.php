<!DOCTYPE html>
<html lang="en-GB">

	<head>
		<meta charset="UTF-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<meta name="description" content="@yield('pageDescription', $siteDescription)" />

		<title>{{$siteName}} @yield('pageTitle')</title>

		<link rel="shortcut icon" href="@url('assets/images/favicon.ico')" type="image/x-icon" sizes="16x16 32x32"/>
		<link rel="stylesheet" href="@url('assets/css/spectre.min.css')" />
		<link rel="stylesheet" href="@url('assets/css/spectre-icons.min.css')" />
		@yield('additional-css')
		<link rel="stylesheet" href="@url('assets/css/all.css')" />
	</head>

	<body>

		<div class="container">
			<div class="columns">
				<div class="column col-12"><div class="p-centered"><a href="@url('/')" class="internal-link"><img src="@url('assets/images/bhawk-blue.svg')" class="img-responsive height-128" alt="Blue Hawk Logo" /></a></div></div>
			</div>
		</div>

		<div class="container">
			<div class="columns">
				<div class="column col-6">@yield('body')
					<div class="columns">
						<div class="column col-6">
							<div class="column col-6">
								<div class="card" id="card-support-gl">
									<div class="card-header">
										<div class="card-title h5">Hosted by</div>
									</div>
									<div class="card-image col-mx-auto"><a href="https://about.gitlab.com/" class="external-link"><img src="@url('assets/images/gitlab-logo-gray-rgb.svg')" class="img-responsive  width-128" alt="Gitlab" /></a></div>
									<div class="card-footer"><form style="display: inline" action="https://gitlab.com/TW3/bhawk" method="get"><button class="btn btn-primary">Fork the code</button></form></div>
								</div>
							</div>
						</div>
						<div class="column col-6"></div>
					</div>
				</div>
				<div class="column col-6">
					<div class="right-side">@include('_includes.sidebar')</div>
				</div>
			</div>
		</div>
		<div class="container">
			<div class="columns">
				<div class="column col-12">
					<div id="footer"><span class="text-dark"><small>All trademarks are copyright their various owners. Text based content copyright © <time>{{ $footerDate }}</time> TW3. Unauthorized reproduction or recreation prohibited.</small></span></div>
				</div>
			</div>
		</div>

	</body>

</html>
