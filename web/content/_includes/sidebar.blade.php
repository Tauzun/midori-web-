<section>
    <h3>Download</h3>
    <aside>
        <span>Grab the latest release here:</span>
        <br /><br />

        <div class="popover popover-bottom">
            <form style="display: inline" action="@url('assets/dl/'){{ $appArchive }}" method="get"><button class="btn btn-success">Blue Hawk <i class="icon icon-link"></i></button></form>
            <div class="popover-container">
                <div class="card bg-dark">
                    <div class="card-header">
                        <h3 class="text-light">Warning:</h3>
                    </div>
                    <div class="card-body"><span class="text-warning bg-dark">Blue Hawk is highly experimental and runs on 64 bit Microsoft Windows® 10 only.</span><br /><br /><span class="text-gray">Please <a href="https://gitlab.com/TW3/bhawk/issues" class="external-link">report</a> any bugs you find. <b>Thank You!</b></span></div>
                    <div class="card-footer"><i class="icon icon-download text-gray"></i></div>
                </div>
            </div>
        </div>

        <br /><br />
        <span>Blue Hawk is licensed under the terms of the <a href="https://www.gnu.org/licenses/gpl-3.0-standalone.html" class="external-link">GNU General Public License V3</a>.</span>
        <br />
        <span>The sha256 checksum for the file {{ $appArchive }} released on {{ $releaseDate }} is : </span>
        <br /><br />
        <div class="bg-dark" id="check-sum"><div class="text-bold"><span class="text-gray"><small>{{ $appChecksum }}</small></span></div></div>
        <br /><hr /><br />

        <div class="container">
            <div class="columns">
                <div class="column col-3">
	            </div>
            </div>
        </div>

        <br />
    </aside>
</section>