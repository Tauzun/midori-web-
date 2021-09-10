/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "adblockaddsubscriptiondialog.h"
#include "ui_adblockaddsubscriptiondialog.h"
#include "adblockmanager.h"

AdBlockAddSubscriptionDialog::AdBlockAddSubscriptionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AdBlockAddSubscriptionDialog)
{
    ui->setupUi(this);

    m_knownSubscriptions << Subscription(tr("Lists..."), QString())
                         << Subscription(QStringLiteral("ABP Anti-Circumvention Filter List"), ADBLOCK_ABPACFL_URL)
                         << Subscription(QStringLiteral("Adblock Wikipedia"), ADBLOCK_AW_URL)
                         << Subscription(QStringLiteral("Adblock YouTube"), ADBLOCK_AY_URL)
                         << Subscription(QStringLiteral("Adware Filters"), ADBLOCK_AF_URL)
                         << Subscription(QStringLiteral("Adware Filter Block"), QStringLiteral("https://cdn.jsdelivr.net/gh/kano1/I/adware.txt"))
                         << Subscription(QStringLiteral("AnonymousPoster - Andblock Rules (Block Google)"), ADBLOCK_APAR_URL)
                         << Subscription(QStringLiteral("Anti-Facebook List"), ADBLOCK_AFLIST_URL)
                         << Subscription(QStringLiteral("Anti-Fake notification counters"), QStringLiteral("https://cdn.jsdelivr.net/gh/DandelionSprout/adfilt/Android%20Scum%20Class%20%E2%80%94%20Fake%20notification%20counters.txt"))
                         << Subscription(QStringLiteral("Clickbait Blocklist"), QStringLiteral("https://cdn.jsdelivr.net/gh/cpeterso/clickbait-blocklist/clickbait-blocklist.txt"))
                         << Subscription(QStringLiteral("Czech List (Czech)"), QStringLiteral("http://adblock.dajbych.net/adblock.txt"))
                         << Subscription(QStringLiteral("Distractions and Clickbait Filter"), ADBLOCK_DAC_URL)
                         << Subscription(QStringLiteral("EasyList"), ADBLOCK_ELIST_URL)
                         << Subscription(QStringLiteral("EasyList Cookie List"), ADBLOCK_ELCL_URL)
                         << Subscription(QStringLiteral("EasyPrivacy"), QStringLiteral("https://easylist.to/easylist/easyprivacy.txt"))
                         << Subscription(QStringLiteral("Fanboy's Annoyance List"), ADBLOCK_FBAL_URL)
                         << Subscription(QStringLiteral("I Don't Care about Cookies"), ADBLOCK_IDCAC_URL)
                         << Subscription(QStringLiteral("I don't care about newsletters"), ADBLOCK_IDCANL_URL)
                         << Subscription(QStringLiteral("I Don't Want to Download Your Browser"), QStringLiteral("https://cdn.jsdelivr.net/gh/DandelionSprout/adfilt/I%20Don't%20Want%20to%20Download%20Your%20Browser.txt"))
                         << Subscription(QStringLiteral("I Hate Overpromoted Games"), QStringLiteral("https://cdn.jsdelivr.net/gh/DandelionSprout/adfilt/IHateOverpromotedGames.txt"))
                         << Subscription(QStringLiteral("I Don't Want Your App"), QStringLiteral("https://cdn.jsdelivr.net/gh/lassekongo83/Frellwits-filter-lists/i-dont-want-your-app.txt"))
                         << Subscription(QStringLiteral("IsraelList (Hebrew)"), QStringLiteral("http://secure.fanboy.co.nz/israelilist/IsraelList.txt"))
                         << Subscription(QStringLiteral("Linked Insanity Annoyance Rules"), ADBLOCK_LIA_URL)
                         << Subscription(QStringLiteral("NLBlock (Dutch)"), QStringLiteral("http://www.verzijlbergh.com/adblock/nlblock.txt"))
                         << Subscription(QStringLiteral("Peter Lowe's list (English)"), ADBLOCK_PLIST_URL)
                         << Subscription(QStringLiteral("PLgeneral (Polish)"), QStringLiteral("http://www.niecko.pl/adblock/adblock.txt"))
                         << Subscription(QStringLiteral("Prebake Obtrusive"), ADBLOCK_PO_URL)
                         << Subscription(QStringLiteral("Schacks Adblock Plus liste (Danish)"), QStringLiteral("https://adblock.schack.dk/block.txt"))
                         << Subscription(QStringLiteral("Spam404"), ADBLOCK_S404_URL)
                         << Subscription(QStringLiteral("The Hosts File Project Adblock Filters"), ADBLOCK_HFPAF_URL)
                         << Subscription(QStringLiteral("Thoughtconverge's Custom AdBlock Filters (Block Google)"), ADBLOCK_TCAF_URL)
                         << Subscription(QStringLiteral("Twitch: Pure Viewing Experience"), ADBLOCK_TPVE_URL);

    for (const Subscription &subscription : qAsConst(m_knownSubscriptions)) {
        ui->comboBox->addItem(subscription.title);
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    indexChanged(0);
}

QString AdBlockAddSubscriptionDialog::title() const
{
    return ui->title->text();
}

QString AdBlockAddSubscriptionDialog::url() const
{
    return ui->url->text();
}

void AdBlockAddSubscriptionDialog::indexChanged(int index)
{
    const Subscription subscription = m_knownSubscriptions.at(index);

    // "Manually add..." entry
    if (subscription.url.isEmpty()) {
        ui->title->clear();
        ui->url->clear();
    }
    else {
        int pos = subscription.title.indexOf(QLatin1Char('('));
        QString title = subscription.title;

        if (pos > 0) {
            title = title.left(pos).trimmed();
        }

        ui->title->setText(title);
        ui->title->setCursorPosition(0);

        ui->url->setText(subscription.url);
        ui->url->setCursorPosition(0);
    }
}

AdBlockAddSubscriptionDialog::~AdBlockAddSubscriptionDialog()
{
    delete ui;
}
